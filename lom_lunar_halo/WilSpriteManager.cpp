#include "pch.h"
#include "WilSpriteManager.h"

struct WilSpriteManager::Impl
{

};

WilSpriteManager::WilSpriteManager()
{
	_impl = new WilSpriteManager::Impl();
}

WilSpriteManager::~WilSpriteManager()
{
	delete _impl;
}

void WilSpriteManager::SetCapacity(uint32_t size)
{
}

void WilSpriteManager::LoadSprite(WilSpriteKey sk, WilSpriteLoadCallback callback)
{
}

void WilSpriteManager::Wait(vector<WilSpriteKey>& keys)
{
}
