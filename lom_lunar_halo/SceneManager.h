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

typedef shared_ptr<SceneNode> SceneNodeRef;

namespace Mir {
	enum class ActorType {
		Monster = 0,
		Player,
		Npc
	};

	namespace MonsterSubPart {
		const string Actor = "actor";
		const string Shadow = "shadow";
	}

	namespace PlayerSubPart {
		const string Weapon = "weapon";
		const string Horse = "horse";
		const string Actor = "actor";
		const string Effect = "effect";
		const string Shield = "shield";
	}
}

class SceneManager
{
public:
	SceneManager();

	shared_ptr<SceneNode> CreateNode();
	shared_ptr<SceneNode> CreateCameraNode();
	// 为mir对象提供支持
	shared_ptr<SceneNode> SpawnStaticSprite();
	void ReleaseStaticSprite(shared_ptr<SceneNode> ptr);

	shared_ptr<SceneNode> SpawnAnimSprite();
	void ReleaseAnimSprite(shared_ptr<SceneNode> ptr);

	shared_ptr<SceneNode> SpawnMonster();
	void ReleaseMonster(shared_ptr<SceneNode> ptr);

	shared_ptr<SceneNode> SpawnNpc();
	void ReleaseNpc(shared_ptr<SceneNode> ptr);

	shared_ptr<SceneNode> SpawnPlayer();
	void ReleasePlayer(shared_ptr<SceneNode> ptr);

	void Clear();
private:
	shared_ptr<SceneNode> CreateRawSpriteNode();

	shared_ptr<SceneNode> _root;
	vector<SceneNodeRef> _staticSpritePool;
	vector<SceneNodeRef> _animSpritePool;
	vector<SceneNodeRef> _monsterPool;
	vector<SceneNodeRef> _npcPool;
	vector<SceneNodeRef> _playerPool;
};

