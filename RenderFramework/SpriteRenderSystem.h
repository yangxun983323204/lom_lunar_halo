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
		_batch = std::make_unique<DirectX::SpriteBatch>(ctx);
		CreateDebugRes();
	}

	inline void SetLayout(int w, int h) { _vw = w; _vh = h; }
	inline void SetDPIScale(float s) { _dpiScale = s; }
	void Render();
	void RenderCamera(Camera* camera,function<bool(SpriteRenderer*)> filter=nullptr, bool debugMode = false);
	void RenderDebug();
private:
	void BeginDraw();
	void EndDraw();
	void Draw(XMINT2 vpos, ID3D11ShaderResourceView* srv, DirectX::SimpleMath::Rectangle viewRect, DirectX::XMFLOAT4 color);
	void DrawDebug(XMINT2 vpos, ID3D11ShaderResourceView* srv, DirectX::SimpleMath::Rectangle viewRect, DirectX::XMFLOAT4 color);
	void Clear(DirectX::XMFLOAT4 color);
	shared_ptr<Sprite> GenTexture(const byte* source, uint32_t width, uint32_t height);
	void CreateDebugRes();
	RECT GetDebugRect(int x, int y);

	HWND _hwnd;
	ID3D11Device* _dev;
	ID3D11DeviceContext* _ctx;
	ID3D11RenderTargetView* _rtv;
	ID3D11DepthStencilView* _dsv;

	unique_ptr<DirectX::SpriteBatch> _batch;

	int _vw, _vh;
	float _dpiScale;

	shared_ptr<Sprite> _debugImgLT;
	shared_ptr<Sprite> _debugImgRT;
	shared_ptr<Sprite> _debugImgLB;
	shared_ptr<Sprite> _debugImgRB;
	shared_ptr<Sprite> _debugImgCross;
};

