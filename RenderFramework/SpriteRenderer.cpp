#include "SpriteRenderer.h"
#include "SceneNode.h"

DirectX::SimpleMath::Rectangle SpriteRenderer::GetWorldRect()
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
#endif
	auto w = rect.width;
	auto h = rect.height;
	auto offsetX = -w * sp->Pivot.x;
	auto offsetY = -h * sp->Pivot.y;

	rect.x += wPos.x + offsetX;
	rect.y += wPos.y + offsetY;
	return rect;
}
