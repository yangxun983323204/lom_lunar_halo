#pragma once
#include "GUIBase.h"
#include "Canvas.h"
#include "Image.h"
#include "Text.h"
#include "../tinyxml2.h"

using std::string;
using std::wstring;
using std::shared_ptr;

namespace YX {
	namespace GUI {
		static class LayoutLoader
		{
		public:
			static shared_ptr<Canvas> Parse(wstring xmlPath);
		};
	}
}

