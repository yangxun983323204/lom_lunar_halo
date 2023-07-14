#pragma once
#include <memory>
#include "SceneNode.h"
#include "ISceneNodeComponent.h"
#include "SpriteRenderer.h"
#include "Sprite.hpp"
#include "ISystem.h"
#include "Camera.h"
#include <functional>
#include <d3d11.h>
#include <DirectXMath.h>
#include "../DirectXTK-main/Inc/SpriteBatch.h"
#include <wrl.h>
#include "SimpleSpriteDraw.h"

using std::function;
using std::shared_ptr;
using Microsoft::WRL::ComPtr;
using DirectX::XMINT2;

class SpriteRenderSystem : public ISystem
{
public:
	SpriteRenderSystem(HWND hwnd,ID3D11Device* dev, ID3D11DeviceContext* ctx, 
		ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv,
		int layoutW, int layoutH, float dpiScale) :
		_hwnd{ hwnd }, _dev{ dev }, _ctx{ ctx }, _rtv{ rtv }, _dsv{ dsv }, 
		_vw{ layoutW }, _vh{ layoutH }, _dpiScale{ dpiScale }
	{
		_draw = std::make_unique<SimpleSpriteDraw>(ctx);
		_shadowColor = { 0,0,0,0.5 };
		CreateDebugRes();
	}

	inline void SetLayout(int w, int h) { _vw = w; _vh = h; }
	inline void SetDPIScale(float s) { _dpiScale = s; }
	void Render();
	
private:
	void RenderCamera(Camera* camera, function<bool(IRenderer*)> filter, bool debugMode);
	void Clear(DirectX::XMFLOAT4 color);
	shared_ptr<Sprite> GenTexture(const byte* source, uint32_t width, uint32_t height);
	void CreateDebugRes();
	RECT GetDebugRect(int x, int y);

	HWND _hwnd;
	ID3D11Device* _dev;
	ID3D11DeviceContext* _ctx;
	ID3D11RenderTargetView* _rtv;
	ID3D11DepthStencilView* _dsv;

	unique_ptr<SimpleSpriteDraw> _draw;

	DirectX::XMFLOAT4 _shadowColor;

	int _vw, _vh;
	float _dpiScale;

	shared_ptr<Sprite> _debugImgLT;
	shared_ptr<Sprite> _debugImgRT;
	shared_ptr<Sprite> _debugImgLB;
	shared_ptr<Sprite> _debugImgRB;
	shared_ptr<Sprite> _debugImgCross;
};

