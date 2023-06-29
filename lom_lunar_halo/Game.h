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

    inline std::shared_ptr<Setting> GetSetting() { return _setting; }
    inline DX::DeviceResources* GetDeviceResource() { return m_deviceResources.get(); }
    void DrawTexture(ID3D11ShaderResourceView* srv, RECT rect);
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

    std::shared_ptr<YX::GUI::Canvas> _testMir3;
};
