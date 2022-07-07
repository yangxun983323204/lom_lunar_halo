#include "pch.h"
#include "ImageLib.h"
#include <cassert>
#include <iostream>

const unsigned char ImageLib::KeyNone = 0xc0;// 透明
const unsigned char ImageLib::KeyConst = 0xc1;
const unsigned char ImageLib::KeyOverlay1 = 0xc2;
const unsigned char ImageLib::KeyOverlay2 = 0xc3;

struct ImageLib::Impl
{
	Impl();
	void Open(wstring wixPath,wstring wilPath);
	void Close();
	bool IsOpened() { return _isOpened; }

	uint32_t GetCount() 
	{ 
		if (!_isOpened)
			return -1;

		return _infos.size(); 
	}

	ImageInfo GetImageInfo(int idx) 
	{
		assert(idx>=0 && idx<GetCount());
		return _infos[idx];
	}
	/// <summary>
	/// 获取未解压的原始数据
	/// </summary>
	vector<uint16_t> GetImageRaw(int idx);
	/// <summary>
	/// 获取解压为RGB565的数据
	/// </summary>
	vector<uint16_t> GetImageRGB565(int idx);
	/// <summary>
	/// 获取解压为RGBA3的数据
	/// </summary>
	vector<uint32_t> GetImageRGBA32(int idx);


	int16_t _version;
	bool _isOpened;
	vector<int32_t> _blockAddress;
	vector<ImageInfo> _infos;
	unique_ptr<FILE, void(*)(FILE*)> _wil;


	inline static int WixOffset(int version)
	{
		switch (version) {
		case 17:
			return 24;
		case 5000:
			return 28;
		case 6000:
			return 32;
		default:
			throw "offset == -1";
		}
	}
	inline static int WilOffset(int version)
	{
		switch (version) {
		case 17:
			return 17;
		case 5000:
			return 21;
		case 6000:
			return 21;
		default:
			throw "offset == -1";
		}
	}
	inline static uint32_t RGBA32(uint16_t rgb565)
	{
		unsigned char r = (rgb565 & 0xF800) >> 8;
		unsigned char g = (rgb565 & 0x07E0) >> 3;
		unsigned char b = (rgb565 & 0x001F) << 3;
		unsigned char a = 0xFF;
		return a << 24 | b << 16 | g << 8 | r;
	}

#pragma pack(1)
	struct WixHeader3
	{
		char Title[20];
		int32_t ImgCount;
		int32_t* Data;
	};
	/* 可以不使用
	struct WilHeader3
	{
		int16_t HasCompress;
		char Title[20];
		int16_t Version;
		int32_t ImgCount;
	};
	*/
#pragma pack()
};

ImageLib::Impl::Impl():
	_isOpened{false},
	_version{0},
	_blockAddress{},
	_infos{},
	_wil{ nullptr, [](FILE* file) {
		fclose(file);
		} }
{

}

void ImageLib::Impl::Open(wstring wixPath, wstring wilPath)
{
	if (_isOpened)
		return;
	// 从wil文件获取版本号
	FILE* wil = nullptr;
	_wfopen_s(&wil, wilPath.c_str(), L"rb");
	//assert(fpeekVer != nullptr);
	if (wil == nullptr)
		return;

	fseek(wil, 22, SEEK_SET);
	fread(reinterpret_cast<void*>(&_version), 2, 1, wil);
	fclose(wil);
	FILE* wix = nullptr;
	_wfopen_s(&wix, wixPath.c_str(), L"rb");
	if (wix == nullptr)
		return;

	auto wixHeader = new WixHeader3();
	fread(reinterpret_cast<void*>(wixHeader), sizeof(WixHeader3) - 4, 1, wix);
	wixHeader->Data = new int32_t[wixHeader->ImgCount];
	fseek(wix, WixOffset(_version), SEEK_SET);
	fread(reinterpret_cast<void*>(wixHeader->Data), sizeof(int32_t), wixHeader->ImgCount, wix);
	fclose(wix);
	wil = nullptr;
	_wfopen_s(&wil, wilPath.c_str(), L"rb");
	_wil.reset(wil);
	// 读取ImageInfo
	_blockAddress.clear();
	_blockAddress.reserve(wixHeader->ImgCount);
	_infos.clear();
	_infos.resize(wixHeader->ImgCount);
	for (size_t i = 0; i < wixHeader->ImgCount; i++)
	{
		auto address = wixHeader->Data[i];
		_blockAddress.push_back(address);
		fseek(_wil.get(), address, SEEK_SET);
		fread(reinterpret_cast<void*>(&_infos[i]), sizeof(ImageInfo), 1, _wil.get());
	}
	//
	_isOpened = true;
}

void ImageLib::Impl::Close()
{
	if (_isOpened)
	{
		_wil.release();
		_isOpened = false;
	}
}

vector<uint16_t> ImageLib::Impl::GetImageRaw(int idx)
{
	auto address = _blockAddress[idx];
	if (address == 0)
		return {};

	auto info = _infos[idx];
	fseek(_wil.get(), address, SEEK_SET);
	fseek(_wil.get(), WilOffset(_version), SEEK_CUR);
	vector<uint16_t> data(info.ImgLength, 0);
	fread(reinterpret_cast<void*>(data.data()), 2, info.ImgLength, _wil.get());
	return data;
}

vector<uint16_t> ImageLib::Impl::GetImageRGB565(int idx)
{
	auto address = _blockAddress[idx];
	auto info = _infos[idx];
	vector<uint16_t> rawbuffer(GetImageRaw(idx));
	if (rawbuffer.size() <= 0)
		return {};

	auto w = info.Width;
	auto h = info.Height;
	size_t rawBeginAt = 0;
	size_t rawEndAt = 0;
	size_t rawNowAt = 0;
	size_t dstNowRowAt = 0;

	vector<uint16_t> data(w * h, 0);
	auto pData = data.data();

	for (size_t row = 0; row < h; ++row)
	{
		rawEndAt += rawbuffer[rawBeginAt++];
		rawNowAt = rawBeginAt;
		dstNowRowAt = 0;
		while (rawNowAt < rawEndAt)
		{
			uint16_t type = rawbuffer[rawNowAt++];
			uint16_t cntCpy = rawbuffer[rawNowAt++];
			switch (type)
			{
			case KeyNone:
				break;
			case KeyConst:
				for (size_t t = 0; t < cntCpy; t++)
				{
					pData[row * w + dstNowRowAt + t] = rawbuffer[rawNowAt + t];
				}
				rawNowAt += cntCpy;
				break;
			case KeyOverlay1:
				//chooseColor = chooseColor1;
			case KeyOverlay2:
				//chooseColor = chooseColor2;
				for (size_t t = 0; t < cntCpy; t++)
				{
					// 考虑不在这里做颜色变更，只做个标记
					pData[row * w + dstNowRowAt + t] = rawbuffer[rawNowAt + t];
				}
				rawNowAt += cntCpy;
				break;
			default:
				break;
			}
			dstNowRowAt += cntCpy;
		}
		rawEndAt++;
		rawBeginAt = rawEndAt;
	}

	return data;
}

vector<uint32_t> ImageLib::Impl::GetImageRGBA32(int idx)
{
	auto address = _blockAddress[idx];
	auto info = _infos[idx];
	vector<uint16_t> rawbuffer(GetImageRaw(idx));
	if (rawbuffer.size() <= 0)
		return {};

	auto w = info.Width;
	auto h = info.Height;
	size_t rawBeginAt = 0;
	size_t rawEndAt = 0;
	size_t rawNowAt = 0;
	size_t dstNowRowAt = 0;

	vector<uint32_t> data(w * h, 0);
	auto pData = data.data();

	for (size_t row = 0; row < h; ++row)
	{
		rawEndAt += rawbuffer[rawBeginAt++];
		rawNowAt = rawBeginAt;
		dstNowRowAt = 0;
		while (rawNowAt < rawEndAt)
		{
			uint16_t type = rawbuffer[rawNowAt++];
			uint16_t cntCpy = rawbuffer[rawNowAt++];
			switch (type)
			{
			case KeyNone:
				break;
			case KeyConst:
				for (size_t t = 0; t < cntCpy; t++)
				{
					pData[row * w + dstNowRowAt + t] = RGBA32(rawbuffer[rawNowAt + t]);
				}
				rawNowAt += cntCpy;
				break;
			case KeyOverlay1:
				//chooseColor = chooseColor1;
			case KeyOverlay2:
				//chooseColor = chooseColor2;
				for (size_t t = 0; t < cntCpy; t++)
				{
					// 考虑不在这里做颜色变更，只做个标记
					pData[row * w + dstNowRowAt + t] = RGBA32(rawbuffer[rawNowAt + t]);
				}
				rawNowAt += cntCpy;
				break;
			default:
				break;
			}
			dstNowRowAt += cntCpy;
		}
		rawEndAt++;
		rawBeginAt = rawEndAt;
	}

	return data;
}

#pragma region ImageLib

ImageLib::ImageLib() :
	_impl{std::make_unique<ImageLib::Impl>()}
{
}

ImageLib::~ImageLib() = default;

void ImageLib::Open(wstring wilPath)
{
	auto wixPath = wilPath.substr(0, wilPath.length() - 4) + L".wix";
	_impl->Open(wixPath, wilPath);
}

void ImageLib::Close()
{
	_impl->Close();
}

bool ImageLib::IsOpened()
{
	return _impl->IsOpened();
}

uint32_t ImageLib::GetCount()
{
	return _impl->GetCount();
}

ImageLib::ImageInfo ImageLib::GetImageInfo(int idx)
{
	return _impl->GetImageInfo(idx);
}

vector<uint16_t> ImageLib::GetImageRaw(int idx)
{
	return _impl->GetImageRaw(idx);
}

vector<uint16_t> ImageLib::GetImageRGB565(int idx)
{
	return _impl->GetImageRGB565(idx);
}

vector<uint32_t> ImageLib::GetImageRGBA32(int idx)
{
	return _impl->GetImageRGBA32(idx);
}

#pragma endregion