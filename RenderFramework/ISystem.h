#pragma once
#include <vector>

using std::vector;

class ISceneNodeComponent;

class ISystem
{
protected:
	static vector<ISceneNodeComponent*> GetComponentsByType(int typeId);
};