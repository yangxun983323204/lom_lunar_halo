#pragma once
#include "ISystem.h"

class AnimatorSystem : public ISystem
{
public:
	void Update(uint64_t totalMs, uint32_t deltaMs);
};

