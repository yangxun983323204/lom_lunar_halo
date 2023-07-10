#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include "../RenderFramework/SceneNode.h"
#include "ActorData.h"
#include "AnimDatabase.h"
#include "WilSpriteManager.h"

using std::unordered_map;
using std::vector;
using std::weak_ptr;
using std::string;

namespace Mir {
	enum class ActorRaceType {
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
		const string Hair = "hair";
		const string Effect = "effect";
		const string Shield = "shield";
	}
}

class ActorView :public ISceneNodeComponent
{
public:
	SCENE_NODE_COMPONENT('atv');

	void PlayAnim(string name);
	vector<string> GetSubPartNames();
	void SetSubPartRenderOrder(vector<string> names);
	weak_ptr<SceneNode> GetSubPart(string name);
	void Clear();
	void InitAs(HeroData& hero, AnimDatabase& animDB, WilSpriteManager* resMgr);
	void SetDirAndMotion(int dir, int motion);

	int Id;
	int Type;
protected:
};

