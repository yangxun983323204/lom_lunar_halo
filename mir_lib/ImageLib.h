#pragma once
#include <string>
#include <memory>
#include <vector>

using std::wstring;
using std::unique_ptr;
using std::vector;

class ImageLib
{
public:
	const static unsigned char KeyNone;// 透明
	const static unsigned char KeyConst;
	const static unsigned char KeyOverlay1;
	const static unsigned char KeyOverlay2;

	#pragma pack(1)
	struct ImageInfo
	{
		int16_t Width;
		int16_t Height;
		int16_t PivotX;
		int16_t PivotY;
		char HasShadow;
		int16_t ShadowPosX;
		int16_t ShadowPosY;
		int32_t ImgLength;
	};
	#pragma pack()

	ImageLib();
	~ImageLib();
	void Open(wstring wilPath);
	void Close();
	bool IsOpened();

	uint32_t GetCount();
	ImageInfo GetImageInfo(int idx);
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

private:
	struct Impl;
	unique_ptr<Impl> _impl;
};

