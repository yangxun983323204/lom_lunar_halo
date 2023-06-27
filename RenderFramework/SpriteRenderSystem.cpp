#include "SpriteRenderSystem.h"
#include "Camera.h"
#include "Sprite.hpp"
#include "SpriteRenderer.h"
#include <iostream>
#include <algorithm>
#include "GridViewComponent.hpp"

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
		cout << L"no camera!";
		return;
	}

	std::sort(cameras.begin(), cameras.end(), [](ISceneNodeComponent* a, ISceneNodeComponent* b) {
		return dynamic_cast<Camera*>(a)->Depth < dynamic_cast<Camera*>(b)->Depth;
		});

	for (auto camera : cameras)
	{
		RenderCamera(dynamic_cast<Camera*>(camera));
	}
}

void SpriteRenderSystem::RenderCamera(Camera* camera)
{
	if (camera->ClearType == Camera::ClearType::Color) {
		_clearFunc(camera->ClearColor);
	}

	auto spRenderers = ISystem::GetComponentsByType(SpriteRenderer::TypeId);
	std::sort(spRenderers.begin(), spRenderers.end(), [](ISceneNodeComponent*a, ISceneNodeComponent*b) {
		auto sa = dynamic_cast<SpriteRenderer*>(a)->SortLayer;
		auto sb = dynamic_cast<SpriteRenderer*>(b)->SortLayer;
		if (sa == sb) {
			auto wposA = a->GetSceneNode()->GetWorldPosition();
			auto wposB = b->GetSceneNode()->GetWorldPosition();
			return wposA.x < wposB.x || wposA.y > wposB.y;
		}
		else
			return sa < sb;
		});

	for (auto spRenderer : spRenderers) {
		auto sr = dynamic_cast<SpriteRenderer*>(spRenderer);
		auto wRect = sr->GetWorldRect();
		auto cameraWPos = camera->GetSceneNode()->GetWorldPosition();
		wRect.x -= cameraWPos.x;
		wRect.y -= cameraWPos.y;
		_renderFunc(sr->Sprite->TextureSRV.Get(), wRect, sr->Color);
	}
}