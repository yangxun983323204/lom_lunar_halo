#pragma once
#include <string>
#include "../mir_lib/MirConst.h"

using std::string;

class ActorData 
{
public:
	uint64_t NetId;
	string Name;
	Mir::Direction Dir;

	uint8_t Dress;
	// cell
	DirectX::XMINT2 Pos;
};

class HeroData:public ActorData
{
public:
	Mir::ActorType Gender;
	uint8_t Hair;
	uint8_t Weapon;
	uint8_t Horse;
	uint8_t Effect;
	uint8_t Shield;
	Mir::PlayerMotion Motion;
};

