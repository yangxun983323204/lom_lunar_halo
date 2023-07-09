#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include "../RenderFramework/SceneNode.h"

using std::unordered_map;
using std::vector;
using std::weak_ptr;
using std::string;

class ActorView :public ISceneNodeComponent
{
public:
	SCENE_NODE_COMPONENT('atv');

	void PlayAnim(string name);
	vector<string> GetSubPartNames();
	void SetSubPartsOrder(vector<string> names);
	shared_ptr<SceneNode> GetSubPart(string name);
	void Clear();

	int Id;
	int Type;
protected:
};

