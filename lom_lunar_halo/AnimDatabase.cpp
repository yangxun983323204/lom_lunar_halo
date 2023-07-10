#include "pch.h"
#include "AnimDatabase.h"
#include "tinyxml2.h"

AnimDatabase::AnimDatabase():
	_animGroups{}
{
}

const unordered_map<uint16_t, Mir::AnimData>& AnimDatabase::GetDataGroup(uint16_t groupId)
{
	return _animGroups[groupId];
}

const Mir::AnimData& AnimDatabase::GetData(uint16_t groupId, uint16_t id)
{
	return _animGroups[groupId][id];
}

void AnimDatabase::LoadFromXml(string path)
{
	tinyxml2::XMLDocument xml;
	if (xml.LoadFile(path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
		return;

	auto root = xml.FirstChildElement();
	auto groupNode = root->FirstChildElement();
	while (groupNode)
	{
		uint16_t groupId = groupNode->UnsignedAttribute("id");
		AnimDataGroup group{};

		auto animNode = groupNode->FirstChildElement();
		while (animNode)
		{
			uint16_t id = animNode->UnsignedAttribute("id");
			uint16_t start = animNode->UnsignedAttribute("start");
			uint16_t count = animNode->UnsignedAttribute("count");
			uint16_t span = animNode->UnsignedAttribute("span");
			group[id] = { start,count,span };
			animNode = animNode->NextSiblingElement();
		}

		_animGroups[groupId] = group;
		groupNode = groupNode->NextSiblingElement();
	}
}
