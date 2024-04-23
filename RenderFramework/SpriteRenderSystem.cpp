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
		RenderCamera(dynamic_cast<Camera*>(camera), nullptr, false);
	}
}

void SpriteRenderSystem::RenderCamera(Camera* camera, function<bool(IRenderer*)> filter, bool debugMode)
{
	_currCamera = camera;
	if (camera->ClearType == Camera::ClearType::Color) {
		Clear(camera->ClearColor);
	}

	auto renderers = ISystem::GetComponentsByType(_ID_OF(IRenderer));
	renderers.sort([](ISceneNodeComponent*a, ISceneNodeComponent*b) {
		auto sa = dynamic_cast<IRenderer*>(a)->SortLayer;
		auto sb = dynamic_cast<IRenderer*>(b)->SortLayer;
		if (sa == sb) {
			auto wposA = a->GetSceneNode()->GetWorldPosition();
			auto wposB = b->GetSceneNode()->GetWorldPosition();
			if (wposA.y == wposB.y) {
				if (wposA.x == wposB.x)
					return dynamic_cast<IRenderer*>(a)->Depth < dynamic_cast<IRenderer*>(b)->Depth;
				else
					return wposA.x < wposB.x;
			}
			else
				return wposA.y > wposB.y;
		}
		else
			return sa < sb;
		});

	auto cameraWPos = camera->GetSceneNode()->GetWorldPosition();
	_draw->Begin(cameraWPos, _dpiScale);
	for (auto renderer : renderers) {
		auto sr = dynamic_cast<IRenderer*>(renderer);
		if (!sr || !renderer->Enable)
			continue;

		if (filter && !filter(sr))
			continue;

		sr->OnRender(_draw.get());
		if (Debug)
		{
			sr->OnRenderDebug(this, _draw.get());
		}
	}
	_draw->End();
}

void SpriteRenderSystem::DrawDebugBounds(XMINT2 wpos, DirectX::SimpleMath::Rectangle wRect, DirectX::XMFLOAT4 color)
{
	auto xMin = wRect.x;
	auto xMax = wRect.x + wRect.width;
	auto yMin = wRect.y;
	auto yMax = wRect.y + wRect.height;
	// 这些小图标都是5*5的大小
	// draw cross
	_draw->Draw(_debugImgCross->TextureSRV.Get(), {wpos.x-2, wpos.y-2, 5, 5}, color);
	// draw left top
	_draw->Draw(_debugImgLT->TextureSRV.Get(), {xMin, yMax-5, 5, 5}, color);
	// draw right top
	_draw->Draw(_debugImgRT->TextureSRV.Get(), {xMax-5, yMax-5, 5, 5}, color);
	// draw right bottom
	_draw->Draw(_debugImgRB->TextureSRV.Get(), {xMax-5, yMin, 5, 5 }, color);
	// draw left bottom
	_draw->Draw(_debugImgLB->TextureSRV.Get(), {xMin, yMin, 5, 5 }, color);
}

void SpriteRenderSystem::DrawDebugOverlay(XMINT2 wpos, DirectX::SimpleMath::Rectangle wRect, DirectX::XMFLOAT4 color)
{
	auto xMin = wRect.x;
	auto xMax = wRect.x + wRect.width;
	auto yMin = wRect.y;
	auto yMax = wRect.y + wRect.height;
	if (color.w>=0.6)
		color.w = 0.6;

	_draw->Draw(_debugImgWhite->TextureSRV.Get(), wRect, color);
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


	byte white[] = {
		col, col, col, col, col,
		col, col, col, col, col,
		col, col, col, col, col,
		col, col, col, col, col,
		col, col, col, col, col,
	};
	_debugImgWhite = GenTexture(white, 5, 5);
	_debugImgWhite->Pivot = { 0.5, 0.5};
}
