#include "Camera.h"

void Camera::FillTypeIds(std::set<uint32_t>& set)
{
	ISceneNodeComponent::FillTypeIds(set);
	set.insert(Camera::TypeId);
}
