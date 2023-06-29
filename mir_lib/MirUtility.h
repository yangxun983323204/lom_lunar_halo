#pragma once
#include "MirConst.h"
#include <DirectXMath.h>

using DirectX::XMINT2;

namespace Mir 
{
	inline XMINT2 GetCellCenter(int32_t cellX, int32_t cellY)
	{
		return { cellX * CellW + CellWHalf, cellY * CellH + CellHHalf };
	}

	inline XMINT2 GetCellMin(int32_t cellX, int32_t cellY)
	{
		return { cellX * CellW, cellY * CellH };
	}

	inline XMINT2 GetCellMax(int32_t cellX, int32_t cellY)
	{
		return { cellX * CellW + CellW, cellY * CellH + CellH };
	}
}