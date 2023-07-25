#pragma once
#include "ISceneNodeComponent.h"

class IUpdate
{
public:
	virtual void Update(uint32_t totalMs, uint32_t deltaMs)=0;
};