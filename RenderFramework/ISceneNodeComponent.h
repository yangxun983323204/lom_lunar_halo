#pragma once
#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include <list>
#include "ISystem.h"

using std::weak_ptr;
using std::vector;
using std::list;

class SceneNode;


#define SCENE_NODE_COMPONENT(T)  static const uint32_t TypeId = T; virtual uint32_t GetTypeId() const {return T;}

class ISceneNodeComponent
{
public:
	SCENE_NODE_COMPONENT('isnc');
	bool Enable;
	inline SceneNode* GetSceneNode() { return _node.expired() ? nullptr : _node.lock().get(); }
	template<typename TComponent>
	TComponent* As();
protected:
	friend class SceneNode;
	friend class ISystem;

	static list<ISceneNodeComponent*> GetComponentsByType(int typeId);

	weak_ptr<SceneNode> _node;

private:
	void Reg();
	void UnReg();

	static std::map<int, list<ISceneNodeComponent*>> __componentsReg;
};

template<typename TComponent>
TComponent* ISceneNodeComponent::As()
{
	return dynamic_cast<TComponent*>(this);
}