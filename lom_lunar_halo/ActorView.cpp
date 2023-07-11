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
	// 为人物的每个【部件】的每个【方向】的【动作】生成动画

	auto actorType = hero.Gender;
	string subName[] = {
		Mir::PlayerSubPart::Weapon,
		Mir::PlayerSubPart::Horse,
		Mir::PlayerSubPart::Actor,
		Mir::PlayerSubPart::Hair,
		Mir::PlayerSubPart::Effect,
		Mir::PlayerSubPart::Shield,
	};

	auto weaponFileIdx = Mir::GetWeaponImgFileIdx(actorType, hero.Weapon);
	auto horseFileIdx = Mir::GetHorseImgFileIdx(actorType);
	auto actorFileIdx = Mir::GetActorImgFileIdx(actorType, hero.Dress);
	auto hairFileIdx = Mir::GetHairImgFileIdx(actorType, hero.Hair);
	auto fxFileIdx = Mir::GetEffectImgFileIdx();
	auto shieldFileIdx = Mir::GetShieldImgFileIdx();
	int fileIdx[] = { weaponFileIdx,horseFileIdx,actorFileIdx,hairFileIdx,fxFileIdx,shieldFileIdx };
	DirectX::XMFLOAT4 debugColor[] = { 
		{1,0,0,1},{0,1,0,1},{0,0,1,1},
		{1,1,0,1},{1,0,1,1},{0,1,1,1}, };

	for (int dir = 0; dir < (int)Mir::Direction::MAX; dir++)
	{
		Mir::Direction direction = (Mir::Direction)dir;
		for (int motion = 0; motion < (int)Mir::PlayerMotion::MAX; motion++)
		{
			auto motionAnimData = animDB.GetData((uint16_t)Mir::ActorRaceType::Player, motion);
			auto actorImgIdx = Mir::GetActorFirstImgIdx(motionAnimData, actorType, hero.Dress, direction);
			auto weaponImgOffset = Mir::GetWeaponFrame(hero.Weapon, hero.Dress);
			auto horseImgOffset = Mir::GetHorseFrame(hero.Horse, hero.Dress);
			auto hairImgOffset = Mir::GetHairFrame(hero.Hair, hero.Dress);
			auto fxImgOffset = Mir::GetEffectFrame(hero.Effect, hero.Dress);
			auto shieldImgOffset = Mir::GetShieldFrame(hero.Shield, hero.Dress);

			int idx[] = { weaponImgOffset,horseImgOffset,0,hairImgOffset,fxImgOffset,shieldImgOffset };
			for (int sub = 0; sub < 6; sub++)
			{
				auto fIdx = fileIdx[sub];
				string name = subName[sub];
				auto node = GetSubPart(name);
				auto imgIdx = idx[sub] += actorImgIdx;
				if (node.expired() || fIdx<0 || imgIdx < 0)
					continue;

				auto animator = node.lock()->GetComponent<Animator>().lock()->As<Animator>();
				auto sr = node.lock()->GetComponent<SpriteRenderer>().lock()->As<SpriteRenderer>();
				sr->Enable = true;
				sr->SortLayer = Layer;
				if (true) {
					sr->__debug = true;
					sr->__debugColor = debugColor[sub];
				}

				auto resHolder = node.lock()->GetComponent<SpriteHandleHolder>().lock()->As<SpriteHandleHolder>();

				Animation anim{};
				anim.SetSpan(motionAnimData.AnimSpanMs);
				for (int frame = 0; frame < motionAnimData.FrameCount; frame++)
				{
					auto sp = resMgr->LoadSprite({ (uint32_t)fIdx, (uint32_t)(imgIdx + frame) });
					if (sp)
						anim.AddFrame(sp->GetSprite());
					else
						anim.AddFrame({});
					resHolder->Add(sp);
				}
				animator->Add(std::to_string(dir)+"," + std::to_string(motion), anim);
			}
		}
	}
}

void ActorView::SetDirAndMotion(int dir, int motion)
{
	PlayAnim(std::to_string(dir) + "," + std::to_string(motion));
}
