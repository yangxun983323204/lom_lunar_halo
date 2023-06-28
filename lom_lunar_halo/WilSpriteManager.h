#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <functional>
#include <string>

using std::shared_ptr;
using Microsoft::WRL::ComPtr;
using std::vector;
using std::function;
using std::wstring;

typedef DirectX::XMUINT2 WilSpriteKey;
typedef function<void(ComPtr<ID3D11ShaderResourceView>)> WilSpriteLoadCallback;

namespace std {
	template<>
	class hash<WilSpriteKey> 
	{
	public:
		size_t operator()(const WilSpriteKey& sk) const
		{
			return hash<uint32_t>()(sk.x) ^ hash<uint32_t>()(sk.y);
		}
	};

	template<>
	struct equal_to<WilSpriteKey> 
	{
	public:
		bool operator()(const WilSpriteKey& k1, const WilSpriteKey& k2) const
		{
			return k1.x == k2.x && k1.y == k2.y;
		}

	};
};

class WilSpriteManager
{
	friend class SpriteResHandle;
public:
	WilSpriteManager(ID3D11Device* dev, wstring dir);
	~WilSpriteManager();
	void SetCapacity(uint32_t sizeMb);
	// 发起加载但不等待完成
	void PreloadSprite(WilSpriteKey key);
	// 发起加载并等待完成
	shared_ptr<SpriteResHandle> LoadSprite(WilSpriteKey key);
private:
	struct Impl;
	Impl* _impl;
};

class SpriteResHandle
{
	friend class WilSpriteManager::Impl;
public:
	SpriteResHandle(WilSpriteManager* mgr, uint32_t size);
	~SpriteResHandle();
	inline uint32_t GetSize() { return _size; }
	inline ID3D11ShaderResourceView* GetSRV() { return _srv.Get(); }

private:
	WilSpriteManager* _mgr;
	WilSpriteKey _key;
	ComPtr<ID3D11ShaderResourceView> _srv;
	uint32_t _size;
};
