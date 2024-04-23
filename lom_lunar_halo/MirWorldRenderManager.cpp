#include "pch.h"
#include "MirWorldRenderManager.h"
#include "SpriteHandleHolder.hpp"
#include "../mir_lib/MirConst.h"
#include "../mir_lib/MirUtility.h"

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
    _gridView->Init(Mir::CellW, Mir::CellH, _mapData->h(), _mapData->w());
    _gridView->GetView()->SetCellHideCallback([this](int x, int y) {
        if (x < 0 || y < 0)
            return;
        CellCoord key = { (uint32_t)x,(uint32_t)y };
        //CellCoord bgKey = { (uint32_t)x / 2 * 2,(uint32_t)y / 2 * 2 };
        auto& nodes = _usedSceneNodes[key];
        for (auto i : nodes)
        {
            _sceneMgr->ReleaseByInnerTag(i);
        }
        nodes.clear();
        });

    _gridView->GetView()->SetCellShowCallback([this](int x, int y) {
        if (x < 0 || y < 0)
            return;
        //auto log = L"{" + std::to_wstring(x) + L"," + std::to_wstring(y) + L"}";
        //OutputDebugString(log.c_str());

        CellCoord key = { (uint32_t)x,(uint32_t)y };
        SetUpBg(key);
        SetUpMid(key, 1);// 物体左半
        SetUpMid(key, 2);// 物体右半
        SetUpDebug(key);
        });

    _gridView->GetView()->SetCellWillShowCallback([](int x, int y) {
        if (x < 0 || y < 0)
            return;
        });

    _gridView->GetView()->SetView(
        Mir::GameLayoutHHalf + Mir::TileH, Mir::GameLayoutHHalf * 4,
        Mir::GameLayoutWHalf * 2, Mir::GameLayoutWHalf * 2,
        Mir::TileW * 4, Mir::TileH * 4);
}

DirectX::XMINT2 MirWorldRenderManager::GetViewPoint()
{
    if (!_gridView)
        return DirectX::XMINT2();
    else
        return _gridView->GetSceneNode()->GetLocalPosition();
}

void MirWorldRenderManager::SetViewPoint(DirectX::XMINT2 coor)
{
    if (!_gridView)
        return;
    else
        _gridView->GetSceneNode()->SetLocalPosition(coor);
}

SpriteRenderer* MirWorldRenderManager::GetMapStaticSpriteRenderer(SpriteRenderLayer& use, CellCoord key)
{
    auto sr = _sceneMgr->SpawnStaticSprite()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
    sr->SortLayer = use.Layer;
    sr->Depth = use.Depth;
    sr->Enable = true;
    sr->Debug = false;
    return sr;
}

SpriteRenderer* MirWorldRenderManager::GetMapAnimSpriteRenderer(SpriteRenderLayer& use, CellCoord key)
{
    auto sr = _sceneMgr->SpawnAnimSprite()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
    sr->GetSceneNode()->GetComponent<Animator>().lock()->As<Animator>()->Enable = true;
    sr->SortLayer = use.Layer;
    sr->Depth = use.Depth;
    sr->Enable = true;
    sr->Debug = false;
    return sr;
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
    _renderSystem->Render();
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
    _selfHeroId = data.NetId;
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

void MirWorldRenderManager::SetUpBg(CellCoord key)
{
    if (key.x % 2!=0 || key.y %2!=0)// 一个tile最多会重复触发4次绘制，检查一下是否已经绘制过了
        return;

    key.x = key.x / 2 * 2;// tile是2*2个cell，所以绘制时cell需对齐到tile
    key.y = key.y / 2 * 2;

    auto tile = this->_mapData->TileAt(key.x, _mapData->h() - key.y);
    int fileIdx = tile.FileIndex;
    int imgIdx = tile.TileIndex;
    if (!tile.RemapFileIndex(fileIdx))
        return;
    auto spriteHandle = _mapResMgr->LoadSprite({ (uint32_t)fileIdx, (uint32_t)imgIdx });
    if (!spriteHandle)
        return;

    auto spRender = _sceneMgr->SpawnStaticSprite()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
    spRender->SortLayer = _bgLayer.Layer;
    spRender->Depth = _bgLayer.Depth;
    spRender->Enable = true;
    spRender->Debug = false;
    _usedSceneNodes[key].push_back(spRender->GetSceneNode()->weak_from_this());

    spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellMin(key.x, key.y));
    spRender->Sprite = spriteHandle->GetSprite();
    spRender->Sprite.lock()->Pivot = { 0, 0 };
    spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();
    spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Add(spriteHandle);
}

void MirWorldRenderManager::SetUpMid(CellCoord key,int i)
{
    auto cell = this->_mapData->CellAt(key.x, _mapData->h() - key.y);
    if (!cell.FileEnableOf(i))
        return;

    auto spRender = _sceneMgr->SpawnAnimSprite()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
    spRender->GetSceneNode()->GetComponent<Animator>().lock()->As<Animator>()->Enable = true;
    spRender->Enable = true;
    spRender->Debug = false;
    spRender->DebugColor = { 1,0,0,1 };
    _usedSceneNodes[key].push_back(spRender->GetSceneNode()->weak_from_this());

    spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellMin(key.x, key.y));
    spRender->GetSceneNode()->GetComponent<Animator>().lock()->Enable = false;
    spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();


    int fileIdx = cell.FileIndexOf(i);
    if (!cell.RemapFileIndex(fileIdx))
        return;

    auto imgIdx = cell.ImgIndexOf(i);
    auto blend = false;

    if (cell.HasDoor())
        imgIdx += cell.DoorImgIdx();

    if (cell.HasAnimOf(i))
    {
        if (cell.ObjBlendOf(i))
            blend = true;

        auto animator = spRender->GetSceneNode()->GetComponent<Animator>().lock()->As<Animator>();
        animator->Enable = true;
        animator->Clear();
        auto spHolder = spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>();
        spHolder->Clear();
        Animation anim{};

        auto frameCount = cell.AnimCountOf(i);
        auto animSpanMs = Mir::TileAnimSpanMs[cell.AnimTickTypeOf(i)];
        for (short i = 0; i < frameCount; i++)
        {
            auto idx = imgIdx + i;
            auto spriteHandle = _mapResMgr->LoadSprite({ (uint32_t)fileIdx, (uint32_t)idx });
            if (!spriteHandle) {
                anim.AddFrame({});
                continue;
            }
            spriteHandle->GetSprite()->Pivot = { 0, 0 };
            anim.AddFrame(spriteHandle->GetSprite());
            spHolder->Add(spriteHandle);
        }
        animator->Add("0",anim);
        animator->SetCurrent("0");
        return;
    }
    // 地表物体图片的原点是左下角,对齐的也是cell的左下角
    auto spriteHandle = _mapResMgr->LoadSprite({ (uint32_t)fileIdx, (uint32_t)imgIdx });
    if (!spriteHandle)
        return;

    auto sp = spriteHandle->GetSprite();
    spRender->Sprite = sp;
    spRender->Sprite.lock()->Pivot = { 0, 0 };

    if (sp->Rect.width == 48 && sp->Rect.height == 32) {
        spRender->SortLayer = _mid1Layer.Layer;// 对于地图上的装饰，如果它巧好是一个cell大小时，它相当于第二层背景。
        spRender->Depth = _mid1Layer.Depth;
    }
    else {
        spRender->SortLayer = _topLayer.Layer;// 否则它是覆盖层
        spRender->Depth = _topLayer.Depth;
    }
    spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Add(spriteHandle);
}

void MirWorldRenderManager::SetUpDebug(CellCoord key) 
{
    auto spRender = _sceneMgr->SpawnStaticSprite()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
    spRender->SortLayer = _debugLayer.Layer;
    spRender->Depth = _debugLayer.Depth;
    spRender->Enable = DebugBarrier && !_mapData->CellAt(key.x, _mapData->h() - key.y).Walkable();
    spRender->Debug = false;
    spRender->Color = { 1,0,0,0.2 };
    _usedSceneNodes[key].push_back(spRender->GetSceneNode()->weak_from_this());

    spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellCenter(key.x, key.y));
    spRender->OverrideWidth = 48;
    spRender->OverrideHeight = 32;
    spRender->Sprite = _renderSystem->_debugImgWhite;
    spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();
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
