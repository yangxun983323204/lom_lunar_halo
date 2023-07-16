#include "ISceneNodeComponent.h"
#include <algorithm>

std::map<int, list<ISceneNodeComponent*>> ISceneNodeComponent::__componentsReg{};

void ISceneNodeComponent::Reg()
{
	__componentsReg[GetTypeId()].push_back(this);
}

void ISceneNodeComponent::UnReg()
{
	UnRegWithId(GetTypeId());
}

void ISceneNodeComponent::RegWithId(uint32_t id) 
{
	__componentsReg[id].push_back(this);
}

void ISceneNodeComponent::UnRegWithId(uint32_t id)
{
	auto v = __componentsReg[id];
	auto f = std::find_if(v.begin(), v.end(), [this](ISceneNodeComponent* n) { return n == this; });
	if (f != v.end())
		v.erase(f);
}

list<ISceneNodeComponent*> ISceneNodeComponent::GetComponentsByType(int typeId)
{
	return __componentsReg[typeId];
}
