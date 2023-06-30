#pragma once
#include "SceneNode.h"
#include "ISceneNodeComponent.h"
#include "SpriteRenderer.h"
#include "Sprite.hpp"
#include "ISystem.h"
#include "Camera.h"
#include <functional>

using std::function;
// SRV  观察坐标系(左下原点)下Rectangle  颜色
typedef function<void(ID3D11ShaderResourceView*, DirectX::SimpleMath::Rectangle, DirectX::XMFLOAT4)> RenderFunc;
typedef function<void(DirectX::XMFLOAT4)> ClearFunc;

class SpriteRenderSystem : public ISystem
{
public:
	inline void SetRenderFunc(RenderFunc func) { _renderFunc = func; }
	inline void SetClearFunc(ClearFunc func) { _clearFunc = func; }
	void Render();
	void RenderCamera(Camera* camera);

private:
	RenderFunc _renderFunc;
	ClearFunc _clearFunc;
};

