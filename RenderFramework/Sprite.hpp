#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "../DirectXTK-main/Inc/SimpleMath.h"

using Microsoft::WRL::ComPtr;

class SpriteShadowInfo
{
public:
	int16_t OffsetX;
	int16_t OffsetY;

	int16_t ProjX;
	int16_t ProjY;

	inline void InitSizeAsOrth()
	{
		ProjX = 0;
		ProjY = 0;
	}

	inline void InitSizeAsProj(int16_t w, int16_t h)
	{
		ProjX = h / 2;
		ProjY = -h / 2;
	}
};

class Sprite
{
public:
	Sprite() :
		Rect(), Pivot(0.5, 0.5), TextureSRV(), GenerateShadow{ false }, ShadowInfo{}
	{
	}

	DirectX::SimpleMath::Rectangle Rect;
	DirectX::XMFLOAT2 Pivot;
	ComPtr<ID3D11ShaderResourceView> TextureSRV;

	bool GenerateShadow;
	SpriteShadowInfo ShadowInfo;
};

