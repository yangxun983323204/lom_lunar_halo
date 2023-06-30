#include "pch.h"
#include "SceneManager.h"

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
