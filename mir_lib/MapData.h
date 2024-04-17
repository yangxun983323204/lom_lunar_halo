#pragma once
#include <stdint.h>
#include <memory>
#include <string>

using std::unique_ptr;
using std::wstring;

// 地图图集文件变换
inline void ExFileIdx(int& n)
{
	// 地图数据以15个单位设置。(来自mir3g的方法)
	n -= n / 15;
	return;

	// 下面的旧代码，不用也可以

	int m = n;
	//__asm
	//{
	//	pusha

	//	xor     ecx, ecx
	//	mov		ebx, 0eh
	//	mov     ecx, m; cl = nFileIdx
	//	; ..........................
	//	mov     eax, 77777777h
	//	imul    ecx
	//	sub     edx, ecx
	//	sar     edx, 3
	//	mov     eax, edx
	//	shr     eax, 1Fh; ·ûºÅÎ»
	//	add     edx, eax
	//	add     ecx, edx
	//	mov     m, ecx

	//	popa
	//}
#pragma region 64位c++改写
	uint32_t b = 14;
	int32_t c = m;
	int32_t a = 0x77777777;
	int64_t edx_eax = 0;
	edx_eax = (int64_t)a * c;
	a = edx_eax & 0x00000000ffffffff;
	int32_t d = (edx_eax & 0xffffffff00000000) >> 32;
	d -= c;
	d >>= 3;
	uint32_t a1 = d;
	a1 >>= 0x1F;
	d += a1;
	c += d;
	m = c;
#pragma endregion
	n = m;
}

inline bool IsMapTileFile(int fileIdx) 
{
	if (fileIdx % 14 <=2 && fileIdx<=69)
	{
		return true;
	}
	
	return false;
}

inline bool IsMapObjectFile(int fileIdx)
{
	if (fileIdx % 14 > 2 && fileIdx <= 69)
	{
		return true;
	}

	return false;
}

class MapData
{
public:

//=============================================
// Head
// TileInfo[w*h/4]   一个tile包含4个cell
// CellInfo[w*h]
//=============================================
	#pragma pack(1)
	struct MapLight
	{
		int32_t IsLight;
		char LightSizeType;
		char LightColorType;
	};

	struct MapHeader
	{
		char Desc[20];
		uint16_t Version;
		int16_t Width;// 宽高的单位是cell
		int16_t Height;
		uint8_t EventFileIndex;
		uint8_t FogColor;
	};

	struct TileInfo
	{
		uint8_t FileIndex;
		uint16_t TileIndex;

		// 文件索引要做个变换，不知为何
		inline bool RemapFileIndex(int& idx) {
			ExFileIdx(idx);
			return IsMapTileFile(idx);
		}
	};
	// Obj1是下层元素，Obj2是上层元素
	struct CellInfo
	{
		uint8_t Flag;// 障碍层
		uint8_t Obj1Animation;
		uint8_t Obj2Animation;
		uint8_t Obj2FileIdx;
		uint8_t Obj1FileIdx;
		uint16_t Obj1ImgIdx;
		uint16_t Obj2ImgIdx;
		// DoorIndex & 0X80 for whether there is a door
		// DoorIndex & 0X7F for door index, if non-zero
		// DoorOffset & 0X80 for open/close the door
		// DoorOffset & 0X7F for door image offset
		uint16_t DoorIndex;
		uint8_t DoorOffset;
		uint16_t LightNEvent;

		inline uint32_t FileIndex1() { return Obj1FileIdx; }
		inline uint32_t FileIndex2() { return Obj2FileIdx; }
		inline bool File1Enable() { return FileIndex1() != 0xff && Obj1ImgIdx != 65535 && Obj1ImgIdx > 0 && Obj1ImgIdx < 0x7fffffff; }
		inline bool File2Enable() { return FileIndex2() != 0xff && Obj2ImgIdx != 65535 && Obj2ImgIdx > 0 && Obj2ImgIdx < 0x7fffffff; }
		inline bool HasAnim1() { return Obj1Animation != 255; }
		inline bool HasAnim2() { return Obj2Animation != 255; }
		inline bool Obj1Blend() { return Obj1Animation & 0x80 >> 7; }
		inline bool Obj2Blend() { return Obj2Animation & 0x80 >> 7; }
		inline unsigned char Obj1AnimTickType() { return Obj1Animation & 0x70 >> 4; }
		inline unsigned char Obj2AnimTickType() { return Obj2Animation & 0x70 >> 4; }
		inline short Obj1AnimCount() { return Obj1Animation & 0x0f; }
		inline short Obj2AnimCount() { return Obj2Animation & 0x0f; }
		inline bool HasDoor() { return (DoorOffset & 0x80) > 0 && (DoorIndex & 0x7f) > 0; }
		inline uint32_t DoorImgIdx() { return DoorOffset & 0x7f; }
		inline bool Walkable() { return Flag & 0x01 ? true : false; }
		// ext
		inline uint32_t FileIndexOf(uint8_t idx) {
			if (idx == 1)
				return FileIndex1();
			else if (idx == 2)
				return FileIndex2();
			else
				return 0;
		}
		inline uint32_t ImgIndexOf(uint8_t idx) {
			if (idx == 1)
				return Obj1ImgIdx;
			else if (idx == 2)
				return Obj2ImgIdx;
			else
				return 0;
		}
		inline bool FileEnableOf(uint8_t idx) {
			if (idx == 1)
				return File1Enable();
			else if (idx == 2)
				return File2Enable();
			else
				return false;
		}
		inline bool HasAnimOf(uint8_t idx) {
			if (idx == 1)
				return HasAnim1();
			else if (idx == 2)
				return HasAnim2();
			else
				return false;
		}
		inline bool ObjBlendOf(uint8_t idx) {
			if (idx == 1)
				return Obj1Blend();
			else if (idx == 2)
				return Obj2Blend();
			else
				return false;
		}
		inline unsigned char AnimTickTypeOf(uint8_t idx) {
			if (idx == 1)
				return Obj1AnimTickType();
			else if (idx == 2)
				return Obj2AnimTickType();
			else
				return 0;
		}
		inline short AnimCountOf(uint8_t idx) {
			if (idx == 1)
				return Obj1AnimCount();
			else if (idx == 2)
				return Obj2AnimCount();
			else
				return 0;
		}
		inline bool RemapFileIndex(int& idx) 
		{
			ExFileIdx(idx);
			return IsMapObjectFile(idx);
		}
	};

	struct DoorImageInfo
	{
		uint16_t PosX;
		uint16_t PosY;
		uint16_t ImageNum;
	};

	struct DoorInfo {
		uint8_t Index;
		uint8_t ImageCount;
		DoorImageInfo* ImageInfo;
	};
	#pragma pack()

	MapData();
	~MapData();
	void Load(wstring mapPath);
	bool IsLoaded();
	int16_t w();
	int16_t h();
	TileInfo TileAt(uint32_t cellX, uint32_t cellY);
	CellInfo CellAt(uint32_t x, uint32_t y);
	bool InMap(uint32_t cellX, uint32_t cellY);
	bool Walkable(uint32_t cellX, uint32_t cellY);

private:
	inline uint32_t GetCellIndex(uint32_t x, uint32_t y)
	{
		return y + x * mHeader->Height;
		//return x + y * mHeader->Width;
	}
	inline uint32_t GetTileIndex(uint32_t x, uint32_t y)
	{
		return (y / 2) + (x / 2) * mHeader->Height / 2;
		//return (x / 2) + (y / 2) * mHeader->Width / 2;
	}

	wstring mPath;
	MapHeader* mHeader;
	TileInfo* mTiles;
	CellInfo* mCells;
	unsigned char mDoorCount;
	DoorInfo* mDoors;
	uint8_t* mCellDoorIndices;
	uint32_t mTileCount;
	uint32_t mCellCount;
};

