#pragma once
#include <string>
#include "GUIBase.h"
#include "DeviceResources.h"
#include "shtypes.h"
#include "dwrite.h"
#include "d2d1.h"

#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"d2d1.lib")

using std::wstring;

namespace YX {
	namespace GUI{
		class Text :public Graphic
		{
		public:
			Text(DX::DeviceResources* dev);
			void Set(wstring val);
			inline wstring Get() { return _text; }
			virtual void Draw(SpriteBatch* batch) override;
		private:
			DX::DeviceResources* _dev;
			wstring _text;
			ComPtr<ID2D1RenderTarget> _d2dRT;

			bool _textDirty;
			bool _rectDirty;

			int _preWidth;
			int _preHeight;

			void CheckRectDirty();
			void UpdateSprite();
		};
	}
}

