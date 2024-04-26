#include "ISceneNodeComponent.h"
#include <algorithm>

std::map<int, list<ISceneNodeComponent*>> ISceneNodeComponent::__componentsReg{};

void ISceneNodeComponent::CallReg()
{
	std::vector<uint32_t> types{};
	FillTypeIds(types);
	for (auto t:types)
	{
		RegWithId(t);
	}
}

void ISceneNodeComponent::CallUnReg()
{
	std::vector<uint32_t> types{};
	FillTypeIds(types);
	for (auto t : types)
	{
		UnRegWithId(t);
	}
}

void ISceneNodeComponent::FillTypeIds(std::vector<uint32_t>& typeIds)
{
	typeIds.push_back(ISceneNodeComponent::TypeId);
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

void ISceneNodeComponent::Tick(uint64_t totalMs, uint32_t deltaMs)
{
}

list<ISceneNodeComponent*> ISceneNodeComponent::GetComponentsByType(int typeId)
{
	return __componentsReg[typeId];
}
