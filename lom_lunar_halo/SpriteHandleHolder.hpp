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
	_SCENE_NODE_COMPONENT_ID(SpriteHandleHolder);
	SpriteHandleHolder() :holder{} {}
	inline void Clear() { holder.clear(); }
	inline void Add(shared_ptr<SpriteResHandle> handle) { holder.push_back(handle); }

private:
	vector<shared_ptr<SpriteResHandle>> holder;
};