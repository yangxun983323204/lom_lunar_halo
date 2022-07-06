#include "pch.h"
#include "Text.h"

using namespace YX::GUI;

Text::Text(DX::DeviceResources* dev):
	_dev{ dev },
	_text{},
	_textDirty{ false },
	_rectDirty{ true },
	_preWidth{ Width },
	_preHeight{ Height }
{

}

void Text::Set(wstring val)
{
	_text = val;
	_textDirty = true;
}

void Text::Draw(SpriteBatch* batch)
{
	CheckRectDirty();
	if (_textDirty || _rectDirty)
	{
		UpdateSprite();
		_textDirty = false;
		_rectDirty = false;
	}

	Graphic::Draw(batch);
}

void Text::CheckRectDirty()
{
	if (!_rectDirty) {
		_rectDirty = _preWidth != Width ||
			_preHeight != Height;
	}

	_preWidth = Width;
	_preHeight = Height;
}

void Text::UpdateSprite()
{
	if (_rectDirty)
	{
		// 先重建精灵，再渲染文本
		_sprite.reset();
		_sprite = Sprite::CreateRenderTarget(_dev->GetD3DDevice(), Width, Height, 0, 0, 0, 0);
	}

	ComPtr<ID2D1Factory> pD2DFactory{};
	DX::ThrowIfFailed(
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			pD2DFactory.GetAddressOf()
		)
	);

	ComPtr<IDXGISurface> pDxgiSurface{};
	DX::ThrowIfFailed(_sprite->Texture()->QueryInterface(pDxgiSurface.GetAddressOf()));
	// Create a D2D render target that can draw into our offscreen D3D
	// surface. Given that we use a constant size for the texture, we
	// fix the DPI at 96.
	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96,
			96);

	DX::ThrowIfFailed(
		pD2DFactory->CreateDxgiSurfaceRenderTarget(
			pDxgiSurface.Get(),
			&props,
			_d2dRT.GetAddressOf())
	);

	// Create a DirectWrite factory.
	static const WCHAR msc_fontName[] = L"Verdana";
	static const FLOAT msc_fontSize = 15;
	ComPtr<IDWriteFactory> m_pDWriteFactory{};
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(m_pDWriteFactory),
		reinterpret_cast<IUnknown**>(m_pDWriteFactory.GetAddressOf())
	);

	ComPtr<IDWriteTextFormat> m_pTextFormat{};
	// Create a DirectWrite text format object.
	DX::ThrowIfFailed(
		m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			m_pTextFormat.GetAddressOf()
		)
	);

	// Center the text horizontally and vertically.
	m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// Retrieve the size of the render target.
	D2D1_SIZE_F renderTargetSize = _d2dRT->GetSize();

	_d2dRT->BeginDraw();
	_d2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
	_d2dRT->Clear(D2D1::ColorF(0,0,0,0));

	ComPtr<ID2D1SolidColorBrush> brush{};
	_d2dRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		brush.GetAddressOf()
	);

	_d2dRT->DrawText(
		_text.c_str(),
		_text.size()/*ARRAYSIZE(_text.c_str()) - 1*/,
		m_pTextFormat.Get(),
		D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
		brush.Get()
	);

	DX::ThrowIfFailed(_d2dRT->EndDraw());
}