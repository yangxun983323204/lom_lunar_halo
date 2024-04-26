#pragma once
#pragma comment(lib,"RmlDebugger.lib")

#include <memory>
#include <unordered_map>
#include "../deps/RmlUi-vs2017-win64/Include/RmlUi/Core/Context.h"
#include "../deps/RmlUi-vs2017-win64/Include/RmlUi/Core/ElementDocument.h"
#include "DX11RmlRenderer.h"
#include "RmlUi_Platform_Win32.h"

using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::unordered_map;

class Game;
namespace YX {
	void RmlWindowProcedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

	class RmlUiAdapter 
	{
	public:
		// 表示从wil库中加载
		const string WIL_PROTO = "wil://";

		RmlUiAdapter(Game* game);
		~RmlUiAdapter();
		void Update();
		void Render();
		inline Rml::Context* GetContext() { return _rmlCtx; }
		void OnWindowSizeChanged(int width, int height);

	private:
		bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source);
		bool GenTexture(Rml::TextureHandle& texture_handle, const byte* source, const Rml::Vector2i& source_dimensions);
		void ReleaseTexture(Rml::TextureHandle texture);
		ID3D11ShaderResourceView** GetTexture(Rml::TextureHandle& texture_handle);
		// ctx由rml内部管理
		Rml::Context* _rmlCtx;

		Game* _game;
		uint32_t _textureIdGen;
		unique_ptr<DX11RmlRenderer> _renderer;
		unique_ptr<SystemInterface_Win32> _system;
		// 加载的图片，由UiSpriteManager管理
		unordered_map<Rml::TextureHandle, shared_ptr<SpriteResHandle>> _textures;
		// 从像素生成的图片，自己管理
		unordered_map<Rml::TextureHandle, shared_ptr<Sprite>> _dynamicTextures;
	};
}