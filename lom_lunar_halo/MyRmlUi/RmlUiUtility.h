#pragma once
#include "../Game.h"
#include "DX11RmlRenderer.h"

namespace YX {
	class RmlUiUtility 
	{
	public:
		static void Init(Game* game);
		static void DeInit();
	private:
		static DX11RmlRenderer* pRenderer;
	};
}