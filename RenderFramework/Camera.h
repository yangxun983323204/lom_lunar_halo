#pragma once
#include "ISceneNodeComponent.h"
#include <DirectXMath.h>

class Camera : public ISceneNodeComponent
{
public:
	enum ClearType
	{
		None,
		Color,
	};

	_SCENE_NODE_COMPONENT_ID(Camera)

	virtual void FillTypeIds(std::set<uint32_t>& set) override;

	int Depth = 0;
	Camera::ClearType ClearType = Camera::ClearType::None;
	DirectX::XMFLOAT4 ClearColor = { 0,0,0,1 };
};

