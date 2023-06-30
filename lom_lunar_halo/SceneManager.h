#pragma once
#include "../RenderFramework/RenderFramework.h"
#include <memory>
#include <string>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "DeviceResources.h"

using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using DirectX::XMFLOAT4;

class SceneManager
{
public:
	inline SceneManager() 
	{
		_root = std::make_shared<SceneNode>();
	}

	shared_ptr<SceneNode> CreateNode();
	shared_ptr<SceneNode> CreateSpriteNode();
	shared_ptr<SceneNode> CreateCameraNode();

private:
	shared_ptr<SceneNode> _root;
};

