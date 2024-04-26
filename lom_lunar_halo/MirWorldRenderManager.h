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

typedef DirectX::XMUINT2 CellCoord;

class MirWorldRenderManager
{
public:
	enum class Layer
	{
		Bg_0,
		Bg_1,
		Object,
		Actor,
		Overlay,
		Fx_0,
		Fx_1,
		Fx_2,
		Debug_0,
		Debug_1,
		Debug_2,

		Max
	};

	class SpriteRenderLayer
	{
	public:
		SpriteRenderLayer(int layer, int depth) :
			Layer{ layer }, Depth{depth}
		{}

		int Layer;
		int Depth;
	};

	MirWorldRenderManager(DX::DeviceResources* dr, 
		shared_ptr<WilSpriteManager> mapResMgr,
		shared_ptr<WilSpriteManager> actorResMgr,
		shared_ptr<WilSpriteManager> itemResMgr);

	void SetMapData(shared_ptr<MapData> mapData);
	DirectX::XMINT2 GetViewPoint();
	CellView* GetViewPointCell();
	void SetViewPoint(DirectX::XMINT2 coor);
	void Update(DX::StepTimer const& timer);
	void Render();
	void Clear();

	void AddHero(HeroData& data);
	void SetSelfHero(HeroData& data);
	void SetSelfHeroDirection(Mir::Direction dir);
	void SetSelfHeroMotion(Mir::PlayerMotion motion);

	/// <summary>
	/// 获取地图格加载情况
	/// </summary>
	inline std::string GetGridDebugInfo() { return _gridView->GetDebugInfo(); }

	bool DebugLayerDrawEnable[(int)Layer::Max]{ true };
private:
	friend class MirCellView;

	ActorView* GetActorView(int id, function<shared_ptr<SceneNode>()> createFunc);

	void ClearScreen(XMFLOAT4 color);
	void Draw(ID3D11ShaderResourceView* srv, DirectX::SimpleMath::Rectangle viewRect, XMFLOAT4 color);

	SpriteRenderLayer _bgLayer;
	SpriteRenderLayer _mid1Layer;
	SpriteRenderLayer _mid2Layer;
	SpriteRenderLayer _topLayer;
	SpriteRenderLayer _debugLayer;

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

