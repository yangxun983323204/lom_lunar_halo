#include "Animator.h"

Animator::Animator():
    _anims{}, _currIdx{0}
{
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
