#include "SpriteRenderer.h"
#include "SceneNode.h"

const int SpriteRenderer::GetTypeId() { return TypeId; }

DirectX::SimpleMath::Rectangle SpriteRenderer::GetWorldRect()
{
	if (GetSceneNode() == nullptr)
		return DirectX::SimpleMath::Rectangle();

	if (!Sprite)
		return DirectX::SimpleMath::Rectangle();

	auto wPos = GetSceneNode()->GetWorldPosition();
	auto rect = Sprite->Rect;
#ifdef SCENE_NODE_SCALE
	auto wScale = GetSceneNode()->GetWorldScale();
	rect.x *= wScale.x;
	rect.y *= wScale.y;
	rect.width *= wScale.x;
	rect.height *= wScale.y;
#endif
	auto w = rect.width;
	auto h = rect.height;
	auto offsetX = -w * Sprite->Pivot.x;
	auto offsetY = -h * Sprite->Pivot.y;

	rect.x += wPos.x + offsetX;
	rect.y += wPos.y + offsetY;
	return rect;
}
