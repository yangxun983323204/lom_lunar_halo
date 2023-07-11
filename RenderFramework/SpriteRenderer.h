#pragma once
#include <memory>
#include <windows.h>
#include "ISceneNodeComponent.h"
#include <DirectXMath.h>
#include "Sprite.hpp"

using std::weak_ptr;

class SpriteRenderer : public ISceneNodeComponent
{
public:
	SCENE_NODE_COMPONENT('sr');
	SpriteRenderer() :__debug{ false } 
	{
		Color = DirectX::XMFLOAT4{ 1,1,1,1 };
		__debugColor = DirectX::XMFLOAT4{ 1,1,1,1 };
	}
	DirectX::SimpleMath::Rectangle GetWorldRect();

	DirectX::XMFLOAT4 Color;
	int SortLayer = 0;
	int Depth = 0;
	weak_ptr<Sprite> Sprite;

	bool __debug;
	DirectX::XMFLOAT4 __debugColor;
};

