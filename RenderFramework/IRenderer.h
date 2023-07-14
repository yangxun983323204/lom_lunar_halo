#pragma once
#include "ISceneNodeComponent.h"
#include "IGraphic2D.h"

class IRenderer :public ISceneNodeComponent
{
public:
	SCENE_NODE_COMPONENT('irdr');
	IRenderer():
		__debug { false }
	{
		Color = DirectX::XMFLOAT4{ 1,1,1,1 };
		__debugColor = DirectX::XMFLOAT4{ 1,1,1,1 };
	}
	virtual void OnRender(IGraphic2D* gfx) = 0;

	DirectX::XMFLOAT4 Color;
	int SortLayer = 0;
	int Depth = 0;
	bool __debug;
	DirectX::XMFLOAT4 __debugColor;
};