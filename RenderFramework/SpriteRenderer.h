#pragma once
#include <memory>
#include <windows.h>
#include "ISceneNodeComponent.h"
#include <DirectXMath.h>
#include "Sprite.hpp"
#include "IRenderer.h"

using std::weak_ptr;
class SpriteRenderSystem;

_SCENE_NODE_COMPONENT_BEGIN1(SpriteRenderer, IRenderer)
public:
	SpriteRenderer():
		IRenderer()
	{ 
		DebugColor = DirectX::XMFLOAT4{ 0,0,1,1 };
		Debug = false;
	}

	DirectX::SimpleMath::Rectangle GetWorldRect();
	virtual void OnRender(IGraphic2D* gfx) override;
	virtual void OnRenderDebug(SpriteRenderSystem* renderSystem, IGraphic2D* gfx) override;

	weak_ptr<Sprite> Sprite;
	DirectX::XMFLOAT4 DebugColor;
	bool Debug;
_SCENE_NODE_COMPONENT_END
