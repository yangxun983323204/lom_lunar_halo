#include "pch.h"
#include "../DirectXTK-main/Inc/BufferHelpers.h"
#include "Sprite.h"

using namespace DirectX;
using namespace DX;
using YX::Sprite;

shared_ptr<Sprite> Sprite::CreateFromWIL(ID3D11Device* device, ImageLib::ImageInfo& info, std::vector<uint32_t>&& rgba32)
{
	auto sprite = std::make_shared<Sprite>();

	sprite->_w = info.Width;
	sprite->_h = info.Height;
	sprite->_pivotX = info.PivotX;
	sprite->_pivotY = info.PivotY;
	sprite->_hasShadow = info.HasShadow != '0';
	sprite->_shadowPosX = info.ShadowPosX;
	sprite->_shadowPosY = info.ShadowPosY;

	D3D11_SUBRESOURCE_DATA subData{ 0 };
	subData.pSysMem = rgba32.data();
	subData.SysMemPitch = info.Width * 4;
	ThrowIfFailed(DirectX::CreateTextureFromMemory(
		device,
		info.Width, info.Height,
		DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		(const D3D11_SUBRESOURCE_DATA)subData,
		sprite->_tex.GetAddressOf(), sprite->_texSRV.GetAddressOf()));

	return sprite;
}