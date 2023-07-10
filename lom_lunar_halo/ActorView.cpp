#include "pch.h"
#include "ActorView.h"
#include "../RenderFramework/Animator.h"
#include "../RenderFramework/SpriteRenderer.h"
#include "SpriteHandleHolder.hpp"
#include "../mir_lib/MirConst.h"

void ActorView::PlayAnim(string name)
{
	for (auto child : GetSceneNode()->GetChildren())
	{
		auto wptr = child.lock()->GetComponent<Animator>();
		if (wptr.expired())
			continue;

		wptr.lock()->As<Animator>()->SetCurrent(name);
	}
}

vector<string> ActorView::GetSubPartNames()
{
	vector<string> names{};
	for (auto child : GetSceneNode()->GetChildren())
	{
		names.push_back(child.lock()->Name);
	}
	return names;
}

void ActorView::SetSubPartRenderOrder(vector<string> names)
{
	for (int i = 0; i < names.size(); i++)
	{
		auto node = GetSceneNode()->FindChild(names[i]);
		if (node)
		{
			auto wptr = node->GetComponent<SpriteRenderer>();
			if (!wptr.expired())
			{
				wptr.lock()->As<SpriteRenderer>()->Depth = i;
			}
		}
	}
}

weak_ptr<SceneNode> ActorView::GetSubPart(string name)
{
	for (auto child : GetSceneNode()->GetChildren())
	{
		if (child.lock()->Name == name)
		{
			return child;
		}
	}

	return {};
}

void ActorView::Clear()
{
	for (auto child : GetSceneNode()->GetChildren())
	{
		auto wptr = child.lock()->GetComponent<Animator>();
		if (wptr.expired())
			continue;

		wptr.lock()->As<Animator>()->Clear();
		child.lock()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>()->Clear();
		child.lock()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>()->Enable = false;
	}
}

void ActorView::InitAs(HeroData& hero, AnimDatabase& animDB, WilSpriteManager* resMgr)
{
	// 为人物的每个【部件】的每个【动作】生成动画
	auto actorType = hero.Gender;
	string subName[] = {
		Mir::PlayerSubPart::Weapon,
		Mir::PlayerSubPart::Horse,
		Mir::PlayerSubPart::Actor,
		Mir::PlayerSubPart::Effect,
		Mir::PlayerSubPart::Shield,
	};

	auto fileIdx = Mir::GetActorImgFileIdx(actorType, hero.Dress);
	for (int i = 0; i < (int)Mir::PlayerMotion::MAX; i++)
	{
		auto motionAnimData = animDB.GetData((uint16_t)Mir::ActorRaceType::Player, i);
		auto actorImgIdx = Mir::GetActorFirstImgIdx(motionAnimData, actorType, hero.Dress, hero.Dir);
		auto weaponImgIdx = actorImgIdx + Mir::GetWeaponFrame(hero.Weapon, hero.Dress);
		auto horseImgIdx = actorImgIdx + Mir::GetHorseFrame(hero.Horse, hero.Dress);
		auto fxImgIdx = actorImgIdx + Mir::GetEffectFrame(hero.Horse, hero.Dress);
		auto shieldImgIdx = actorImgIdx + Mir::GetShieldFrame(hero.Horse, hero.Dress);

		int idx[] = { weaponImgIdx,horseImgIdx,actorImgIdx,fxImgIdx,shieldImgIdx };
		for (int i = 0; i < 5; i++)
		{
			string name = subName[i];
			auto node = GetSubPart(name);
			if (node.expired())
				continue;

			auto imgIdx = idx[i];
			auto animator = node.lock()->GetComponent<Animator>().lock()->As<Animator>();
			auto resHolder = node.lock()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>();

			Animation anim{};
			anim.SetSpan(motionAnimData.AnimSpanMs);
			for (int frame = 0; frame < motionAnimData.FrameCount; frame++)
			{
				auto sp = resMgr->LoadSprite({ (uint32_t)fileIdx, (uint32_t)(imgIdx+ frame) });
				anim.AddFrame(sp->GetSprite());
				resHolder->Add(sp);
			}
			animator->Add(std::to_string(i), anim);
		}
	}
}