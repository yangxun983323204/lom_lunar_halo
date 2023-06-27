#pragma once
#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include "ISystem.h"

using std::weak_ptr;
using std::vector;

class SceneNode;


#define SCENE_NODE_COMPONENT(T)  static const uint32_t TypeId = T; virtual uint32_t GetTypeId() const {return T;}

class ISceneNodeComponent
{
public:
	SCENE_NODE_COMPONENT('isnc');
	bool Enable;
	inline SceneNode* GetSceneNode() { return _node.expired() ? nullptr : _node.lock().get(); }
protected:
	friend class SceneNode;
	friend class ISystem;

	static vector<ISceneNodeComponent*> GetComponentsByType(int typeId);

	weak_ptr<SceneNode> _node;

private:
	void Reg();
	void UnReg();

	static std::map<int,vector<ISceneNodeComponent*>> __componentsReg;
};