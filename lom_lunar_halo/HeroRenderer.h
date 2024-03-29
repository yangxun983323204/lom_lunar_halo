#pragma once
#include <DirectXMath.h>
#include "../mir_lib/MirConst.h"
#include "../RenderFramework/ISceneNodeComponent.h"
#include "../RenderFramework/Sprite.hpp"
#include "../RenderFramework/IRenderer.h"
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

_SCENE_NODE_COMPONENT_BEGIN2(HeroRenderer, IRenderer, IUpdate)
public:
	void Init(HeroData& hero, AnimDatabase& animDB, WilSpriteManager* resMgr);
	virtual void OnRender(IGraphic2D* gfx) override;

private:
	Animation _anims[(int)Mir::HeroSubPart::MAX][(int)Mir::Direction::MAX];
_SCENE_NODE_COMPONENT_END
