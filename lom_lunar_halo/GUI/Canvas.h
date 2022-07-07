#pragma once
#include "GUIBase.h"
#include <memory>
#include <string>
#include <functional>

using std::shared_ptr;
using std::wstring;
using std::function;

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
			void Draw();
			void Foreach(function<void(shared_ptr<GUIElement>)> func);
			static shared_ptr<Canvas> CreateFromXml(wstring path);

			static void UpdateAll(float deltaSec);
			static void DrawAll();
		private:
			static std::vector<Canvas*> _canvasList;
		};
	}
}