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
	const uint16_t TileAnimSpanMs[]
	{
		150,200,250,300,350,400,420,450
	};

	enum class ShadowType 
	{
		None = 0,
		Proj = 49,
		Orth = 50,
	};

	// 值关系到8方向的精灵索引偏移
	enum class Direction 
	{
		Top = 0,
		TopRight,
		Right,
		BottomRight,
		Bottom,
		BottomLeft,
		Left,
		TopLeft,
		
		MAX,
	};

	enum ActorType 
	{
		Man = 0,
		Woman = 1,
		Monster = 3,
		Npc = 5,
	};

	class AnimData 
	{
	public:
		uint16_t FirstFrame;
		uint16_t FrameCount;
		uint16_t AnimSpanMs;
	};

	const int HorseFrameBegin = 2320;
	const int HorseFrameGroupMax = 400;

	const int HeroFrameGroupMax = 3000;

	const int WeaponFrameGroupMax = 3000;

	enum class PlayerMotion 
	{
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

	const int HeroFrameSpan = 3000;
	const int MonsterFrameSpan = 1000;

	int GetActorImgFileIdx(ActorType actorType, uint8_t dress);
	int GetHairImgFileIdx(ActorType actorType, uint8_t hair);
	int GetWeaponImgFileIdx(ActorType actorType, uint8_t weapon);

	#define _IMAGE_HORSE				33
	inline int GetHorseImgFileIdx(uint8_t horse) { return horse == 0 ? -1 : _IMAGE_HORSE; }
	inline int GetEffectImgFileIdx() { return -1; }// todo
	inline int GetShieldImgFileIdx() { return -1; }// todo

	int GetActorFirstImgIdx(const AnimData& data, ActorType actorType, uint8_t dress, Direction& dir);
	inline int GetHorseFrame(uint8_t horse, uint8_t dress)
	{
		//m_dwCurrHorseFrame = (m_stFeatureEx.bHorse * _MAX_HORSE_FRAME - _MAX_HORSE_FRAME) + (m_dwCurrFrame - (_MAX_HERO_FRAME * m_stFeature.bDress) - _START_HORSE_FRAME);
		return (horse - 1) * HorseFrameGroupMax - HeroFrameGroupMax * dress - HorseFrameBegin;
	}

	inline int GetHairFrame(uint8_t hair, uint8_t dress)
	{
		//m_dwCurrHairFrame = (m_stFeature.bHair*_MAX_HERO_FRAME-_MAX_HERO_FRAME)+(m_dwCurrFrame-(_MAX_HERO_FRAME*m_stFeature.bDress));
		return (hair - 1) * HeroFrameSpan - HeroFrameSpan * dress;
	}

	inline int GetWeaponFrame(uint8_t weapon, uint8_t dress)
	{
		//m_dwCurrWeaponFrame = (((m_stFeature.bWeapon-1)%10)*_MAX_WEAPON_FRAME)+(m_dwCurrFrame-(_MAX_HERO_FRAME*m_stFeature.bDress));
		return (weapon - 1) % 10 * WeaponFrameGroupMax - HeroFrameGroupMax * dress;
	}

	inline int GetEffectFrame(uint8_t effect, uint8_t dress)
	{
		return -1;// todo
	}

	inline int GetShieldFrame(uint8_t shield, uint8_t dress)
	{
		return -1;// todo
	}
}