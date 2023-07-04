#pragma once
#include <vector>
#include <memory>
#include "SceneNode.h"
#include "ISceneNodeComponent.h"
#include "Animation.h"

class Animator : public ISceneNodeComponent
{
public:
	SCENE_NODE_COMPONENT('anmr');
	Animator():
		Current{}, Anims{}
	{}

	inline void Clear()
	{
		Current = nullptr;
		Anims.clear();
	}

	Animation* Current;
	std::vector<Animation> Anims;
};

