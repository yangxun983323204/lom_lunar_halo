#pragma once
#include <memory>
#include <unordered_map>
#include "Game.h"
#include "DX11RmlRenderer.h"

using std::unique_ptr;
using std::unordered_map;

namespace YX {
	class RmlUiUtility 
	{
	public:
		RmlUiUtility(Game* game);
		~RmlUiUtility();
	private:
		bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source);
		bool GetTexutre(Rml::TextureHandle& texture_handle, const byte* source, const Rml::Vector2i& source_dimensions);
		void ReleaseTexture(Rml::TextureHandle texture);

		Game* _game;
		uint32_t _textureIdGen;
		unique_ptr<DX11RmlRenderer> _renderer;
		unordered_map<uint32_t, shared_ptr<SpriteResHandle>> _textures;
	};
}