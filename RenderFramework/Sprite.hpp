#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "../DirectXTK-main/Inc/SimpleMath.h"

using Microsoft::WRL::ComPtr;

class Sprite
{
public:
	Sprite() :
		Rect(), Pivot(0.5, 0.5), TextureSRV()
	{
	}

	DirectX::SimpleMath::Rectangle Rect;
	DirectX::XMFLOAT2 Pivot;
	ComPtr<ID3D11ShaderResourceView> TextureSRV;
};

