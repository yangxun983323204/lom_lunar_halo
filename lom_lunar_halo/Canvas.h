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
		class Canvas final:public GUIElement
		{
		public:
			int RenderOrder;
			Canvas();
			~Canvas();
			inline bool IsRoot() { return true; }
			inline bool Drawable() { return false; }
			void Update(float deltaSec) override;
			void Draw(SpriteBatch* batch);
			void Foreach(function<void(shared_ptr<GUIElement>)> func);
			static shared_ptr<Canvas> CreateFromXml(wstring path);

			static void UpdateAll(float deltaSec);
			static void DrawAll(SpriteBatch* batch);
		private:
			static std::vector<Canvas*> _canvasList;
		};
	}
}