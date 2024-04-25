#include "ISceneNodeComponent.h"
#include <algorithm>

std::map<int, list<ISceneNodeComponent*>> ISceneNodeComponent::__componentsReg{};

void ISceneNodeComponent::CallReg()
{
	std::set<uint32_t> types{};
	FillTypeIds(types);
	for (auto t:types)
	{
		RegWithId(t);
	}
}

void ISceneNodeComponent::CallUnReg()
{
	std::set<uint32_t> types{};
	FillTypeIds(types);
	for (auto t : types)
	{
		UnRegWithId(t);
	}
}

void ISceneNodeComponent::FillTypeIds(std::set<uint32_t>& set)
{
	set.insert(ISceneNodeComponent::TypeId);
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
