#include "pch.h"
#include "SceneManager.h"
#include "SpriteHandleHolder.hpp"
#include "ActorView.h"

SceneManager::SceneManager():
    _staticSpritePool{}, _animSpritePool{},
    _monsterPool{}, _npcPool{}, _playerPool{}
{
    _root = std::make_shared<SceneNode>();
    _spawnMonsterFunc = std::bind(&SceneManager::SpawnMonster, this);
    _spawnPlayerFunc = std::bind(&SceneManager::SpawnPlayer, this);
    _spawnNpcFunc = std::bind(&SceneManager::SpawnNpc, this);
}

shared_ptr<SceneNode> SceneManager::CreateNode()
{
    auto node = std::make_shared<SceneNode>();
    node->SetLocalPosition({ 0,0 });
    node->SetParent(weak_ptr<SceneNode>(_root));
    return node;
}

shared_ptr<SceneNode> SceneManager::CreateRawSpriteNode()
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

shared_ptr<SceneNode> SceneManager::SpawnStaticSprite()
{
    if (_staticSpritePool.size() > 0) {
        auto last = --_staticSpritePool.end();
        auto ret = *last;
        _staticSpritePool.pop_back();
        return ret;
    }

    auto node = CreateRawSpriteNode();
    node->AddComponent<SpriteHandleHolder>();
    return node;
}

void SceneManager::ReleaseStaticSprite(shared_ptr<SceneNode> ptr)
{
    ptr->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();
    ptr->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>()->Enable = false;
    _staticSpritePool.push_back(ptr);
}

shared_ptr<SceneNode> SceneManager::SpawnAnimSprite()
{
    if (_animSpritePool.size() > 0) {
        auto last = --_animSpritePool.end();
        auto ret = *last;
        _animSpritePool.pop_back();
        return ret;
    }

    auto node = CreateRawSpriteNode();
    node->AddComponent<SpriteHandleHolder>();
    node->AddComponent<Animator>();
    return node;
}

void SceneManager::ReleaseAnimSprite(shared_ptr<SceneNode> ptr)
{
    ptr->GetComponent<Animator>().lock()->As<Animator>()->Clear();
    ptr->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();
    ptr->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>()->Enable = false;
    _animSpritePool.push_back(ptr);
}

shared_ptr<SceneNode> SceneManager::SpawnMonster()
{
    if (_monsterPool.size() > 0) {
        auto last = --_monsterPool.end();
        auto ret = *last;
        _monsterPool.pop_back();
        return ret;
    }

    auto root = CreateNode();
    auto actor = SpawnAnimSprite();
    auto shadow = SpawnAnimSprite();

    actor->SetParent(root);
    actor->ResetTRS();
    shadow->SetParent(root);
    shadow->ResetTRS();

    actor->Name = Mir::MonsterSubPart::Actor;
    shadow->Name = Mir::MonsterSubPart::Shadow;

    root->AddComponent<ActorView>();

    return root;
}

void SceneManager::ReleaseMonster(shared_ptr<SceneNode> ptr)
{
    ptr->GetComponent<ActorView>().lock()->As<ActorView>()->Clear();
    _monsterPool.push_back(ptr);
}

shared_ptr<SceneNode> SceneManager::SpawnNpc()
{
    if (_npcPool.size() > 0) {
        auto last = --_npcPool.end();
        auto ret = *last;
        _npcPool.pop_back();
        return ret;
    }

    auto root = SpawnAnimSprite();
    root->AddComponent<ActorView>();
    return root;
}

void SceneManager::ReleaseNpc(shared_ptr<SceneNode> ptr)
{
    ptr->GetComponent<ActorView>().lock()->As<ActorView>()->Clear();
    _npcPool.push_back(ptr);
}

shared_ptr<SceneNode> SceneManager::SpawnPlayer()
{
    if (_playerPool.size() > 0) {
        auto last = --_playerPool.end();
        auto ret = *last;
        _playerPool.pop_back();
        return ret;
    }

    auto root = CreateNode();
    auto weapon = SpawnAnimSprite();
    auto horse = SpawnAnimSprite();
    auto actor = SpawnAnimSprite();
    auto effect = SpawnAnimSprite();
    auto shield = SpawnAnimSprite();

    weapon->SetParent(root); weapon->ResetTRS();
    horse->SetParent(root); horse->ResetTRS();
    actor->SetParent(root); actor->ResetTRS();
    effect->SetParent(root); effect->ResetTRS();
    shield->SetParent(root); shield->ResetTRS();

    weapon->Name = Mir::PlayerSubPart::Weapon;
    horse->Name = Mir::PlayerSubPart::Horse;
    actor->Name = Mir::PlayerSubPart::Actor;
    effect->Name = Mir::PlayerSubPart::Effect;
    shield->Name = Mir::PlayerSubPart::Shield;

    root->AddComponent<ActorView>();

    return root;
}

void SceneManager::ReleasePlayer(shared_ptr<SceneNode> ptr)
{
    ptr->GetComponent<ActorView>().lock()->As<ActorView>()->Clear();
    _playerPool.push_back(ptr);
}

void SceneManager::Clear()
{
    for (auto c : _root->GetChildren())
    {
        c.lock()->SetParent({});
    }
}
