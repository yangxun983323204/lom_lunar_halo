#pragma once
#include <DirectXMath.h>
#include "../mir_lib/MirConst.h"
#include "../RenderFramework/ISceneNodeComponent.h"
#include "../RenderFramework/Sprite.hpp"
#include "../RenderFramework/IRendererComponent.h"
#include "../RenderFramework/IUpdate.h"
#include "../RenderFramework/Animation.h"
#include "ActorData.h"
#include "AnimDatabase.h"
#include "WilSpriteManager.h"

namespace Mir {

	enum class HeroSubPart
	{
		Weapon = 0,
		Horse,
		Actor,
		Hair,
		Effect,
		Shield,

		MAX,
	};
}

class HeroRenderer : public IRendererComponent
{
public:
	_SCENE_NODE_COMPONENT_ID(HeroRenderer)
	void Init(HeroData& hero, AnimDatabase& animDB, WilSpriteManager* resMgr);
	virtual void OnRender(IGraphic2D* gfx) override;
	virtual void FillTypeIds(std::vector<uint32_t>& typeIds) override;
private:
	Animation _anims[(int)Mir::HeroSubPart::MAX][(int)Mir::Direction::MAX];
};
