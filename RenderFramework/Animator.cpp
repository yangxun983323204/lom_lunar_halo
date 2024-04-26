#include "Animator.h"
#include "SpriteRendererComponent.h"

Animator::Animator():
    _anims{}, _currIdx{0}
{
    bEnableTick = true;
    _hashHelper = std::hash<string>();
}

void Animator::Clear()
{
    _currIdx = 0;
    _anims.clear();
}

void Animator::Add(string name, Animation& anim)
{
    auto idx = _hashHelper(name);
    _anims.insert({ idx, anim });
}

void Animator::Remove(string name)
{
    auto idx = _hashHelper(name);
    _anims.erase(idx);
}

void Animator::Remove(int hash)
{
    _anims.erase(hash);
}

Animation* Animator::Get(string name)
{
    auto idx = _hashHelper(name);
    return Get(idx);
}

Animation* Animator::Get(int hash)
{
    auto f = _anims.find(hash);
    if (f == _anims.end())
        return nullptr;
    else
        return &f->second;
}

Animation* Animator::GetCurrent()
{
    return Get(_currIdx);
}

int Animator::SetCurrent(string name)
{
    _currIdx = _hashHelper(name);
    return _currIdx;
}

void Animator::Tick(uint64_t totalMs, uint32_t deltaMs)
{
    auto anim = GetCurrent();
    if (anim)
    {
        anim->Update(totalMs);
        if (anim->FrameChanged()) {
            anim->ResetFlag();
            auto sp = anim->GetCurrentFrame();
            if (!sp.expired())
            {
                auto renderer = GetSceneNode()->GetComponent<SpriteRendererComponent>().lock()->As<SpriteRendererComponent>();
                if (renderer)
                {
                    renderer->Sprite = sp;
                }
            }
        }
    }
}

void Animator::FillTypeIds(std::vector<uint32_t>& typeIds)
{
    ISceneNodeComponent::FillTypeIds(typeIds);
    typeIds.push_back(Animator::TypeId);
}
