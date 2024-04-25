#include "IRendererComponent.h"

void IRendererComponent::FillTypeIds(std::set<uint32_t>& set)
{
	ISceneNodeComponent::FillTypeIds(set);
	set.insert(IRendererComponent::TypeId);
}
