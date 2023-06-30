#include "pch.h"
#include "MirWorldRenderManager.h"
#include "SpriteHandleHolder.hpp"
#include "../mir_lib/MirConst.h"
#include "../mir_lib/MirUtility.h"

MirWorldRenderManager::MirWorldRenderManager(DX::DeviceResources* dr, shared_ptr<WilSpriteManager> spriteMgr):
    _dr{ dr }, _spriteMgr{spriteMgr},
    _bgUse{0}, _mid1Use{1}, _mid2Use{1}, _topUse{2}
{
    _sceneMgr = std::make_unique<SceneManager>();
    _renderSystem = std::make_unique<SpriteRenderSystem>();
    auto clearFunc = std::bind(&MirWorldRenderManager::ClearScreen, this, std::placeholders::_1);
    _renderSystem->SetClearFunc(clearFunc);
    auto renderFunc = std::bind(&MirWorldRenderManager::Draw, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    _renderSystem->SetRenderFunc(renderFunc);

    auto cameraNode = _sceneMgr->CreateCameraNode();
    cameraNode->SetLocalPosition(Mir::GetCellCenter(400, 400));
    //
    _gridView = cameraNode->AddComponent<GridViewComponent>().lock();
}

void MirWorldRenderManager::SetMapData(shared_ptr<MapData> mapData)
{
    _mapData = mapData;
    _gridView->Init(Mir::CellW, Mir::CellH, _mapData->h(), _mapData->w());
    _gridView->GetView()->SetCellHideCallback([this](int x, int y) {
        WilSpriteKey key = { (uint32_t)x,(uint32_t)y };
        this->ReleaseSpriteRenderer(_bgUse, key);
        this->ReleaseSpriteRenderer(_mid1Use, key);
        this->ReleaseSpriteRenderer(_mid2Use, key);
        this->ReleaseSpriteRenderer(_topUse, key);
        });

    _gridView->GetView()->SetCellShowCallback([this](int x, int y) {
        auto log = L"{" + std::to_wstring(x) + L"," + std::to_wstring(y) + L"}";
        OutputDebugString(log.c_str());

        WilSpriteKey key = { (uint32_t)x,(uint32_t)y };
        SetUpBg(key);
        SetUpMid(key, 1, _mid1Use);// 物体左半
        SetUpMid(key, 2, _mid2Use);// 物体右半
        });

    _gridView->GetView()->SetCellWillShowCallback([](int x, int y) {
        });

    _gridView->GetView()->SetView(Mir::GameLayoutW + Mir::CellW, Mir::GameLayoutH + Mir::CellH, Mir::CellW * 4, Mir::CellH * 4);
}

SpriteRenderer* MirWorldRenderManager::GetSpriteRenderer(SpriteRenderLayer& use, WilSpriteKey key)
{
    auto f = use.Record.find(key);
    if (f != use.Record.end())
        return f->second;

    SpriteRenderer* sr = nullptr;
    if (_pool.size() > 0) {
        SpriteRenderer* last = *(--_pool.end());
        _pool.pop_back();
    }
    else {
        sr = _sceneMgr->CreateSpriteNode()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
        sr->GetSceneNode()->AddComponent<SpriteHandleHolder>();
    }

    sr->SortLayer = use.Layer;
    sr->Enable = true;
    use.Record[key] = sr;
    return sr;
}

void MirWorldRenderManager::ReleaseSpriteRenderer(SpriteRenderLayer& use, WilSpriteKey key)
{
    auto f = use.Record.find(key);
    if (f == use.Record.end())
        return;

    SpriteRenderer* sr = f->second;
    use.Record.erase(key);
    _pool.push_back(sr);
    sr->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->holder.clear();
    sr->Enable = false;
}

void MirWorldRenderManager::SetUpBg(WilSpriteKey key)
{
    if (key.x % 2 != 0 || key.y % 2 != 0)
        return;

    auto tile = this->_mapData->TileAt(key.x, _mapData->h() - key.y);
    auto fileIdx = tile.FileIndex;
    int imgIdx = tile.TileIndex;
    //if (tile.RemapFileIndex(imgIdx))
    //    return;
    auto spriteHandle = _spriteMgr->LoadSprite({ fileIdx, (uint32_t)imgIdx });
    if (!spriteHandle)
        return;
    auto spRender = this->GetSpriteRenderer(_bgUse, key);
    spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellMin(key.x, key.y));
    spRender->Sprite = spriteHandle->GetSprite();
    spRender->Sprite.lock()->Pivot = { 0, 0 };
    spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->holder.push_back(spriteHandle);
}

void MirWorldRenderManager::SetUpMid(WilSpriteKey key,int i, SpriteRenderLayer use)
{
    auto cell = this->_mapData->CellAt(key.x, _mapData->h() - key.y);
    if (!cell.FileEnableOf(i))
        return;

    auto fileIdx = cell.FileIndexOf(i);
    auto imgIdx = cell.ImgIndexOf(i);
    auto blend = false;
    if (fileIdx > 2 && fileIdx < 14) {
        if (cell.HasDoor())
            imgIdx += cell.DoorImgIdx();
        /*if (cell.HasAnimOf(i))
        {
            if (cell.ObjBlendOf(i))
                blend = true;
            imgIdx += _animTileState[cell.Obj1AnimTickType()][cell.Obj1AnimCount()];
        }*/
        // 地表物体图片的原点是左下角,对齐的也是cell的左下角
        auto spriteHandle = _spriteMgr->LoadSprite({ fileIdx, (uint32_t)imgIdx });
        if (!spriteHandle)
            return;
        auto spRender = this->GetSpriteRenderer(use, key);
        spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellMin(key.x, key.y));
        spRender->Sprite = spriteHandle->GetSprite();
        spRender->Sprite.lock()->Pivot = { 0, 0 };
        spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->holder.push_back(spriteHandle);
    }
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
    // 观察坐标->屏幕坐标
    int offsetX = Mir::GameLayoutWHalf;
    int offsetY = Mir::GameLayoutHHalf;
    // 变换到左下角
    viewRect.x += offsetX;
    viewRect.y += offsetY;
    // 变换到左上角
    viewRect.y = Mir::GameLayoutH - viewRect.y;
    RECT sRect = { viewRect.x, viewRect.y - viewRect.height, viewRect.x + viewRect.width, viewRect.y };
    // todo 支持精灵颜色
    DrawTexture(srv, sRect);
}
