#include "Camera.h"

void Camera::FillTypeIds(std::vector<uint32_t>& typeIds)
{
	ISceneNodeComponent::FillTypeIds(typeIds);
	typeIds.push_back(Camera::TypeId);
}
