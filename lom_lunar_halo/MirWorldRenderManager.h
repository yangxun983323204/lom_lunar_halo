#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "../RenderFramework/HashSupport.h"
#include <unordered_map>
#include "DeviceResources.h"
#include "SceneManager.h"
#include "WilSpriteManager.h"
#include "../mir_lib/MapData.h"
#include "StepTimer.h"
#include "ActorView.h"
#include "AnimDatabase.h"

using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::unordered_map;
using std::function;

typedef DirectX::XMUINT2 WilSpriteKey;

class MirWorldRenderManager
{
public:
	class SpriteRenderLayer
	{
	public:
		SpriteRenderLayer(int layer, int depth) :
			Layer{ layer }, Depth{depth}, Record{}
		{}

		int Layer;
		int Depth;
		unordered_map<WilSpriteKey, SpriteRenderer*> Record;
	};

	MirWorldRenderManager(DX::DeviceResources* dr, 
		shared_ptr<WilSpriteManager> mapResMgr,
		shared_ptr<WilSpriteManager> actorResMgr,
		shared_ptr<WilSpriteManager> itemResMgr);

	void SetMapData(shared_ptr<MapData> mapData);
	DirectX::XMINT2 GetViewPoint();
	void SetViewPoint(DirectX::XMINT2 coor);
	void Update(DX::StepTimer const& timer);
	void Render();
	void RenderDebug();
	void Clear();

	void AddHero(HeroData& data);
	void SetSelfHero(HeroData& data);
	void SetSelfHeroDirection(Mir::Direction dir);
	void SetSelfHeroMotion(Mir::PlayerMotion motion);
private:
	SpriteRenderer* GetMapStaticSpriteRenderer(SpriteRenderLayer& use, WilSpriteKey key);
	SpriteRenderer* GetMapAnimSpriteRenderer(SpriteRenderLayer& use, WilSpriteKey key);
	ActorView* GetActorView(int id, function<shared_ptr<SceneNode>()> createFunc);

	void ReleaseMapStaticSpriteRenderer(SpriteRenderLayer& use, WilSpriteKey key);
	void ReleaseMapAnimSpriteRenderer(SpriteRenderLayer& use, WilSpriteKey key);
	void ReleaseActor(int id);

	void SetUpBg(WilSpriteKey key);
	void SetUpMid(WilSpriteKey key, int i, SpriteRenderLayer& use);

	void ClearScreen(XMFLOAT4 color);
	void Draw(ID3D11ShaderResourceView* srv, DirectX::SimpleMath::Rectangle viewRect, XMFLOAT4 color);

	SpriteRenderLayer _bgUse;
	SpriteRenderLayer _mid1Use;
	SpriteRenderLayer _mid2Use;
	SpriteRenderLayer _topUse;

	int _selfHeroId;
	unordered_map<int, HeroData> _heros;
	unordered_map<int, ActorView*> _actorViews;

	DX::DeviceResources* _dr;
	shared_ptr<MapData> _mapData;
	unique_ptr<SceneManager> _sceneMgr;
	unique_ptr<AnimatorSystem> _animatorSystem;
	unique_ptr<SpriteRenderSystem> _renderSystem;

	shared_ptr<WilSpriteManager> _mapResMgr;
	shared_ptr<WilSpriteManager> _actorResMgr;
	shared_ptr<WilSpriteManager> _itemResMgr;

	shared_ptr<SceneNode> _camera;
	shared_ptr<GridViewComponent> _gridView;

	unique_ptr<AnimDatabase> _animDB;
};

