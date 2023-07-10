#include "pch.h"
#include "MirConst.h"

#define _IMAGE_M_HUMAN				71
#define _IMAGE_WM_HUMAN				77
#define _IMAGE_MONSTER1				90
#define _IMAGE_NPC					130

int Mir::GetActorImgFileIdx(ActorType actorType, uint8_t dress)
{
	switch (actorType)
	{
	case Mir::Man:
		return _IMAGE_M_HUMAN;
	case Mir::Woman:
		return _IMAGE_WM_HUMAN;
	case Mir::Monster:
		return _IMAGE_MONSTER1 + dress / 10;
	case Mir::Npc:
		return _IMAGE_NPC;
	}

    return -1;
}
const int HeroFrameSpan = 3000;
const int MonsterFrameSpan = 1000;

int Mir::GetActorFirstImgIdx(const AnimData& data, ActorType actorType, uint8_t dress, Direction& dir)
{
	switch (actorType)
	{
	case Mir::Man:
	case Mir::Woman:
		return data.FirstFrame + dress * HeroFrameGroupMax + (int)dir * 10;
	case Mir::Monster:
	{
		switch (dress)
		{
		case 31:	// 食人花	식인초.
		case 67:	// 触龙神	촉룡신.
		case 73:	// 角蝇		비막원충.
		case 104:	// 赤月恶魔	적월마.
			dir = Mir::Direction::Bottom;
			break;
		case 106:	// 蜘蛛xx	폭안거미.
			dir = Mir::Direction::BottomRight;
			break;
		default:
			break;
		}

		return data.FirstFrame + dress % 10 * MonsterFrameSpan + (int)dir * 10 - 1;
	}
	case Mir::Npc:
		return _IMAGE_NPC;
	}

	return -1;
}
