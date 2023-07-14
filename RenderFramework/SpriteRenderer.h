#pragma once
#include <memory>
#include <windows.h>
#include "ISceneNodeComponent.h"
#include <DirectXMath.h>
#include "Sprite.hpp"
#include "IRenderer.h"

using std::weak_ptr;

class SpriteRenderer : public IRenderer
{
public:
	DirectX::SimpleMath::Rectangle GetWorldRect();
	virtual void OnRender(IGraphic2D* gfx) override;

	weak_ptr<Sprite> Sprite;
};

