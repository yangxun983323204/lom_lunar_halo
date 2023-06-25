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
	inline SceneManager(DX::DeviceResources* dr) 
		: _dr{ dr }, _width{ 1920 }, _height{1080}
	{
		_renderSystem = std::make_unique<SpriteRenderSystem>();
		auto clearFunc = std::bind(&SceneManager::ClearScreen, this, std::placeholders::_1);
		_renderSystem->SetClearFunc(clearFunc);
		auto renderFunc = std::bind(&SceneManager::Draw, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		_renderSystem->SetRenderFunc(renderFunc);
		_root = std::make_shared<SceneNode>();
	}

	inline void SetScreen(int width, int height) { _width = width; _height = height; }
	void Render();

	shared_ptr<SceneNode> CreateNode();
	shared_ptr<SceneNode> CreateSpriteNode();
	shared_ptr<SceneNode> CreateCameraNode();

private:
	void ClearScreen(XMFLOAT4 color);
	void Draw(ID3D11ShaderResourceView* srv, RECT wRect, XMFLOAT4 color);

	int _width;
	int _height;
	DX::DeviceResources* _dr;

	shared_ptr<SceneNode> _root;
	unique_ptr<SpriteRenderSystem> _renderSystem;
};

