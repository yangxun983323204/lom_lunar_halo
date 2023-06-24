#include "SpriteRenderer.h"
#include "SceneNode.h"

const int SpriteRenderer::GetTypeId() { return TypeId; }

RECT SpriteRenderer::GetWorldRect()
{
	if (GetSceneNode() == nullptr)
		return RECT();

	if (!Sprite)
		return RECT();

	auto wPos = GetSceneNode()->GetWorldPosition();
	auto rect = Sprite->Rect;
#ifdef SCENE_NODE_SCALE
	auto wScale = GetSceneNode()->GetWorldScale();
	rect.left *= wScale.x;
	rect.right *= wScale.x;
	rect.top *= wScale.y;
	rect.bottom *= wScale.y;
#endif
	auto w = rect.right - rect.left;
	auto h = rect.bottom - rect.top;
	auto offsetX = -w * Sprite->Pivot.x;
	auto offsetY = -h * Sprite->Pivot.y;

	rect.left += wPos.x + offsetX;
	rect.right += wPos.x + offsetX;
	rect.bottom += wPos.y + offsetY;
	rect.top += wPos.y + offsetY;
	return rect;
}
