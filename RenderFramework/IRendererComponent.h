#pragma once
#include "ISceneNodeComponent.h"
#include "IGraphic2D.h"

class SpriteRenderSystem;

class IRendererComponent : public ISceneNodeComponent
{
public:
	_SCENE_NODE_COMPONENT_ID(IRendererComponent)
	IRendererComponent()
	{
		Color = DirectX::XMFLOAT4{ 1,1,1,1 };
	}
	virtual void OnRender(IGraphic2D* gfx) = 0;
	inline virtual void OnRenderDebug(SpriteRenderSystem* renderSystem, IGraphic2D* gfx) {};
	virtual void FillTypeIds(std::vector<uint32_t>& typeIds) override;

	DirectX::XMFLOAT4 Color;
	int SortLayer = 0;
	int Depth = 0;
	bool Debug = false;
};