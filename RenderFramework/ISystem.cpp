#include "ISystem.h"
#include "ISceneNodeComponent.h"

vector<ISceneNodeComponent*> ISystem::GetComponentsByType(int typeId)
{ 
	return ISceneNodeComponent::GetComponentsByType(typeId); 
}