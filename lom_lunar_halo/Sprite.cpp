#include "pch.h"
#include "../DirectXTK-main/Inc/BufferHelpers.h"
#include "Sprite.h"
#include <vector>

using namespace DirectX;
using YX::Sprite;

shared_ptr<Sprite> Sprite::CreateFromWIL(ID3D11Device* device, ImageLib::ImageInfo& info, std::vector<uint32_t>&& rgba32)
{
	if (rgba32.size() <= 0)
		return {};

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
	DX::ThrowIfFailed(DirectX::CreateTextureFromMemory(
		device,
		info.Width, info.Height,
		DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		(const D3D11_SUBRESOURCE_DATA)subData,
		sprite->_tex.GetAddressOf(), sprite->_texSRV.GetAddressOf()));

	return sprite;
}

shared_ptr<Sprite> Sprite::CreateRenderTarget(ID3D11Device* device, int16_t w, int16_t h, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	auto sprite = std::make_shared<Sprite>();

	sprite->_w = w;
	sprite->_h = h;
	sprite->_pivotX = 0;
	sprite->_pivotY = 0;
	sprite->_hasShadow = false;
	sprite->_shadowPosX = 0;
	sprite->_shadowPosY = 0;

	D3D11_SUBRESOURCE_DATA subData{ 0 };
	uint32_t rgba32 = a << 24 | b << 16 | g << 8 | r;

	const std::vector<uint32_t>::size_type size = w * h;
	std::vector<uint32_t> data(size,rgba32);
	subData.pSysMem = data.data();
	subData.SysMemPitch = w * 4;
	DX::ThrowIfFailed(DirectX::CreateTextureFromMemory(
		device,
		w, h,
		DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		(const D3D11_SUBRESOURCE_DATA)subData,
		sprite->_tex.GetAddressOf(), sprite->_texSRV.GetAddressOf(),
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET));

	return sprite;
}