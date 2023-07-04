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
	SCENE_NODE_COMPONENT('sr')
	DirectX::SimpleMath::Rectangle GetWorldRect();

	DirectX::XMFLOAT4 Color = DirectX::XMFLOAT4{ 1,1,1,1 };
	int SortLayer = 0;
	int Depth = 0;
	weak_ptr<Sprite> Sprite;
};

