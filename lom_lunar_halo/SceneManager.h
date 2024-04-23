#pragma once
#include "../RenderFramework/RenderFramework.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <d3d11.h>
#include <DirectXMath.h>
#include "DeviceResources.h"

using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using DirectX::XMFLOAT4;

typedef shared_ptr<SceneNode> SceneNodeRef;

class SceneManager
{
public:
	SceneManager();

	shared_ptr<SceneNode> CreateNode();
	shared_ptr<SceneNode> CreateCameraNode();

	// 为mir对象提供支持
	shared_ptr<SceneNode> SpawnStaticSprite();
	shared_ptr<SceneNode> SpawnAnimSprite();
	shared_ptr<SceneNode> SpawnMonster();
	shared_ptr<SceneNode> SpawnNpc();
	shared_ptr<SceneNode> SpawnPlayer();

	void ReleaseByInnerTag(weak_ptr<SceneNode> ptr);
	
	void ReleaseStaticSprite(weak_ptr<SceneNode> ptr);
	void ReleaseAnimSprite(weak_ptr<SceneNode> ptr);
	void ReleaseMonster(weak_ptr<SceneNode> ptr);
	void ReleaseNpc(weak_ptr<SceneNode> ptr);
	void ReleasePlayer(weak_ptr<SceneNode> ptr);

	void Clear();

	inline function<shared_ptr<SceneNode>()> GetSpawnMonsterFunctor() { return _spawnMonsterFunc; }
	inline function<shared_ptr<SceneNode>()> GetSpawnPlayerFunctor() { return _spawnPlayerFunc; }
	inline function<shared_ptr<SceneNode>()> GetSpawnNpcFunctor() { return _spawnNpcFunc; }
private:
	shared_ptr<SceneNode> CreateRawSpriteNode();

	shared_ptr<SceneNode> _root;
	vector<SceneNodeRef> _staticSpritePool;
	vector<SceneNodeRef> _animSpritePool;
	vector<SceneNodeRef> _monsterPool;
	vector<SceneNodeRef> _npcPool;
	vector<SceneNodeRef> _playerPool;

	function<shared_ptr<SceneNode>()> _spawnMonsterFunc;
	function<shared_ptr<SceneNode>()> _spawnPlayerFunc;
	function<shared_ptr<SceneNode>()> _spawnNpcFunc;

	const static string TAG_SCENE_NODE_STATIC_SP;
	const static string TAG_SCENE_NODE_ANIM_SP;
	const static string TAG_SCENE_NODE_MONSTER;
	const static string TAG_SCENE_NODE_NPC;
	const static string TAG_SCENE_NODE_PLAYER;
};

