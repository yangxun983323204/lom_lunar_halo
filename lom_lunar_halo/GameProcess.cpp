#include "pch.h"
#include "GameProcess.h"
#include "Game.h"
#include "StringCodec.hpp"
#include "../DirectXTK-main/Inc/Keyboard.h"
#include "ActorData.h"
#include <format>

using DirectX::Keyboard;

void GameProcess::StartEnter()
{
    _state = IProcess::State::Entering;
    _game->SetWindowAsGame();
    auto mapData = std::make_shared<MapData>();
    mapData->Load(_W(_game->GetSetting()->GetMapDir() + "1.map"));
    if (mapData->IsLoaded()) {
        string sizeStr = "map size:" + std::to_string(mapData->w()) + "," + std::to_string(mapData->h());
        MessageBox(_game->GetDeviceResource()->GetWindow(), _W(sizeStr).c_str(), _W("测试map加载").c_str(), 0);
        _game->GetWorldRenderManager()->SetMapData(mapData);
    }
    else {
        MessageBox(_game->GetDeviceResource()->GetWindow(), _W("加载map失败").c_str(), _W("测试map加载").c_str(), 0);
    }
    // 测试添加一个角色
    HeroData hero{};
    hero.NetId = 1;
    hero.Name = "小明";
    hero.Pos = { 422,429 };
    hero.Dir = Mir::Direction::Bottom;
    hero.Dress = 8;
    hero.Gender = Mir::ActorType::Woman;
    hero.Hair = 1;
    hero.Weapon = 19;
    hero.Horse = 0;
    hero.Motion = Mir::PlayerMotion::Walk;
    _game->GetWorldRenderManager()->SetSelfHero(hero);
    //
    string rmlPath = _game->GetSetting()->GetUILayoutDir() + "game.rml";
    _game->GetRmlUiAdapter()->GetContext()->LoadDocument(rmlPath)->Show();

	_state = IProcess::State::Entered;
}

void GameProcess::StartExit()
{
    _state = IProcess::State::Exiting;
    _game->GetRmlUiAdapter()->GetContext()->UnloadAllDocuments();
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
        _game->GetWorldRenderManager()->SetSelfHeroDirection(Mir::Direction::Left);
    }
    else if (state.IsKeyDown(Keyboard::Keys::D)) {
        auto pos = _game->GetWorldRenderManager()->GetViewPoint();
        pos.x += 4;
        _game->GetWorldRenderManager()->SetViewPoint(pos);
        _game->GetWorldRenderManager()->SetSelfHeroDirection(Mir::Direction::Right);
    }
    else if (state.IsKeyDown(Keyboard::Keys::W)) {
        auto pos = _game->GetWorldRenderManager()->GetViewPoint();
        pos.y += 4;
        _game->GetWorldRenderManager()->SetViewPoint(pos);
        _game->GetWorldRenderManager()->SetSelfHeroDirection(Mir::Direction::Top);
    }
    else if (state.IsKeyDown(Keyboard::Keys::S)) {
        auto pos = _game->GetWorldRenderManager()->GetViewPoint();
        pos.y -= 4;
        _game->GetWorldRenderManager()->SetViewPoint(pos);
        _game->GetWorldRenderManager()->SetSelfHeroDirection(Mir::Direction::Bottom);
    }

    auto vp = _game->GetWorldRenderManager()->GetViewPoint();
    float x = (float)vp.x / Mir::CellW;
    float y = (float)vp.y / Mir::CellH;
    auto debug = _game->GetRmlUiAdapter()->GetContext()->GetDocument(0)->GetElementById("debug");
    auto str = std::format("视点:({0},{1}), fps:{2}", x, y, timer.GetFramesPerSecond());
    debug->SetInnerRML(str);
}

void GameProcess::GetWindowSize(int& w, int& h)
{
	w = Mir::GameLayoutW;
	h = Mir::GameLayoutH;
}
