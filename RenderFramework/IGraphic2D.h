#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../DirectXTK-main/Inc/SimpleMath.h"

class IGraphic2D
{
public:
	virtual void Draw(ID3D11ShaderResourceView* texture, DirectX::SimpleMath::Rectangle const& rect, DirectX::XMFLOAT4 color) = 0;

	virtual void SetModeNormal() = 0;
	virtual void SetModeShadowOrth() = 0;
	virtual void SetModeShadowProj() = 0;

	virtual void SetShadowColor(DirectX::XMFLOAT4 color) = 0;
	virtual void SetShadowProjOrigin(DirectX::XMINT2 origin) = 0;
};