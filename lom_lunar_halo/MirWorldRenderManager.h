#pragma once
#include <memory>
#include <vector>
#include "../RenderFramework/HashSupport.h"
#include <unordered_map>
#include "DeviceResources.h"
#include "SceneManager.h"
#include "WilSpriteManager.h"
#include "../mir_lib/MapData.h"

using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::unordered_map;

typedef DirectX::XMUINT2 WilSpriteKey;

class MirWorldRenderManager
{
public:
	class SpriteRenderLayer
	{
	public:
		SpriteRenderLayer(int layer) :
			Layer{ layer }, Record{}
		{}

		int Layer;
		unordered_map<WilSpriteKey, SpriteRenderer*> Record;
	};

	MirWorldRenderManager(DX::DeviceResources* dr, shared_ptr<WilSpriteManager> spriteMgr);
	void SetMapData(shared_ptr<MapData> mapData);
	SpriteRenderer* GetSpriteRenderer(SpriteRenderLayer& use, WilSpriteKey key);
	void ReleaseSpriteRenderer(SpriteRenderLayer& use, WilSpriteKey key);
	inline void Render() { _sceneMgr->Render(); }

private:
	void DrawBg(WilSpriteKey key);
	void DrawMid(WilSpriteKey key, int i, SpriteRenderLayer use);

	SpriteRenderLayer _bgUse;
	SpriteRenderLayer _mid1Use;
	SpriteRenderLayer _mid2Use;
	SpriteRenderLayer _topUse;
	vector<SpriteRenderer*> _pool;

	DX::DeviceResources* _dr;
	shared_ptr<MapData> _mapData;
	unique_ptr<SceneManager> _sceneMgr;
	shared_ptr<WilSpriteManager> _spriteMgr;
	shared_ptr<GridViewComponent> _gridView;
};

