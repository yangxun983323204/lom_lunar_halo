#include "pch.h"
#include "RmlUiUtility.h"
#include "RmlUi/Core.h"
#include <sstream>
#include <vector>

using std::string;
using std::stringstream;
using std::vector;
using namespace YX;

RmlUiUtility::RmlUiUtility(Game* game):
	_game{game}, _textureIdGen {0}
{
	// Instantiate the interfaces to RmlUi.
	//MyRenderInterface render_interface;
	//MySystemInterface system_interface;
	auto dr = game->GetDeviceResource();
	_renderer = std::make_unique<DX11RmlRenderer>(dr->GetD3DDevice(), dr->GetD3DDeviceContext());

	// Begin by installing the custom interfaces.
	//Rml::SetRenderInterface(&render_interface);
	//Rml::SetSystemInterface(&system_interface);
	Rml::SetRenderInterface(_renderer.get());

	// Now we can initialize RmlUi.
	Rml::Initialise();

	// Create a context next.
	int w, h;
	game->GetCurrSize(w, h);
	Rml::Context* context = Rml::CreateContext("main", Rml::Vector2i(w, h));
	if (!context)
	{
		Rml::Shutdown();
		throw L"Rml::CreateContext失败";
	}
}
RmlUiUtility::~RmlUiUtility()
{
	Rml::Shutdown();
}

bool YX::RmlUiUtility::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source)
{
	if (source.rfind("wil://", 0) != 0)
		return false;

	auto path = source.substr(6);
	stringstream ss{ path };
	string item{};
	vector<string> split{ 2 };
	while (std::getline(ss, item, '/'))
	{
		split.push_back(item);
	}

	if (split.size() != 2)
		return false;

	uint32_t fileIdx = (uint32_t)std::stoi(split[0]);
	uint32_t imgIdx = (uint32_t)std::stoi(split[1]);
	auto sp = _game->GetUiSpriteManager()->LoadSprite({ fileIdx, imgIdx });
	if (!sp)
		return false;

	texture_dimensions.x = sp->GetSprite()->Rect.width;
	texture_dimensions.y = sp->GetSprite()->Rect.height;
	texture_handle = ++_textureIdGen;
	_textures[texture_handle] = sp;
	
	return true;
}

bool YX::RmlUiUtility::GetTexutre(Rml::TextureHandle& texture_handle, const byte* source, const Rml::Vector2i& source_dimensions)
{
	return false;
}

void YX::RmlUiUtility::ReleaseTexture(Rml::TextureHandle texture)
{
}
