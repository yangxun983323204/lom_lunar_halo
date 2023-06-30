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
}