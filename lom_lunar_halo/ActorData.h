#pragma once
#include <string>
#include "../mir_lib/MirConst.h"

using std::string;

class ActorData
{
public:
	string Name;
	Mir::Direction Dir;
	uint8_t Gender;
	uint8_t Weapon;
	uint8_t Dress;
	uint8_t Hair;
	uint8_t ActorImgIdx;
};

