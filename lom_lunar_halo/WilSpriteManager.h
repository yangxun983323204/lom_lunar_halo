#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <d3d11.h>
#include <DirectXMath.h>
#include <functional>
#include <string>
#include "../RenderFramework/Sprite.hpp"
#include "../RenderFramework/HashSupport.h"

using std::shared_ptr;
using Microsoft::WRL::ComPtr;
using std::vector;
using std::unordered_map;
using std::function;
using std::wstring;
using std::string;

typedef DirectX::XMUINT2 WilSpriteKey;
typedef function<void(ComPtr<ID3D11ShaderResourceView>)> WilSpriteLoadCallback;

class WilSpriteManager
{
	friend class SpriteResHandle;
public:
	WilSpriteManager(ID3D11Device* dev, string dir);
	~WilSpriteManager();
	void SetCapacity(uint32_t sizeMb);
	// 发起加载但不等待完成
	void PreloadSprite(WilSpriteKey key);
	// 发起加载并等待完成
	shared_ptr<SpriteResHandle> LoadSprite(WilSpriteKey key);
	uint32_t GetFileId(string name);
private:
	class Impl;
	Impl* _impl;
};

class SpriteResHandle
{
	friend class WilSpriteManager::Impl;
public:
	SpriteResHandle(WilSpriteManager* mgr, uint32_t size);
	~SpriteResHandle();
	inline uint32_t GetSize() { return _size; }
	inline shared_ptr<Sprite> GetSprite() { return _sprite; }
private:
	WilSpriteManager* _mgr;
	WilSpriteKey _key;
	uint32_t _size;
	shared_ptr<Sprite> _sprite;
};
