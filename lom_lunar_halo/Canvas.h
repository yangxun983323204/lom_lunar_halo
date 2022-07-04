#pragma once
#include "GUIBase.h"
#include <memory>
#include "../DirectXTK-main/Inc/SpriteBatch.h"
#include <string>
#include <functional>

using std::shared_ptr;
using std::wstring;
using std::function;
using DirectX::SpriteBatch;

namespace YX {
	namespace GUI {
		class Canvas:public GUIElement
		{
		public:
			void Update(float deltaSec) override;
			void Draw(SpriteBatch* batch);
			void Foreach(function<void(shared_ptr<GUIElement>)> func);
			static shared_ptr<Canvas> CreateFromXml(wstring path);
		};
	}
}