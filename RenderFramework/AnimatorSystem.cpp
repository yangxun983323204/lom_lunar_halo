#include "AnimatorSystem.h"
#include "Animation.h"
#include "Animator.h"
#include "SpriteRendererComponent.h"

void AnimatorSystem::Update(uint64_t totalMs, uint32_t deltaMs)
{
	auto updater = ISystem::GetComponentsByType(ISceneNodeComponent::TypeId);
	for (auto i : updater) {
		if (!i->GetSceneNode()->IsActive())
			continue;

		if (!i->Enable)
			continue;

		if(i->bEnableTick)
			i->Tick(totalMs, deltaMs);
	}
}
