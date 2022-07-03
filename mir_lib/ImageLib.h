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
	const static unsigned char KeyNone;// ͸��
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
	/// ��ȡδ��ѹ��ԭʼ����
	/// </summary>
	vector<uint16_t> GetImageRaw(int idx);
	/// <summary>
	/// ��ȡ��ѹΪRGB565������
	/// </summary>
	vector<uint16_t> GetImageRGB565(int idx);
	/// <summary>
	/// ��ȡ��ѹΪRGBA3������
	/// </summary>
	vector<uint32_t> GetImageRGBA32(int idx);

private:
	struct Impl;
	unique_ptr<Impl> _impl;
};

