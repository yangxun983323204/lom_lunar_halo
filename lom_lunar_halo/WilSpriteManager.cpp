#include "pch.h"
#include <string>
#include <unordered_map>
#include "WilSpriteManager.h"
#include "../RenderFramework/LRU.hpp"
#include "../mir_lib/ImageLib.h"
#include "../DirectXTK-main/Inc/BufferHelpers.h"
#include "json.hpp"
#include <fstream>
#include <streambuf>
#include "StringCodec.hpp"

using json = nlohmann::json;
using std::unordered_map;
using std::wstring;
using std::string;

class WilSpriteManager::Impl
{
public:
	Impl(ID3D11Device* dev, string dir, WilSpriteManager* parent);
	~Impl();
	void SetCapacity(uint32_t sizeMb);
	void PreloadSprite(WilSpriteKey key);
	shared_ptr<SpriteResHandle> LoadSprite(WilSpriteKey key);
	void MakeSpace(uint32_t size);
	void NotifyFreeSpace(uint32_t size);
	inline uint32_t GetIdleSpace() { return _currentSize >= _capacitySize ? 0 : _capacitySize - _currentSize; }
	uint32_t GetFileId(string name);
private:
	shared_ptr<SpriteResHandle> Find(WilSpriteKey key);

	string _dir;
	WilSpriteManager* _parent;
	ID3D11Device* _dev;
	LRUCache<WilSpriteKey, shared_ptr<SpriteResHandle>> _lru;
	uint32_t _capacitySize;
	uint32_t _currentSize;
	unordered_map<uint32_t, string> _fileMap;
	unordered_map<uint32_t, ImageLib*> _imgLibs;
};

WilSpriteManager::Impl::Impl(ID3D11Device* dev, string dir, WilSpriteManager* parent) :
	_dir{dir}, _parent{ parent }, _dev{ dev },
	_lru{ INT_MAX }, _capacitySize{ 0 }, _currentSize{ 0 },
	_imgLibs{}
{
	struct _stat buffer;
	if (_wstat(L"./ResDef/wil.json", &buffer) == 0)
	{
		std::ifstream cfg(L"./ResDef/wil.json");
		std::string str = std::string((std::istreambuf_iterator<char>(cfg)),
			std::istreambuf_iterator<char>());
		cfg.close();
		auto jObj = json::parse(str);
		
		int i = 0;
		for (auto record : jObj) {
			string r = record;
			_fileMap[i] = r.substr(2);
			++i;
		}
	}
}
WilSpriteManager::Impl::~Impl()
{
	for (auto lib : _imgLibs)
	{
		lib.second->Close();
		delete lib.second;
	}
	_imgLibs.clear();
}
void WilSpriteManager::Impl::SetCapacity(uint32_t sizeMb)
{
	_capacitySize = sizeMb * 1024 * 1024;
}
void WilSpriteManager::Impl::PreloadSprite(WilSpriteKey key)
{
	// todo
}
shared_ptr<SpriteResHandle> WilSpriteManager::Impl::LoadSprite(WilSpriteKey key)
{
	auto fileId = key.x;
	auto imgId = key.y;
	unordered_map<uint32_t, string>::iterator fileRecord;
	shared_ptr<SpriteResHandle> handle = Find(key);
	if (handle)
		goto LABEL_RET;

	fileRecord = _fileMap.find(fileId);
	if (fileRecord == _fileMap.end()) {
		handle = {};
		goto LABEL_RET;
	}

	{
		ImageLib* imgLib;
		auto imgLibF = _imgLibs.find(fileRecord->first);
		if (imgLibF!= _imgLibs.end())
		{
			imgLib = imgLibF->second;
		}
		else {
			imgLib = new ImageLib();
			imgLib->Open(_W(_dir + fileRecord->second));
			_imgLibs[fileRecord->first] = imgLib;
		}

		if (!imgLib->IsOpened() || !imgLib->ImageValid(imgId)) {
			handle = {};
			goto LABEL_RET;
		}

		auto info = imgLib->GetImageInfo(imgId);
		if (info.Width<=0 || info.Height<=0) {
			handle = {};
			goto LABEL_RET;
		}
		auto needSize = info.Width * info.Height * 4;
		MakeSpace(needSize);
		if (needSize >= GetIdleSpace())
		{
			handle = {};
			goto LABEL_RET;
		}
		auto rgba32 = imgLib->GetImageRGBA32(imgId);
		handle = std::make_shared<SpriteResHandle>(_parent, rgba32.size());
		handle->GetSprite()->Rect = { 0,0,info.Width,info.Height };
		// offset是相对于图片左上角为原点，y轴正方向向下坐标系的偏移
		auto ox = info.OffsetX;
		auto oy = -(info.OffsetY + info.Height);
		auto sprite = handle->GetSprite();
		sprite->Pivot = { -(float)ox / info.Width, -(float)oy / info.Height };
		auto shType = info.GetShadowType();
		if (shType == Mir::ShadowType::Orth)
		{
			sprite->GenerateShadow = true;
			sprite->ShadowInfo.InitSizeAsOrth();
		}
		else if (shType == Mir::ShadowType::Proj)
		{
			sprite->GenerateShadow = true;
			sprite->ShadowInfo.InitSizeAsProj(info.Width, info.Height);
		}
		sprite->ShadowInfo.OffsetX = info.ShadowPosX;
		sprite->ShadowInfo.OffsetY = -info.ShadowPosY;
		D3D11_SUBRESOURCE_DATA subData{ 0 };
		subData.pSysMem = rgba32.data();
		subData.SysMemPitch = info.Width * 4;
		DX::ThrowIfFailed(DirectX::CreateTextureFromMemory(
			_dev,
			info.Width, info.Height,
			DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
			(const D3D11_SUBRESOURCE_DATA)subData,
			nullptr, sprite->TextureSRV.GetAddressOf()));
		_currentSize += needSize;
	}

LABEL_RET:
	_lru.set(key, handle);
	return handle;
}

void WilSpriteManager::Impl::MakeSpace(uint32_t size)
{
	if (GetIdleSpace() >= size)
		return;

	auto moreSpace = size * 20;
	while (_lru.getSize() > 0 && GetIdleSpace() < moreSpace) {
		_lru.removeLast();
	}
}

void WilSpriteManager::Impl::NotifyFreeSpace(uint32_t size)
{
	if (_currentSize <= size)
		_currentSize = 0;
	else
		_currentSize -= size;
}

uint32_t WilSpriteManager::Impl::GetFileId(string name)
{
	for (auto kv : _fileMap) {
		if (kv.second == name)
			return kv.first;
	}
	// 没有记录的文件，添加进来
	const uint32_t id = _fileMap.size();
	_fileMap.insert({ id, name });
	return id;
}

shared_ptr<SpriteResHandle> WilSpriteManager::Impl::Find(WilSpriteKey key)
{
	shared_ptr<SpriteResHandle> p = {};
	_lru.get(key, p);
	return p;
}

SpriteResHandle::SpriteResHandle(WilSpriteManager* mgr, uint32_t size) :
	_mgr{ mgr }, _size{ size }, _sprite{}
{
	_sprite = std::make_shared<Sprite>();
}

SpriteResHandle::~SpriteResHandle()
{
	_mgr->_impl->NotifyFreeSpace(_size);
}

WilSpriteManager::WilSpriteManager(ID3D11Device* dev, string dir)
{
	_impl = new WilSpriteManager::Impl(dev, dir, this);
}

WilSpriteManager::~WilSpriteManager()
{
	delete _impl;
}

void WilSpriteManager::SetCapacity(uint32_t sizeMb)
{
	_impl->SetCapacity(sizeMb);
}

void WilSpriteManager::PreloadSprite(WilSpriteKey key)
{
	_impl->PreloadSprite(key);
}

shared_ptr<SpriteResHandle> WilSpriteManager::LoadSprite(WilSpriteKey key)
{
	return _impl->LoadSprite(key);
}

uint32_t WilSpriteManager::GetFileId(string name)
{
	return _impl->GetFileId(name);
}
