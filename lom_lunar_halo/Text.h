#pragma once
#include <string>
#include "GUIBase.h"

using std::wstring;

namespace YX {
	namespace GUI{
		class Text :public Graphic
		{
		public:
			void Set(wstring val);
			wstring Get();
		private:
			wstring _text;
			bool _dirty;
		};
	}
}

