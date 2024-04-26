#include "pch.h"
#include "MirWorldRenderManager.h"
#include "SpriteHandleHolder.hpp"
#include "../mir_lib/MirConst.h"
#include "../mir_lib/MirUtility.h"
#include "MirCellView.h"

MirWorldRenderManager::MirWorldRenderManager(DX::DeviceResources* dr,
    shared_ptr<WilSpriteManager> mapResMgr,
    shared_ptr<WilSpriteManager> actorResMgr,
    shared_ptr<WilSpriteManager> itemResMgr):
    _dr{ dr }, _mapResMgr{ mapResMgr }, _actorResMgr{ actorResMgr }, _itemResMgr{ itemResMgr },
    _bgLayer{ 0,0 }, _mid1Layer{ 1,0 }, _mid2Layer{ 2,0 }, _topLayer{ 3,0 }, _debugLayer{ 4,0 },
    _animDB{}
{
    _sceneMgr = std::make_unique<SceneManager>();
    _animatorSystem = std::make_unique<AnimatorSystem>();
    _renderSystem = std::make_unique<SpriteRenderSystem>(dr->GetWindow(), dr->GetD3DDevice(), dr->GetD3DDeviceContext(),
        dr->GetRenderTargetView(), dr->GetDepthStencilView(), 
        Mir::GameLayoutW, Mir::GameLayoutH, DpiScale* UserScale);

    _renderSystem->Debug = true;
    _camera = _sceneMgr->CreateCameraNode();
    _camera->SetLocalPosition(Mir::GetCellCenter(400, 460));
    //
    _gridView = _camera->AddComponent<GridViewComponent>().lock();
    _animDB = std::make_unique<AnimDatabase>();
    _animDB->LoadFromXml("./ResDef/AnimData.xml");
}

void MirWorldRenderManager::SetMapData(shared_ptr<MapData> mapData)
{
    _mapData = mapData;
    _gridView->Init(Mir::CellW, Mir::CellH, _mapData->h(), _mapData->w(), [this](int rowIdx, int colIdx)-> CellView* {
        return new MirCellView(rowIdx, colIdx, this);
        });

    _gridView->GetView()->SetView(
        Mir::GameLayoutHHalf + Mir::TileH, Mir::GameLayoutHHalf + Mir::TileH * 8,
        Mir::GameLayoutWHalf + Mir::TileH * 4, Mir::GameLayoutWHalf + Mir::TileH * 4,
        Mir::TileW * 4, Mir::TileH * 4);
}

DirectX::XMINT2 MirWorldRenderManager::GetViewPoint()
{
    if (!_gridView)
        return DirectX::XMINT2();
    else
        return _gridView->GetSceneNode()->GetLocalPosition();
}

CellView* MirWorldRenderManager::GetViewPointCell()
{
    auto p = GetViewPoint();
    return _gridView->GetView()->GetCellView(p.x, p.y);
}

void MirWorldRenderManager::SetViewPoint(DirectX::XMINT2 coor)
{
    if (!_gridView)
        return;
    else
        _gridView->GetSceneNode()->SetLocalPosition(coor);
}

ActorView* MirWorldRenderManager::GetActorView(int id, function<shared_ptr<SceneNode>()> createFunc)
{
    auto f = _actorViews.find(id);
    if (f != _actorViews.end())
        return f->second;

    auto node = createFunc();
    auto v = node->GetComponent<ActorView>().lock()->As<ActorView>();
    _actorViews.insert({id, v});
    v->Layer = _mid2Layer.Layer;
    v->Depth = _mid2Layer.Depth;
    v->Debug = true;
    return v;
}

void MirWorldRenderManager::Update(DX::StepTimer const& timer)
{
    _animatorSystem->Update(timer.GetTotalTicks() / 10000, timer.GetElapsedTicks() / 10000);
}

void MirWorldRenderManager::Render()
{
    //_renderSystem->Render();// 不需要使用精灵排序，因此手动渲染。
    auto grid = _gridView->GetView();
    if (!grid)
        return;

    auto cameraCpt = _camera->GetComponent<Camera>().lock()->As<Camera>();
    if (cameraCpt->ClearType == Camera::ClearType::Color) {
        _renderSystem->Clear(cameraCpt->ClearColor);
    }

    auto cameraWPos = _camera->GetWorldPosition();
    auto _draw = _renderSystem->GetGraphic2D();

    _draw->Begin(cameraWPos, DpiScale * UserScale);

    
    for (int i = 0; i < (int)Layer::Max; i++)
    {
        grid->ViewCellForeach([i, _draw](CellView* cell) {
            MirCellView::DrawLayer(i, cell, _draw);
            });
    }
    
    _draw->End();
}

void MirWorldRenderManager::Clear()
{
    _mapData = nullptr;
    _sceneMgr->Clear();
}

void MirWorldRenderManager::AddHero(HeroData& data)
{
    _heros[data.NetId] = data;
    ActorView* view = nullptr;
    auto s = GetActorView(data.NetId, _sceneMgr->GetSpawnPlayerFunctor());
    s->Clear();
    s->Type = data.Gender;
    s->GetSceneNode()->SetLocalPosition(Mir::GetCellCenter(data.Pos.x, data.Pos.y));
    s->InitAs(data, *_animDB, _actorResMgr.get());
    s->PlayAnim(std::to_string((int)data.Motion));
}

void MirWorldRenderManager::SetSelfHero(HeroData& data)
{
    AddHero(data);
    auto s = GetActorView(data.NetId, _sceneMgr->GetSpawnPlayerFunctor());
    s->GetSceneNode()->SetParent(_camera);
    s->GetSceneNode()->ResetTRS();
    s->GetSceneNode()->SetLocalPosition({ -Mir::CellWHalf, -Mir::CellHHalf });
    SetSelfHeroDirection(data.Dir);
    _selfHeroId = data.NetId;
    SetViewPoint(data.Pos);
}

void MirWorldRenderManager::SetSelfHeroDirection(Mir::Direction dir)
{
    _heros[_selfHeroId].Dir = dir;
    auto s = GetActorView(_selfHeroId, _sceneMgr->GetSpawnPlayerFunctor());
    s->SetDirAndMotion((int)_heros[_selfHeroId].Dir, (int)_heros[_selfHeroId].Motion);
}

void MirWorldRenderManager::SetSelfHeroMotion(Mir::PlayerMotion motion)
{
    _heros[_selfHeroId].Motion = motion;
    auto s = GetActorView(_selfHeroId, _sceneMgr->GetSpawnPlayerFunctor());
    s->SetDirAndMotion((int)_heros[_selfHeroId].Dir, (int)_heros[_selfHeroId].Motion);
}

void MirWorldRenderManager::ClearScreen(XMFLOAT4 color)
{
    auto context = _dr->GetD3DDeviceContext();
    auto renderTarget = _dr->GetRenderTargetView();
    auto depthStencil = _dr->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, (const FLOAT*)&color);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

extern void DrawTexture(ID3D11ShaderResourceView* srv, RECT rect);

void MirWorldRenderManager::Draw(ID3D11ShaderResourceView* srv, DirectX::SimpleMath::Rectangle viewRect, XMFLOAT4 color)
{
    // 目地：观察坐标->屏幕坐标

    int offsetX = Mir::GameLayoutWHalf;
    int offsetY = -Mir::GameLayoutHHalf;
    // 变换到左上角
    viewRect.x += offsetX;
    viewRect.y += offsetY;
    // y轴反向
    viewRect.y *= -1;
    RECT sRect = { viewRect.x, viewRect.y - viewRect.height, viewRect.x + viewRect.width, viewRect.y };
    // todo 支持精灵颜色
    DrawTexture(srv, sRect);
}
