#include "ISystem.h"
#include "ISceneNodeComponent.h"

list<ISceneNodeComponent*> ISystem::GetComponentsByType(int typeId)
{ 
	return ISceneNodeComponent::GetComponentsByType(typeId); 
}