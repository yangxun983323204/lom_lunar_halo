#include "pch.h"
#include "SceneManager.h"
#include "SpriteHandleHolder.hpp"
#include "ActorView.h"

const string SceneManager::TAG_SCENE_NODE_STATIC_SP = "__TAG_SCENE_NODE_STATIC_SP";
const string SceneManager::TAG_SCENE_NODE_ANIM_SP = "__TAG_SCENE_NODE_STATIC_SP";
const string SceneManager::TAG_SCENE_NODE_MONSTER = "__TAG_SCENE_NODE_STATIC_SP";
const string SceneManager::TAG_SCENE_NODE_NPC = "__TAG_SCENE_NODE_STATIC_SP";
const string SceneManager::TAG_SCENE_NODE_PLAYER = "__TAG_SCENE_NODE_STATIC_SP";

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
    node->Tags.push_back(TAG_SCENE_NODE_STATIC_SP);
    return node;
}

void SceneManager::ReleaseByInnerTag(weak_ptr<SceneNode> ptr)
{
    if (ptr.lock()->HasTag(TAG_SCENE_NODE_STATIC_SP))
        ReleaseStaticSprite(ptr);
    else if (ptr.lock()->HasTag(TAG_SCENE_NODE_ANIM_SP))
        ReleaseAnimSprite(ptr);
    else if (ptr.lock()->HasTag(TAG_SCENE_NODE_MONSTER))
        ReleaseMonster(ptr);
    else if (ptr.lock()->HasTag(TAG_SCENE_NODE_NPC))
        ReleaseNpc(ptr);
    else if (ptr.lock()->HasTag(TAG_SCENE_NODE_PLAYER))
        ReleasePlayer(ptr);
}

void SceneManager::ReleaseStaticSprite(weak_ptr<SceneNode> ptr)
{
    ptr.lock()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();
    ptr.lock()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>()->Sprite.reset();
    ptr.lock()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>()->Enable = false;
    _staticSpritePool.push_back(ptr.lock());
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
    node->Tags.push_back(TAG_SCENE_NODE_ANIM_SP);
    return node;
}

void SceneManager::ReleaseAnimSprite(weak_ptr<SceneNode> ptr)
{
    ptr.lock()->GetComponent<Animator>().lock()->As<Animator>()->Clear();
    ptr.lock()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();
    ptr.lock()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>()->Sprite.reset();
    ptr.lock()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>()->Enable = false;
    _animSpritePool.push_back(ptr.lock());
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
    root->Tags.push_back(TAG_SCENE_NODE_MONSTER);
    return root;
}

void SceneManager::ReleaseMonster(weak_ptr<SceneNode> ptr)
{
    ptr.lock()->GetComponent<ActorView>().lock()->As<ActorView>()->Clear();
    _monsterPool.push_back(ptr.lock());
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
    root->Tags.push_back(TAG_SCENE_NODE_NPC);
    return root;
}

void SceneManager::ReleaseNpc(weak_ptr<SceneNode> ptr)
{
    ptr.lock()->GetComponent<ActorView>().lock()->As<ActorView>()->Clear();
    _npcPool.push_back(ptr.lock());
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
    auto hair = SpawnAnimSprite();
    auto effect = SpawnAnimSprite();
    auto shield = SpawnAnimSprite();

    weapon->SetParent(root); weapon->ResetTRS();
    horse->SetParent(root); horse->ResetTRS();
    actor->SetParent(root); actor->ResetTRS();
    hair->SetParent(root); hair->ResetTRS();
    effect->SetParent(root); effect->ResetTRS();
    shield->SetParent(root); shield->ResetTRS();

    weapon->Name = Mir::PlayerSubPart::Weapon;
    horse->Name = Mir::PlayerSubPart::Horse;
    actor->Name = Mir::PlayerSubPart::Actor;
    hair->Name = Mir::PlayerSubPart::Hair;
    effect->Name = Mir::PlayerSubPart::Effect;
    shield->Name = Mir::PlayerSubPart::Shield;

    root->AddComponent<ActorView>();
    root->Tags.push_back(TAG_SCENE_NODE_PLAYER);

    return root;
}

void SceneManager::ReleasePlayer(weak_ptr<SceneNode> ptr)
{
    ptr.lock()->GetComponent<ActorView>().lock()->As<ActorView>()->Clear();
    _playerPool.push_back(ptr.lock());
}

void SceneManager::Clear()
{
    for (auto c : _root->GetChildren())
    {
        c.lock()->SetParent({});
    }
}
