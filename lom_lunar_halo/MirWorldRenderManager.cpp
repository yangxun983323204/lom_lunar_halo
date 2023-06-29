#include "pch.h"
#include "MirWorldRenderManager.h"
#include "SpriteHandleHolder.hpp"
#include "../mir_lib/MirConst.h"
#include "../mir_lib/MirUtility.h"

MirWorldRenderManager::MirWorldRenderManager(DX::DeviceResources* dr, shared_ptr<WilSpriteManager> spriteMgr):
    _dr{ dr }, _spriteMgr{spriteMgr},
    _bgUse{0}, _mid1Use{1}, _mid2Use{1}, _topUse{2}
{
    _sceneMgr = std::make_unique<SceneManager>(_dr);
    _sceneMgr->SetScreen(Mir::GameLayoutW, Mir::GameLayoutH);
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
        WilSpriteKey key = { (uint32_t)x,(uint32_t)y };
        DrawBg(key);
        DrawMid(key, 1, _mid1Use);// 物体左半
        DrawMid(key, 2, _mid2Use);// 物体右半
        });

    _gridView->GetView()->SetCellWillShowCallback([](int x, int y) {
        });

    _gridView->GetView()->SetView(Mir::GameLayoutW, Mir::GameLayoutH, Mir::CellW * 4, Mir::CellH * 4);
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

void MirWorldRenderManager::DrawBg(WilSpriteKey key)
{
    auto tile = this->_mapData->TileAt(key.x, key.y);
    auto fileIdx = tile.FileIndex;
    int imgIdx = tile.TileIndex;
    if (tile.RemapFileIndex(imgIdx))
        return;
    auto spriteHandle = _spriteMgr->LoadSprite({ fileIdx, (uint32_t)imgIdx });
    if (!spriteHandle)
        return;
    auto spRender = this->GetSpriteRenderer(_bgUse, key);
    spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellMin(key.x, key.y));
    spRender->Sprite = spriteHandle->GetSprite();
    spRender->Sprite.lock()->Pivot = { 0, 0 };
    spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->holder.push_back(spriteHandle);
}

void MirWorldRenderManager::DrawMid(WilSpriteKey key,int i, SpriteRenderLayer use)
{
    auto cell = this->_mapData->CellAt(key.x, key.y);
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
