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
			Text();
			void Set(wstring val,uint8_t fontSize = 15);
			inline wstring Get() { return _text; }
			virtual void Draw() override;
			virtual void LoadXml(tinyxml2::XMLElement* e) override;
		private:
			wstring _text;
			uint8_t _fontSize;
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

