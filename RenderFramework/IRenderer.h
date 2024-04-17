#pragma once
#include "ISceneNodeComponent.h"
#include "IGraphic2D.h"

class SpriteRenderSystem;

class IRenderer
{
public:
	IRenderer()
	{
		Color = DirectX::XMFLOAT4{ 1,1,1,1 };
	}
	virtual void OnRender(IGraphic2D* gfx) = 0;
	inline virtual void OnRenderDebug(SpriteRenderSystem* renderSystem,IGraphic2D* gfx) {};

	DirectX::XMFLOAT4 Color;
	int SortLayer = 0;
	int Depth = 0;
};