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
	// width = WidthFactorX*sprite.width + WidthFactorY*sprite.height;
	// height = HeightFactorX*sprite.width + HeightFactorY*sprite.height;

	float WidthFactorX;
	float WidthFactorY;
	float HeightFactorX;
	float HeightFactorY;

	inline long Width(long spriteWidth, long spriteHeight)
	{
		return WidthFactorX * spriteWidth + WidthFactorY * spriteHeight;
	}

	inline long Height(long spriteWidth, long spriteHeight)
	{
		return HeightFactorX * spriteWidth + HeightFactorY * spriteHeight;
	}

	inline void InitSizeAsOrth()
	{
		WidthFactorX = 1; WidthFactorY = 0;
		HeightFactorX = 0; HeightFactorY = 1;
	}

	inline void InitSizeAsProj()
	{
		WidthFactorX = 1; WidthFactorY = 0.5;
		HeightFactorX = 0; HeightFactorY = 1;
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

