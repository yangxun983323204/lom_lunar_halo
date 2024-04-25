#include "pch.h"
#include "MirCellView.h"
#include "MirWorldRenderManager.h"
#include "SpriteHandleHolder.hpp"
#include "../mir_lib/MirConst.h"
#include "../mir_lib/MirUtility.h"

MirCellView::MirCellView(int rowIdx, int colIdx, MirWorldRenderManager* mgr):
	CellView(rowIdx, colIdx),
    _actors{}, _mirWorldMgr{ mgr }, _barrierTex{}
{
}

MirCellView::~MirCellView()
{
	CellView::~CellView();
}

void MirCellView::OnUnload()
{
    for (auto actor : _actors)
    {
        _mirWorldMgr->_sceneMgr->ReleaseByInnerTag(actor);
    }
    _actors.clear();

    _mirWorldMgr->_sceneMgr->ReleaseByInnerTag(_barrierTex);
    _mirWorldMgr->_sceneMgr->ReleaseByInnerTag(_frameLeftTex);
    _mirWorldMgr->_sceneMgr->ReleaseByInnerTag(_frameBottomTex);
}

void MirCellView::OnLoad()
{
    SetUpBg();
    SetUpMid();
    SetUpDebugBarrier();
    SetUpDebugFrame();
}

void MirCellView::OnHide()
{
    for (auto actor : _actors)
    {
        if(!actor.expired())
            actor.lock()->SelfActive = false;
    }

    _barrierTex.lock()->SelfActive = false;
    _frameLeftTex.lock()->SelfActive = false;
    _frameBottomTex.lock()->SelfActive = false;
}

void MirCellView::OnShow()
{
    for (auto actor : _actors)
    {
        if (!actor.expired())
            actor.lock()->SelfActive = true;
    }

    _barrierTex.lock()->SelfActive = _mirWorldMgr->DebugBarrier && !_mirWorldMgr->_mapData->CellAt(_colIdx, _mirWorldMgr->_mapData->h() - _rowIdx).Walkable();;
    _frameLeftTex.lock()->SelfActive = _mirWorldMgr->DebugGrid;
    _frameBottomTex.lock()->SelfActive = _mirWorldMgr->DebugGrid;
}

void MirCellView::SetUpBg()
{
	if (_rowIdx % 2 != 0 || _colIdx % 2 != 0) // 背景是一个图块覆盖2*2的cell，因此只有左下角的图块需要画背景
		return;

	auto tile = _mirWorldMgr->_mapData->TileAt(_colIdx, _mirWorldMgr->_mapData->h() - _rowIdx);
	int fileIdx = tile.FileIndex;
	int imgIdx = tile.TileIndex;
	if (!tile.RemapFileIndex(fileIdx))
		return;

	auto spriteHandle = _mirWorldMgr->_mapResMgr->LoadSprite({ (uint32_t)fileIdx, (uint32_t)imgIdx });
	if (!spriteHandle)
		return;

	auto spRender = _mirWorldMgr->_sceneMgr->SpawnStaticSprite()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
	spRender->SortLayer = _mirWorldMgr->_bgLayer.Layer;
	spRender->Depth = _mirWorldMgr->_bgLayer.Depth;
	spRender->Enable = true;
	spRender->Debug = false;

	spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellMin(_colIdx, _rowIdx));
	spRender->Sprite = spriteHandle->GetSprite();
	spRender->Sprite.lock()->Pivot = { 0, 0 };
	spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();
	spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Add(spriteHandle);

    spRender->GetSceneNode()->SelfActive = false;
	_actors.push_back(spRender->GetSceneNodeWeakPtr());
}

void MirCellView::SetUpMid()
{
	for (uint8_t i = 1; i <= 2; i++)
	{
		auto cell = _mirWorldMgr->_mapData->CellAt(_colIdx, _mirWorldMgr->_mapData->h() - _rowIdx);
		if (!cell.FileEnableOf(i))
			continue;

        auto spRender = _mirWorldMgr->_sceneMgr->SpawnAnimSprite()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
        spRender->GetSceneNode()->GetComponent<Animator>().lock()->As<Animator>()->Enable = true;
        spRender->Enable = true;
        spRender->Debug = false;
        spRender->DebugColor = { 1,0,0,1 };

        spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellMin(_colIdx, _rowIdx));
        spRender->GetSceneNode()->GetComponent<Animator>().lock()->Enable = false;
        spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();


        int fileIdx = cell.FileIndexOf(i);
        if (!cell.RemapFileIndex(fileIdx))
            continue;

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
                auto spriteHandle = _mirWorldMgr->_mapResMgr->LoadSprite({ (uint32_t)fileIdx, (uint32_t)idx });
                if (!spriteHandle) {
                    anim.AddFrame({});
                    continue;
                }
                spriteHandle->GetSprite()->Pivot = { 0, 0 };
                anim.AddFrame(spriteHandle->GetSprite());
                spHolder->Add(spriteHandle);
            }
            animator->Add("0", anim);
            animator->SetCurrent("0");
            continue;
        }
        // 地表物体图片的原点是左下角,对齐的也是cell的左下角
        auto spriteHandle = _mirWorldMgr->_mapResMgr->LoadSprite({ (uint32_t)fileIdx, (uint32_t)imgIdx });
        if (!spriteHandle)
            continue;

        auto sp = spriteHandle->GetSprite();
        spRender->Sprite = sp;
        spRender->Sprite.lock()->Pivot = { 0, 0 };

        if (sp->Rect.width == 48 && sp->Rect.height == 32) {
            spRender->SortLayer = _mirWorldMgr->_mid1Layer.Layer;// 对于地图上的装饰，如果它巧好是一个cell大小时，它相当于第二层背景。
            spRender->Depth = _mirWorldMgr->_mid1Layer.Depth;
        }
        else {
            spRender->SortLayer = _mirWorldMgr->_topLayer.Layer;// 否则它是覆盖层
            spRender->Depth = _mirWorldMgr->_topLayer.Depth;
        }
        spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Add(spriteHandle);

        spRender->GetSceneNode()->SelfActive = false;
        _actors.push_back(spRender->GetSceneNodeWeakPtr());
	}
}

void MirCellView::SetUpDebugBarrier()
{
    auto spRender = _mirWorldMgr->_sceneMgr->SpawnStaticSprite()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
    spRender->SortLayer = _mirWorldMgr->_debugLayer.Layer;
    spRender->Depth = _mirWorldMgr->_debugLayer.Depth;
    spRender->Enable = true;
    spRender->Debug = false;
    spRender->Color = { 1,0,0,0.2 };

    spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellCenter(_colIdx, _rowIdx));
    spRender->OverrideWidth = 48;
    spRender->OverrideHeight = 32;
    spRender->Sprite = _mirWorldMgr->_renderSystem->_debugImgWhite;
    spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();

    spRender->GetSceneNode()->SelfActive = false;
    _barrierTex = spRender->GetSceneNodeWeakPtr();
}

void MirCellView::SetUpDebugFrame()
{
    // 左边框
    auto spRender = _mirWorldMgr->_sceneMgr->SpawnStaticSprite()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
    spRender->SortLayer = _mirWorldMgr->_debugLayer.Layer;
    spRender->Depth = _mirWorldMgr->_debugLayer.Depth;
    spRender->Enable = true;
    spRender->Debug = false;
    spRender->Color = { 1,0,0,0.2 };

    spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellMin(_colIdx, _rowIdx));
    spRender->OverrideWidth = 1;
    spRender->OverrideHeight = 32;
    spRender->Sprite = _mirWorldMgr->_renderSystem->_debugImgWhite;
    spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();

    spRender->GetSceneNode()->SelfActive = false;
    _frameLeftTex = spRender->GetSceneNodeWeakPtr();

    // 下边框
    spRender = _mirWorldMgr->_sceneMgr->SpawnStaticSprite()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
    spRender->SortLayer = _mirWorldMgr->_debugLayer.Layer;
    spRender->Depth = _mirWorldMgr->_debugLayer.Depth;
    spRender->Enable = true;
    spRender->Debug = false;
    spRender->Color = { 1,0,0,0.2 };

    spRender->GetSceneNode()->SetLocalPosition(Mir::GetCellMin(_colIdx, _rowIdx));
    spRender->OverrideWidth = 48;
    spRender->OverrideHeight = 1;
    spRender->Sprite = _mirWorldMgr->_renderSystem->_debugImgWhite;
    spRender->GetSceneNode()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();

    spRender->GetSceneNode()->SelfActive = false;
    _frameBottomTex = spRender->GetSceneNodeWeakPtr();
}
