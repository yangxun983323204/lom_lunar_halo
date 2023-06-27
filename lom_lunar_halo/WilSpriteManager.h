#pragma once
#include <wrl.h>
#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <functional>

using Microsoft::WRL::ComPtr;
using std::vector;
using std::function;

typedef DirectX::XMUINT2 WilSpriteKey;
typedef function<void(ComPtr<ID3D11ShaderResourceView>)> WilSpriteLoadCallback;

class WilSpriteManager
{
public:
	WilSpriteManager();
	~WilSpriteManager();
	void SetCapacity(uint32_t size);
	void LoadSprite(WilSpriteKey sk, WilSpriteLoadCallback callback=0);
	void Wait(vector<WilSpriteKey>& keys);
private:
	struct Impl;
	Impl* _impl;
};

