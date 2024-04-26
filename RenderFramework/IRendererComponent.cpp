#include "IRendererComponent.h"

void IRendererComponent::FillTypeIds(std::vector<uint32_t>& typeIds)
{
	ISceneNodeComponent::FillTypeIds(typeIds);
	typeIds.push_back(IRendererComponent::TypeId);
}
