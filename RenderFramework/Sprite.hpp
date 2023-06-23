#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;

class Sprite
{
public:
	inline Sprite() :
		Rect(), Pivot(0.5, 0.5), TextureSRV()
	{
	}

	inline uint32_t GetWidth() { return Rect.right - Rect.left; }
	inline uint32_t GetHeight() { return Rect.top - Rect.bottom; }

	RECT Rect;
	DirectX::XMFLOAT2 Pivot;
	ComPtr<ID3D11ShaderResourceView> TextureSRV;
};

