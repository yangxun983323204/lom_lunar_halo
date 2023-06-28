#pragma once
#include <vector>
#include <memory>
#include "../RenderFramework/ISceneNodeComponent.h"
#include "WilSpriteManager.h"

using std::vector;
using std::shared_ptr;

class SpriteHandleHolder : public ISceneNodeComponent
{
public:
	SCENE_NODE_COMPONENT('shh');
	SpriteHandleHolder() :holder{} {}
	vector<shared_ptr<SpriteResHandle>> holder;
};