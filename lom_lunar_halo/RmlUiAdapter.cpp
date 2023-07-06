#include "pch.h"
#include "Game.h"
#include "RmlUiAdapter.h"
#include "RmlUi/Core.h"
#include "../deps/RmlUi-vs2017-win64/Include/RmlUi/Debugger.h"
#include <sstream>
#include <vector>
#include <functional>

using std::string;
using std::stringstream;
using std::vector;
using namespace YX;

RmlUiAdapter::RmlUiAdapter(Game* game):
	_game{game}, _textureIdGen {0},
	_textures{}, _dynamicTextures{}
{
	int w, h;
	game->GetCurrSize(w, h);
	// Instantiate the interfaces to RmlUi.
	//MyRenderInterface render_interface;
	//MySystemInterface system_interface;
	auto dr = game->GetDeviceResource();
	_renderer = std::make_unique<DX11RmlRenderer>(dr->GetD3DDevice(), dr->GetD3DDeviceContext());
	_renderer->SetLoadTextureFunc(std::bind(&RmlUiAdapter::LoadTexture, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_renderer->SetGenTextureFunc(std::bind(&RmlUiAdapter::GenTexture, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_renderer->SetReleaseTextureFunc(std::bind(&RmlUiAdapter::ReleaseTexture, this, std::placeholders::_1));
	_renderer->SetGetTextureFunc(std::bind(&RmlUiAdapter::GetTexture, this, std::placeholders::_1));
	_renderer->SetWindowSize(DPI_S(w), DPI_S(h));

	_system = std::make_unique<SystemInterface_Win32>();
	_system->SetWindow(dr->GetWindow());

	// Begin by installing the custom interfaces.
	//Rml::SetRenderInterface(&render_interface);
	//Rml::SetSystemInterface(&system_interface);
	Rml::SetRenderInterface(_renderer.get());
	Rml::SetSystemInterface(_system.get());

	// Now we can initialize RmlUi.
	Rml::Initialise();

	// Create a context next.
	_rmlCtx = Rml::CreateContext("main", Rml::Vector2i(DPI_S(w), DPI_S(h)));
	if (!_rmlCtx)
	{
		Rml::Shutdown();
		throw L"Rml::CreateContext失败";
	}
	// debugger
	//Rml::Debugger::Initialise(_rmlCtx);
	//Rml::Debugger::SetVisible(true);

	Rml::LoadFontFace("./font/NotoEmoji-Regular.ttf");
	Rml::LoadFontFace("./font/LatoLatin-Bold.ttf");
	Rml::LoadFontFace("./font/LatoLatin-BoldItalic.ttf");
	Rml::LoadFontFace("./font/LatoLatin-Italic.ttf");
	Rml::LoadFontFace("./font/LatoLatin-Regular.ttf");

	Rml::LoadFontFace("./font/NotoSansSC-Regular.otf");
}
RmlUiAdapter::~RmlUiAdapter()
{
	Rml::Shutdown();
}

void YX::RmlUiAdapter::Render()
{
	_renderer->OnBeginRender();
	_rmlCtx->Render();
	_renderer->OnEndRender();
}

void YX::RmlUiAdapter::OnWindowSizeChanged(int width, int height)
{
	_renderer->SetWindowSize(DPI_S(width), DPI_S(height));
	_rmlCtx->SetDimensions(Rml::Vector2i(DPI_S(width), DPI_S(height)));
}

bool YX::RmlUiAdapter::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source)
{
	if (source.rfind(WIL_PROTO, 0) != 0)
		return false;

	auto path = source.substr(WIL_PROTO.size());
	stringstream ss{ path };
	string item{};
	vector<string> split{};
	while (std::getline(ss, item, '?'))
	{
		split.push_back(item);
	}

	if (split.size() != 2)
		return false;

	auto file = split[0];
	auto isNum = !file.empty() && std::find_if(file.begin(),
		file.end(), [](unsigned char c) { return !std::isdigit(c); }) == file.end();

	uint32_t fileIdx = 0;
	if (!isNum)
		fileIdx = _game->GetUiSpriteManager()->GetFileId(file);
	else
		fileIdx = (uint32_t)std::stoi(split[0]);

	uint32_t imgIdx = (uint32_t)std::stoi(split[1]);
	auto sp = _game->GetUiSpriteManager()->LoadSprite({ fileIdx, imgIdx });
	if (!sp)
		return false;

	texture_dimensions.x = sp->GetSprite()->Rect.width;
	texture_dimensions.y = sp->GetSprite()->Rect.height;
	++_textureIdGen;
	if (_textureIdGen == 0)
		_textureIdGen = 1;
	texture_handle = _textureIdGen;
	_textures[texture_handle] = sp;
	
	return true;
}

bool YX::RmlUiAdapter::GenTexture(Rml::TextureHandle& texture_handle, const byte* source, const Rml::Vector2i& source_dimensions)
{
	// source is an array of unsigned, 8-bit values in RGBA order
	auto sp = std::make_shared<Sprite>();
	D3D11_SUBRESOURCE_DATA subData{ 0 };
	subData.pSysMem = source;
	subData.SysMemPitch = source_dimensions.x * 4;
	DX::ThrowIfFailed(DirectX::CreateTextureFromMemory(
		_game->GetDeviceResource()->GetD3DDevice(),
		source_dimensions.x, source_dimensions.y,
		DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		(const D3D11_SUBRESOURCE_DATA)subData,
		nullptr, sp->TextureSRV.GetAddressOf()));

	sp->Pivot = { 0,0 };
	sp->Rect = { 0,0,source_dimensions.x, source_dimensions.y };
	++_textureIdGen;
	if (_textureIdGen == 0)
		_textureIdGen = 1;
	texture_handle = _textureIdGen;
	_dynamicTextures[texture_handle] = sp;

	return true;
}

void YX::RmlUiAdapter::ReleaseTexture(Rml::TextureHandle texture)
{
	auto f = _textures.find(texture);
	if (f != _textures.end())
	{
		_textures.erase(f);
	}
	else {
		auto f2 = _dynamicTextures.find(texture);
		if (f2 != _dynamicTextures.end())
		{
			_dynamicTextures.erase(f2);
		}
	}
}

ID3D11ShaderResourceView** YX::RmlUiAdapter::GetTexture(Rml::TextureHandle& texture_handle)
{
	auto f = _textures.find(texture_handle);
	if (f != _textures.end())
	{
		return f->second->GetSprite()->TextureSRV.GetAddressOf();
	}
	else {
		auto f2 = _dynamicTextures.find(texture_handle);
		if (f2!= _dynamicTextures.end())
		{
			return f2->second->TextureSRV.GetAddressOf();
		}
	}
	return nullptr;
}
