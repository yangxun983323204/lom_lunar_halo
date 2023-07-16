#pragma once
#include "ISceneNodeComponent.h"

class IUpdate :public ISceneNodeComponent
{
public:
	_SCENE_NODE_COMPONENT_ID(IUpdate);

	virtual void Update(uint32_t totalMs, uint32_t deltaMs)=0;
};