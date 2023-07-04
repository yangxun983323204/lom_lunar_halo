//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "../DirectXTK-main/Inc/SpriteBatch.h"
#include "Sprite_old.h"
#include "./GUI/Canvas.h"
#include "Setting.h"
#include "../mir_lib/MirLib.h"
#include "MirWorldRenderManager.h"
#include "WilSpriteManager.h"
#include "SpriteHandleHolder.hpp"
#include "ProcessManager.h"

using namespace Mir;

using Microsoft::WRL::ComPtr;
using DX::ThrowIfFailed;


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:
    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;
    void GetCurrSize(int& width, int& height) const noexcept;
    void SetWindowSize(int width, int height);

    inline std::shared_ptr<Setting> GetSetting() { return _setting; }
    inline DX::DeviceResources* GetDeviceResource() { return m_deviceResources.get(); }
    void DrawTexture(ID3D11ShaderResourceView* srv, RECT rect);
    inline std::shared_ptr<MirWorldRenderManager> GetWorldRenderManager() { return _worldRenderMgr; }
    inline std::shared_ptr<WilSpriteManager> GetMapSpriteManager() { return _mapSpriteManager; }
    inline std::shared_ptr<WilSpriteManager> GetActorSpriteManager() { return _actorSpriteManager; }
    inline std::shared_ptr<WilSpriteManager> GetUiSpriteManager() { return _uiSpriteManager; }
private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;
    
    std::shared_ptr<Setting> _setting;
    std::unique_ptr<DirectX::SpriteBatch> _batch;

    std::shared_ptr<MirWorldRenderManager> _worldRenderMgr;
    std::shared_ptr<WilSpriteManager> _mapSpriteManager;
    std::shared_ptr<WilSpriteManager> _actorSpriteManager;
    std::shared_ptr<WilSpriteManager> _uiSpriteManager;

    std::shared_ptr<ProcessManager> _procMgr;
};
