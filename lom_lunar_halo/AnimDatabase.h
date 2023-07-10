#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "../mir_lib/MirConst.h"

using std::vector;
using std::unordered_map;
using std::string;

typedef unordered_map<uint16_t, Mir::AnimData> AnimDataGroup;

class AnimDatabase
{
public:
	AnimDatabase();
	const unordered_map<uint16_t, Mir::AnimData>& GetDataGroup(uint16_t groupId);
	const Mir::AnimData& GetData(uint16_t groupId, uint16_t id);
	void LoadFromXml(string path);

private:
	unordered_map<uint16_t, AnimDataGroup> _animGroups;
};

