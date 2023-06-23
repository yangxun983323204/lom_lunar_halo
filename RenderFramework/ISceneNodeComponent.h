#pragma once
#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include "ISystem.h"

using std::weak_ptr;
using std::vector;

class SceneNode;

class ISceneNodeComponent
{
public:
	bool Enable;
	virtual const int GetTypeId();
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