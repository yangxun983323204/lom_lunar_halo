#pragma once
#include <stdint.h>

namespace Mir
{
	const uint16_t CellW = 48;
	const uint16_t CellH = 32;
	const uint16_t CellWHalf = CellW / 2;
	const uint16_t CellHHalf = CellH / 2;
	const uint8_t CellPerTileX = 2;
	const uint8_t CellPerTileY = 2;
	const uint16_t TileW = CellW * CellPerTileX;
	const uint16_t TileH = CellH * CellPerTileY;
	const uint16_t CellCountX = 17;// 为了让角色居中，因此让屏幕水平和竖直方向都显示奇数个格子
	const uint16_t CellCountY = 17;
	const uint16_t LoginLayoutW = 640;
	const uint16_t LoginLayoutH = 480;
	const uint16_t GameLayoutW = 800;// 必须为800*600，不然UI对应不上位置
	const uint16_t GameLayoutH = 600;
	const uint16_t GameLayoutWHalf = 400;// 必须为800*600，不然UI对应不上位置
	const uint16_t GameLayoutHHalf = 300;

	// Tile有8种动画帧间间隔
	const uint16_t TileAnimSpanMs[]{
		150,200,250,300,350,400,420,450
	};

	// 值关系到8方向的精灵索引偏移
	enum class Direction {
		Bottom = 0,
		BottomRight,
		Right,
		TopRight,
		Top,
		TopLeft,
		Left,
		BottomLeft,
		
		MAX,
	};

	enum class PlayerMotion {
		Stand = 0,
		ArrowAttack,
		Spell_1,
		Spell_2,
		Hold,
		PushBack,
		PushBackFly,
		Attack,
		Cut,
		OneVerticalSwing,
		TwoVerticalSwing,
		OneHorizontalSwing,
		TowHorizontalSwing,
		SpearVerticalSwing,
		SpearHorizontalSwing,
		Hitted,
		WheelWind,
		RandSwing,
		BackDropKick,
		Die,
		SpecialDie,
		Walk,
		Run,
		MoodEpo,
		Roll,
		FishStand,
		FishHand,
		FishThrow,
		FishPull,
		HorseStand,
		HorseWalk,
		HorseRun,
		HorseHit,

		MAX,
	};

	enum class MonsterMotion {
		Stand = 0,
		Walk,
		Attack_A,
		Hitted,
		Die,
		Attack_B,
		Spell_A,
		Speel_B,
		Appear,
		SpecialMotion,

		MAX,
	};

	enum class NpcMotion {
		Stand = 0,
		Act01,
		Act02,

		MAX,
	};
}