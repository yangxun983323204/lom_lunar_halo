#include "AnimatorSystem.h"
#include "Animation.h"
#include "Animator.h"
#include "SpriteRendererComponent.h"

void AnimatorSystem::Update(uint64_t totalMs, uint32_t deltaMs)
{
	auto animators = ISystem::GetComponentsByType(Animator::TypeId);
	for (auto i : animators) {
		if (!i->Enable)
			continue;

		auto animator = i->As<Animator>();
		auto anim = animator->GetCurrent();
		if (anim)
		{
			anim->Update(totalMs);
			if (anim->FrameChanged()) {
				anim->ResetFlag();
				auto sp = anim->GetCurrentFrame();
				if (!sp.expired())
				{
					auto renderer = i->GetSceneNode()->GetComponent<SpriteRendererComponent>().lock()->As<SpriteRendererComponent>();
					if (renderer)
					{
						renderer->Sprite = sp;
					}
				}
			}
		}
	}
}
