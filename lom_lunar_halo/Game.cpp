//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <sys/stat.h>
#include "../DirectXTK-main/Inc/BufferHelpers.h"
#include "../mir_lib/ImageLib.h"
#include "../mir_lib/MapData.h"
#include <string>
#include "./GUI/Image.h"
#include "./GUI/Text.h"
#include "./GUI/LayoutLoader.h"
#include "StringCodec.hpp"
#include <format>

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false):
    _batch{}
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    _setting = std::make_shared<Setting>();
    _setting->Load();

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    auto state = Keyboard::Get().GetState();
    gKBTracker.Update(state);
    if (_procMgr)
        _procMgr->Update(timer);

    _worldRenderMgr->Update(timer);
    _rmlUiAdapter->Update();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    _worldRenderMgr->Render();

    _batch->Begin();
    YX::GUI::Canvas::DrawAll();
    _batch->End();

    _rmlUiAdapter->Render();
    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::Black);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
    _rmlUiAdapter->OnWindowSizeChanged(width, height);
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    width = LoginLayoutW;
    height = LoginLayoutH;
}

void Game::GetCurrSize(int& width, int& height) const noexcept
{
    if (!_procMgr) {
        GetDefaultSize(width, height);
        return;
    }

    _procMgr->GetCurrentProcess()->GetWindowSize(width, height);
}

extern long style;
void Game::SetWindowSize(int width, int height)
{
    RECT oldRc;
    GetWindowRect(GetDeviceResource()->GetWindow(), &oldRc);

    RECT rc = { 0, 0, static_cast<LONG>(DPI_S(width)), static_cast<LONG>(DPI_S(height)) };
    AdjustWindowRect(&rc, style, FALSE);
    SetWindowPos(GetDeviceResource()->GetWindow(), 0, oldRc.left, oldRc.top, rc.right - rc.left, rc.bottom - rc.top, 0);
    OnWindowSizeChanged(width, height);
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
    _batch.reset(new SpriteBatch{ m_deviceResources->GetD3DDeviceContext()});
    _states = std::make_unique<CommonStates>(m_deviceResources->GetD3DDevice());
    _mapSpriteManager = std::make_shared<WilSpriteManager>(device, _setting->GetRootDir());
    _mapSpriteManager->SetCapacity(100);
    _actorSpriteManager = std::make_shared<WilSpriteManager>(device, _setting->GetRootDir());
    _actorSpriteManager->SetCapacity(100);
    _itemSpriteManager = std::make_shared<WilSpriteManager>(device, _setting->GetRootDir());
    _itemSpriteManager->SetCapacity(100);
    _uiSpriteManager = std::make_shared<WilSpriteManager>(device, _setting->GetRootDir());
    _uiSpriteManager->SetCapacity(50);

    _worldRenderMgr = std::make_shared<MirWorldRenderManager>(m_deviceResources.get(), 
        _mapSpriteManager, _actorSpriteManager, _itemSpriteManager);
    
    _rmlUiAdapter = std::make_shared<YX::RmlUiAdapter>(this);
    _procMgr = std::make_shared<ProcessManager>(this);
    _procMgr->StartEnter(_procMgr->GetLoginProcess());
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

void Game::DrawTexture(ID3D11ShaderResourceView* srv, RECT rect)
{
    rect.left = DPI_S(rect.left);
    rect.right = DPI_S(rect.right);
    rect.bottom = DPI_S(rect.bottom);
    rect.top = DPI_S(rect.top);
    _batch->Draw(srv, rect);
}
