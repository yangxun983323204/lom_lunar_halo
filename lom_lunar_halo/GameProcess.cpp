#include "pch.h"
#include "GameProcess.h"
#include "Game.h"
#include "StringCodec.hpp"
#include "../DirectXTK-main/Inc/Keyboard.h"

using DirectX::Keyboard;

void GameProcess::StartEnter()
{
    _state = IProcess::State::Entering;
    _game->SetWindowSize(Mir::GameLayoutW, Mir::GameLayoutH);
    auto mapData = std::make_shared<MapData>();
    mapData->Load(_W(_game->GetSetting()->GetMapDir() + "0.map"));
    if (mapData->IsLoaded()) {
        string sizeStr = "map size:" + std::to_string(mapData->w()) + "," + std::to_string(mapData->h());
        MessageBox(_game->GetDeviceResource()->GetWindow(), _W(sizeStr).c_str(), _W("测试map加载").c_str(), 0);
        _game->GetWorldRenderManager()->SetMapData(mapData);
    }
    else {
        MessageBox(_game->GetDeviceResource()->GetWindow(), _W("加载map失败").c_str(), _W("测试map加载").c_str(), 0);
    }

	_state = IProcess::State::Entered;
}

void GameProcess::StartExit()
{
    _state = IProcess::State::Exiting;
    _game->GetWorldRenderManager()->Clear();
	_state = IProcess::State::Exited;
}

void GameProcess::Update(DX::StepTimer const& timer)
{
    auto state = Keyboard::Get().GetState();
    if (state.IsKeyDown(Keyboard::Keys::A))
    {
        auto pos = _game->GetWorldRenderManager()->GetViewPoint();
        pos.x -= 4;
        _game->GetWorldRenderManager()->SetViewPoint(pos);
    }
    else if (state.IsKeyDown(Keyboard::Keys::D)) {
        auto pos = _game->GetWorldRenderManager()->GetViewPoint();
        pos.x += 4;
        _game->GetWorldRenderManager()->SetViewPoint(pos);
    }
    else if (state.IsKeyDown(Keyboard::Keys::W)) {
        auto pos = _game->GetWorldRenderManager()->GetViewPoint();
        pos.y += 4;
        _game->GetWorldRenderManager()->SetViewPoint(pos);
    }
    else if (state.IsKeyDown(Keyboard::Keys::S)) {
        auto pos = _game->GetWorldRenderManager()->GetViewPoint();
        pos.y -= 4;
        _game->GetWorldRenderManager()->SetViewPoint(pos);
    }
}

void GameProcess::GetWindowSize(int& w, int& h)
{
	w = Mir::GameLayoutW;
	h = Mir::GameLayoutH;
}
