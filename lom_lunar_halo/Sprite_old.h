// d3d11基本不会设备丢失:
// https://docs.microsoft.com/zh-cn/windows/uwp/gaming/handling-device-lost-scenarios
#pragma once
#include <memory>
#include "../mir_lib/MirConst.h"
#include "../mir_lib/ImageLib.h"

using Microsoft::WRL::ComPtr;
using std::shared_ptr;

namespace YX {
	class Sprite_old
	{
	public:
		inline int16_t Width() { return _w; }
		inline int16_t Height() { return _h; }
		inline int16_t PivotX() { return _pivotX; }
		inline int16_t PivotY() { return _pivotY; }
		inline Mir::ShadowType GetShadowType() { return _shadow; }
		inline int16_t ShadowPosX() { return _shadowPosX; }
		inline int16_t ShadowPosY() { return _shadowPosY; }
		inline ComPtr<ID3D11Texture2D> Texture() { return _tex; }
		inline ComPtr<ID3D11ShaderResourceView> SRV() { return _texSRV; }
		inline ComPtr<ID3D11RenderTargetView> RTV() { return _texRTV; }// todo

		static shared_ptr<Sprite_old> CreateRenderTarget(ID3D11Device* device, int16_t w, int16_t h, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		static shared_ptr<Sprite_old> CreateFromWIL(ID3D11Device* device, ImageLib::ImageInfo& info, std::vector<uint32_t>&& rgba32);
	protected:
		int16_t _w, _h;
		int16_t _pivotX, _pivotY;
		Mir::ShadowType _shadow;
		int16_t _shadowPosX;
		int16_t _shadowPosY;

		ComPtr<ID3D11Texture2D> _tex;
		ComPtr<ID3D11ShaderResourceView> _texSRV;
		ComPtr<ID3D11RenderTargetView> _texRTV;
	};
}
