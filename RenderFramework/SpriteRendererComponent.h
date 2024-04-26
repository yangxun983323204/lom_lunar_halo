#pragma once
#include <memory>
#include <windows.h>
#include "ISceneNodeComponent.h"
#include <DirectXMath.h>
#include "Sprite.hpp"
#include "IRendererComponent.h"

using std::weak_ptr;
class SpriteRenderSystem;

class SpriteRendererComponent : public IRendererComponent
{
public:
	_SCENE_NODE_COMPONENT_ID(SpriteRendererComponent)
	SpriteRendererComponent() :
		IRendererComponent()
	{
		DebugColor = DirectX::XMFLOAT4{ 0,0,1,1 };
	}

	DirectX::SimpleMath::Rectangle GetWorldRect();
	virtual void OnRender(IGraphic2D* gfx) override;
	virtual void OnRenderDebug(SpriteRenderSystem* renderSystem, IGraphic2D* gfx) override;
	virtual void FillTypeIds(std::vector<uint32_t>& typeIds) override;

	weak_ptr<Sprite> Sprite;
	int OverrideWidth;
	int OverrideHeight;
	DirectX::XMFLOAT4 DebugColor;
};
