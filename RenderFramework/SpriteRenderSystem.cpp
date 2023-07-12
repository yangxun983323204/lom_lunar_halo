#include "SpriteRenderSystem.h"
#include "Camera.h"
#include "Sprite.hpp"
#include "SpriteRenderer.h"
#include <iostream>
#include <algorithm>
#include <DirectXMath.h>
#include "GridViewComponent.hpp"
#include "../DirectXTK-main/Inc/BufferHelpers.h"

using std::cout;

void SpriteRenderSystem::Render()
{
	auto gridViews = ISystem::GetComponentsByType(GridViewComponent::TypeId);
	for (auto view : gridViews)
	{
		dynamic_cast<GridViewComponent*>(view)->SyncNodePos();
	}

	auto cameras = ISystem::GetComponentsByType(Camera::TypeId);
	if (cameras.size()<=0)
	{
		cout << "no camera!";
		return;
	}

	cameras.sort([](ISceneNodeComponent* a, ISceneNodeComponent* b) {
		return dynamic_cast<Camera*>(a)->Depth < dynamic_cast<Camera*>(b)->Depth;
		});

	for (auto camera : cameras)
	{
		RenderCamera(dynamic_cast<Camera*>(camera));
	}
}

void SpriteRenderSystem::RenderCamera(Camera* camera, function<bool(SpriteRenderer*)> filter, bool debugMode)
{
	if (camera->ClearType == Camera::ClearType::Color) {
		Clear(camera->ClearColor);
	}

	auto spRenderers = ISystem::GetComponentsByType(SpriteRenderer::TypeId);
	spRenderers.sort([](ISceneNodeComponent*a, ISceneNodeComponent*b) {
		auto sa = dynamic_cast<SpriteRenderer*>(a)->SortLayer;
		auto sb = dynamic_cast<SpriteRenderer*>(b)->SortLayer;
		if (sa == sb) {
			auto wposA = a->GetSceneNode()->GetWorldPosition();
			auto wposB = b->GetSceneNode()->GetWorldPosition();
			if (wposA.y == wposB.y) {
				if (wposA.x == wposB.x)
					return dynamic_cast<SpriteRenderer*>(a)->Depth < dynamic_cast<SpriteRenderer*>(b)->Depth;
				else
					return wposA.x < wposB.x;
			}
			else
				return wposA.y > wposB.y;
		}
		else
			return sa < sb;
		});

	BeginDraw();
	for (auto spRenderer : spRenderers) {
		auto sr = dynamic_cast<SpriteRenderer*>(spRenderer);
		if (sr->Sprite.expired())
			continue;
		if (filter && !filter(sr))
			continue;

		auto wpos = sr->GetSceneNode()->GetWorldPosition();
		auto viewRect = sr->GetWorldRect();
		auto cameraWPos = camera->GetSceneNode()->GetWorldPosition();
		viewRect.x -= cameraWPos.x;
		viewRect.y -= cameraWPos.y;
		wpos.x -= cameraWPos.x;
		wpos.y -= cameraWPos.y;
		auto sprite = sr->Sprite.lock();
		if (!debugMode) {
			if (sprite->GenerateShadow) {
				DirectX::SimpleMath::Rectangle shadowRect = {
					viewRect.x + sprite->ShadowInfo.OffsetX, viewRect.y + sprite->ShadowInfo.OffsetY,
					sprite->ShadowInfo.Width(viewRect.width, viewRect.height), sprite->ShadowInfo.Height(viewRect.width, viewRect.height)
				};

				Draw(wpos, sprite->TextureSRV.Get(), shadowRect, _shadowColor);
			}
			Draw(wpos, sprite->TextureSRV.Get(), viewRect, sr->Color);
		}
		else
			DrawDebug(wpos, sprite->TextureSRV.Get(), viewRect, sr->__debugColor);
	}
	EndDraw();
}

void SpriteRenderSystem::RenderDebug()
{
	auto cameras = ISystem::GetComponentsByType(Camera::TypeId);
	if (cameras.size() <= 0)
	{
		cout << "no camera!";
		return;
	}

	cameras.sort([](ISceneNodeComponent* a, ISceneNodeComponent* b) {
		return dynamic_cast<Camera*>(a)->Depth < dynamic_cast<Camera*>(b)->Depth;
		});

	for (auto camera : cameras)
	{
		RenderCamera(dynamic_cast<Camera*>(camera), [](SpriteRenderer* sr) {
			return sr->__debug;
			}, true);
	}
}

void SpriteRenderSystem::BeginDraw()
{
	_draw->Begin();
}

void SpriteRenderSystem::EndDraw()
{
	_draw->End();
}

void SpriteRenderSystem::Draw(XMINT2 vpos, ID3D11ShaderResourceView* srv, DirectX::SimpleMath::Rectangle viewRect, DirectX::XMFLOAT4 color)
{
	// 目地：观察坐标->屏幕坐标
	int offsetX = _vw / 2;
	int offsetY = -_vh / 2;
	// 变换到左上角
	viewRect.x += offsetX;
	viewRect.y += offsetY;
	// y轴反向
	viewRect.y *= -1;
	RECT sRect = { viewRect.x, viewRect.y - viewRect.height, viewRect.x + viewRect.width, viewRect.y };
	auto f4 = DirectX::XMLoadFloat4(&color);
	sRect.left = sRect.left * _dpiScale;
	sRect.right = sRect.right * _dpiScale;
	sRect.bottom = sRect.bottom * _dpiScale;
	sRect.top = sRect.top * _dpiScale;
	_draw->Draw(srv, sRect, f4);
}

void SpriteRenderSystem::DrawDebug(XMINT2 vpos, ID3D11ShaderResourceView* srv, DirectX::SimpleMath::Rectangle viewRect, DirectX::XMFLOAT4 color)
{
	// 目地：观察坐标->屏幕坐标
	int offsetX = _vw / 2;
	int offsetY = -_vh / 2;
	// 变换到左上角
	viewRect.x += offsetX;
	viewRect.y += offsetY;
	vpos.x += offsetX; vpos.y += offsetY;
	// y轴反向
	viewRect.y *= -1;
	vpos.y *= -1;
	RECT sRect = { viewRect.x, viewRect.y - viewRect.height, viewRect.x + viewRect.width, viewRect.y };

	auto centerX = (sRect.right + sRect.left) / 2;
	auto centerY = (sRect.bottom + sRect.top) / 2;
	auto f4 = DirectX::XMLoadFloat4(&color);
	// draw cross
	RECT cross = GetDebugRect(vpos.x, vpos.y);
	_draw->Draw(_debugImgCross->TextureSRV.Get(), cross, f4);
	// draw left top
	RECT lt = GetDebugRect(sRect.left, sRect.top);
	_draw->Draw(_debugImgLT->TextureSRV.Get(), lt, f4);
	// draw right top
	RECT rt = GetDebugRect(sRect.right, sRect.top);
	_draw->Draw(_debugImgRT->TextureSRV.Get(), rt, f4);
	// draw right bottom
	RECT rb = GetDebugRect(sRect.right, sRect.bottom);
	_draw->Draw(_debugImgRB->TextureSRV.Get(), rb, f4);
	// draw left bottom
	RECT lb = GetDebugRect(sRect.left, sRect.bottom);
	_draw->Draw(_debugImgLB->TextureSRV.Get(), lb, f4);
}

void SpriteRenderSystem::Clear(DirectX::XMFLOAT4 color)
{
	_ctx->ClearRenderTargetView(_rtv, (const FLOAT*)&color);
	_ctx->ClearDepthStencilView(_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
shared_ptr<Sprite> SpriteRenderSystem::GenTexture(const byte* source, uint32_t width, uint32_t height)
{
	auto sp = std::make_shared<Sprite>();
	D3D11_SUBRESOURCE_DATA subData{ 0 };
	subData.pSysMem = source;
	subData.SysMemPitch = width * 4;
	DirectX::CreateTextureFromMemory(
		_dev,
		width, height,
		DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		(const D3D11_SUBRESOURCE_DATA)subData,
		nullptr, sp->TextureSRV.GetAddressOf());

	sp->Pivot = { 0.5,0.5 };
	sp->Rect = { 0,0,(long)width, (long)width };
	return sp;
}
void SpriteRenderSystem::CreateDebugRes()
{
#define col 255,255,255,255
#define nil 0,0,0,0
	byte cross[] = {
		nil, nil, col, nil,	nil,
		nil, nil, col, nil,	nil,
		col, col, col, col,	col,
		nil, nil, col, nil,	nil,
		nil, nil, col, nil,	nil,
	};
	_debugImgCross = GenTexture(cross, 5, 5);
	_debugImgCross->Pivot = { 0.5,0.5 };
	
	byte lt[] = {
		col, col, col, col, col,
		col, nil, nil, nil, nil,
		col, nil, nil, nil, nil,
		col, nil, nil, nil, nil,
		col, nil, nil, nil, nil,
	};
	_debugImgLT = GenTexture(lt, 5, 5);
	_debugImgLT->Pivot = { 0,1 };

	byte rt[] = {
		col, col, col, col, col,
		nil, nil, nil, nil, col,
		nil, nil, nil, nil, col,
		nil, nil, nil, nil, col,
		nil, nil, nil, nil, col,
	};
	_debugImgRT = GenTexture(rt, 5, 5);
	_debugImgRT->Pivot = { 1,1 };

	byte lb[] = {
		col, nil, nil, nil, nil,
		col, nil, nil, nil, nil,
		col, nil, nil, nil, nil,
		col, nil, nil, nil, nil,
		col, col, col, col, col,
	};
	_debugImgLB = GenTexture(lb, 5, 5);
	_debugImgLB->Pivot = { 0,0 };

	byte rb[] = {
		nil, nil, nil, nil, col,
		nil, nil, nil, nil, col,
		nil, nil, nil, nil, col,
		nil, nil, nil, nil, col,
		col, col, col, col, col,
	};
	_debugImgRB = GenTexture(rb, 5, 5);
	_debugImgRB->Pivot = { 1,0 };
}

RECT SpriteRenderSystem::GetDebugRect(int x, int y)
{
	RECT rc{ x - 4, y - 4, x + 4, y + 4 };
	rc.left *= _dpiScale;
	rc.right *= _dpiScale;
	rc.bottom *= _dpiScale;
	rc.top *= _dpiScale;
	return rc;
}
