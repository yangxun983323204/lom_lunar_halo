#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include "SceneNode.h"
#include "ISceneNodeComponent.h"
#include "Animation.h"

using std::unordered_map;
using std::string;

class Animator : public ISceneNodeComponent
{
public:
	_SCENE_NODE_COMPONENT_ID(Animator);

	Animator();

	void Clear();
	void Add(string name, Animation& anim);
	void Remove(string name);
	void Remove(int hash);
	Animation* Get(string name);
	Animation* Get(int hash);
	Animation* GetCurrent();
	int SetCurrent(string name);

private:
	unordered_map<int, Animation> _anims;
	int _currIdx;
	std::hash<string> _hashHelper;
};

