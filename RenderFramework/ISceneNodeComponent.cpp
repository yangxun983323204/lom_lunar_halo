#include "ISceneNodeComponent.h"
#include <algorithm>

std::map<int, vector<ISceneNodeComponent*>> ISceneNodeComponent::__componentsReg{};

void ISceneNodeComponent::Reg()
{
	__componentsReg[GetTypeId()].push_back(this);
}

void ISceneNodeComponent::UnReg()
{
	auto v = __componentsReg[GetTypeId()];
	auto f = std::find_if(v.begin(), v.end(), [this](ISceneNodeComponent* n) { return n == this; });
	if (f != v.end())
		v.erase(f);
}

vector<ISceneNodeComponent*> ISceneNodeComponent::GetComponentsByType(int typeId)
{
	return __componentsReg[typeId];
}
