#pragma once
#include <memory>
#include <windows.h>
#include "ISceneNodeComponent.h"
#include <DirectXMath.h>
#include "Sprite.hpp"
#include "IRenderer.h"

using std::weak_ptr;

_SCENE_NODE_COMPONENT_BEGIN1(SpriteRenderer, IRenderer)
public:
	DirectX::SimpleMath::Rectangle GetWorldRect();
	virtual void OnRender(IGraphic2D* gfx) override;

	weak_ptr<Sprite> Sprite;
_SCENE_NODE_COMPONENT_END
