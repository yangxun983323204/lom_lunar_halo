#pragma once
#include <vector>
#include <list>

using std::vector;
using std::list;

class ISceneNodeComponent;

class ISystem
{
protected:
	static list<ISceneNodeComponent*> GetComponentsByType(int typeId);
};