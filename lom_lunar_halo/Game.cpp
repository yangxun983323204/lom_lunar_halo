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
#include "SceneManager.h"
#include "StringCodec.hpp"
#include <format>

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;
// test scene
std::shared_ptr<SceneNode> _testScene;
std::shared_ptr<SceneManager> _sceneManager;

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
    elapsedTime;
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
    _batch->Begin();
    _sceneManager->Render();
    //YX::GUI::Canvas::DrawAll();
    _batch->End();

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
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    width = LoginLayoutW;
    height = LoginLayoutH;
}

void Game::GetCurrSize(int& width, int& height) const noexcept
{
    // todo 根据当前游戏状态，返回登陆布局大小或游戏布局大小
    width = LoginLayoutW;
    height = LoginLayoutH;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
    _batch.reset(new SpriteBatch{ m_deviceResources->GetD3DDeviceContext()});

    _testMir3 = YX::GUI::LayoutLoader::Parse(_setting->GetUILayoutDir() + L"login.xml");
    // 测试场景渲染
    _sceneManager = std::make_shared<SceneManager>(m_deviceResources.get());
    int w, h;
    GetCurrSize(w, h);
    _sceneManager->SetScreen(w, h);
    _testScene = _sceneManager->CreateSpriteNode();
    _testScene->SetLocalPosition({ 0,0 });
    auto cameraNode = _sceneManager->CreateCameraNode();
    cameraNode->SetLocalPosition({ int(w / 4),int(h / 4) });
    //
    ImageLib imgLib{};
    imgLib.Open(GetSetting()->GetDataDir() + L"Interface1c.wil");
    //assert(imgLib.IsOpened());
    if (imgLib.IsOpened())
    {
        int i = 0;
        auto info = imgLib.GetImageInfo(i);
        auto sp = dynamic_cast<SpriteRenderer*>(_testScene->GetComponent(SpriteRenderer::TypeId).lock().get())->Sprite;
        sp->Rect = { 0,0,long(info.Width * 0.5), long(info.Height * 0.5) };
        auto rgba32 = imgLib.GetImageRGBA32(i);
        imgLib.Close();
        D3D11_SUBRESOURCE_DATA subData{ 0 };
        subData.pSysMem = rgba32.data();
        subData.SysMemPitch = info.Width * 4;
        DX::ThrowIfFailed(DirectX::CreateTextureFromMemory(
            device,
            info.Width, info.Height,
            DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
            (const D3D11_SUBRESOURCE_DATA)subData,
            nullptr, sp->TextureSRV.GetAddressOf()));
    }
    // 测试地图加载
    auto mapData = std::make_unique<MapData>();
    mapData->Load(GetSetting()->GetMapDir() + L"0.map");
    wstring sizeStr = L"map size:" + std::to_wstring(mapData->w()) + L"," + std::to_wstring(mapData->h());
    MessageBox(m_deviceResources->GetWindow(), sizeStr.c_str(), L"测试map加载", 0);
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
