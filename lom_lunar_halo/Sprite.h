// d3d11���������豸��ʧ:
// https://docs.microsoft.com/zh-cn/windows/uwp/gaming/handling-device-lost-scenarios
#pragma once
#include <memory>
#include "../mir_lib/ImageLib.h"

using Microsoft::WRL::ComPtr;
using std::shared_ptr;

namespace YX {
	class Sprite
	{
	public:
		inline int16_t Width() { return _w; }
		inline int16_t Height() { return _h; }
		inline int16_t PivotX() { return _pivotX; }
		inline int16_t PivotY() { return _pivotY; }
		inline bool HasShadow() { return _hasShadow; }
		inline int16_t ShadowPosX() { return _shadowPosX; }
		inline int16_t ShadowPosY() { return _shadowPosY; }
		inline ComPtr<ID3D11ShaderResourceView> SRV() { return _texSRV; }

		static shared_ptr<Sprite> CreateFromWIL(ID3D11Device* device, ImageLib::ImageInfo& info, std::vector<uint32_t>&& rgba32);
	protected:
		int16_t _w, _h;
		int16_t _pivotX, _pivotY;
		bool _hasShadow;
		int16_t _shadowPosX;
		int16_t _shadowPosY;

		ComPtr<ID3D11Texture2D> _tex;
		ComPtr<ID3D11ShaderResourceView> _texSRV;
	};
}