#include "pch.h"
#include "RmlUiUtility.h"
#include "RmlUi/Core.h"

using namespace YX;

DX11RmlRenderer* RmlUiUtility::pRenderer = nullptr;

void RmlUiUtility::Init(Game* game)
{
	// Instantiate the interfaces to RmlUi.
	//MyRenderInterface render_interface;
	//MySystemInterface system_interface;
	auto dr = game->GetDeviceResource();
	pRenderer = new DX11RmlRenderer(dr->GetD3DDevice(), dr->GetD3DDeviceContext());

	// Begin by installing the custom interfaces.
	//Rml::SetRenderInterface(&render_interface);
	//Rml::SetSystemInterface(&system_interface);
	Rml::SetRenderInterface(pRenderer);

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
void RmlUiUtility::DeInit()
{
	delete pRenderer;
}