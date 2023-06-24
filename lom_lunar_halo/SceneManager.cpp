#include "pch.h"
#include "SceneManager.h"

void SceneManager::Render()
{
	_renderSystem->Render();
}

shared_ptr<SceneNode> SceneManager::CreateSpriteNode()
{
    auto node = std::make_shared<SceneNode>();
    node->SetLocalPosition({ 0,0 });
    auto sp = std::make_shared<Sprite>();
    auto spRenderer = node->AddComponent<SpriteRenderer>().lock();
    spRenderer->Sprite = sp;
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
    // todo 支持精灵颜色
    DrawTexture(srv, sRect);
}
