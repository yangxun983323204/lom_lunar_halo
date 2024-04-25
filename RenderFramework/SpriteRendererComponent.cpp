#include "SpriteRendererComponent.h"
#include "SceneNode.h"
#include "SpriteRenderSystem.h"

DirectX::SimpleMath::Rectangle SpriteRendererComponent::GetWorldRect()
{
	if (GetSceneNode() == nullptr)
		return DirectX::SimpleMath::Rectangle();

	if (Sprite.expired())
		return DirectX::SimpleMath::Rectangle();

	auto wPos = GetSceneNode()->GetWorldPosition();
	auto sp = Sprite.lock();
	auto rect = sp->Rect;
#ifdef SCENE_NODE_SCALE
	auto wScale = GetSceneNode()->GetWorldScale();
	rect.x *= wScale.x;
	rect.y *= wScale.y;
	rect.width *= wScale.x;
	rect.height *= wScale.y;
	OverrideWidth *= wScale.x;
	OverrideHeight *= wScale.y;
#endif
	auto w = OverrideWidth <= 0 ? rect.width : OverrideWidth;
	auto h = OverrideHeight <= 0 ? rect.height : OverrideHeight;
	auto offsetX = -w * sp->Pivot.x;
	auto offsetY = -h * sp->Pivot.y;

	rect.x += wPos.x + offsetX;
	rect.y += wPos.y + offsetY;
	rect.width = w;
	rect.height = h;
	return rect;
}

void SpriteRendererComponent::OnRender(IGraphic2D* gfx)
{
	if (Sprite.expired())
		return;

	auto wpos = GetSceneNode()->GetWorldPosition();
	auto wRect = GetWorldRect();
	auto sprite = Sprite.lock();
	gfx->SetModeNormal();
	gfx->Draw(sprite->TextureSRV.Get(), wRect, Color);
}

void SpriteRendererComponent::OnRenderDebug(SpriteRenderSystem* renderSystem, IGraphic2D* gfx)
{
	if (Sprite.expired() || !Debug)
		return;

	auto wpos = GetSceneNode()->GetWorldPosition();
	auto wRect = GetWorldRect();
	auto sprite = Sprite.lock();
	renderSystem->DrawDebugBounds(wpos, wRect, DebugColor);
}

void SpriteRendererComponent::FillTypeIds(std::set<uint32_t>& set)
{
	IRendererComponent::FillTypeIds(set);
	set.insert(SpriteRendererComponent::TypeId);
}
