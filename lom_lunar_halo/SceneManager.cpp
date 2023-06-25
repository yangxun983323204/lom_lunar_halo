#include "pch.h"
#include "SceneManager.h"

void SceneManager::Render()
{
	_renderSystem->Render();
}

shared_ptr<SceneNode> SceneManager::CreateNode()
{
    auto node = std::make_shared<SceneNode>();
    node->SetLocalPosition({ 0,0 });
    node->SetParent(weak_ptr<SceneNode>(_root));
    return node;
}

shared_ptr<SceneNode> SceneManager::CreateSpriteNode()
{
    auto node = std::make_shared<SceneNode>();
    node->SetLocalPosition({ 0,0 });
    auto sp = std::make_shared<Sprite>();
    auto spRenderer = node->AddComponent<SpriteRenderer>().lock();
    spRenderer->Sprite = sp;
    node->SetParent(weak_ptr<SceneNode>(_root));
    return node;
}

shared_ptr<SceneNode> SceneManager::CreateCameraNode()
{
    auto node = std::make_shared<SceneNode>();
    node->SetLocalPosition({ 0,0 });
    node->AddComponent<Camera>();
    node->SetParent(weak_ptr<SceneNode>(_root));
    return node;
}

void SceneManager::ClearScreen(XMFLOAT4 color)
{
    auto context = _dr->GetD3DDeviceContext();
    auto renderTarget = _dr->GetRenderTargetView();
    auto depthStencil = _dr->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, (const FLOAT*)&color);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

extern void DrawTexture(ID3D11ShaderResourceView* srv, RECT rect);

void SceneManager::Draw(ID3D11ShaderResourceView* srv, RECT wRect, XMFLOAT4 color)
{
    int offsetX = _width / 2;
    int offsetY = _height / 2;
    RECT sRect = wRect;
    sRect.left += offsetX;
    sRect.right += offsetX;
    sRect.top += offsetY;
    sRect.bottom += offsetY;

    auto oldBottom = sRect.bottom;
    sRect.bottom = _height - sRect.top;
    sRect.top = _height - oldBottom;
    // todo 支持精灵颜色
    DrawTexture(srv, sRect);
}
