#include "pch.h"
#include "MapData.h"

MapData::MapData()
{
	mHeader = nullptr;
	mTiles = nullptr;
	mCells = nullptr;
	mDoorCount = 0;
	mDoors = nullptr;
	mCellDoorIndices = nullptr;
}


MapData::~MapData()
{
	delete mHeader;
	delete mTiles;
	delete mCells;
	delete mDoors;
	delete mCellDoorIndices;
}

void MapData::Load(wstring mapPath)
{
	mPath = mapPath;
	FILE* f;
	_wfopen_s(&f, mPath.c_str(), L"rb");
	mHeader = new MapHeader();
	fread(reinterpret_cast<void*>(mHeader), sizeof(MapHeader), 1, f);
	mTileCount = mHeader->Width * mHeader->Height / 4;
	mTiles = new TileInfo[mTileCount];// 一个tile被分为田字形的4个cell
	mCellCount = mHeader->Width * mHeader->Height;
	mCells = new CellInfo[mCellCount];
	fread(reinterpret_cast<void*>(mTiles), sizeof(TileInfo), mHeader->Width * mHeader->Height / 4, f);
	fread(reinterpret_cast<void*>(mCells), sizeof(CellInfo), mHeader->Width * mHeader->Height, f);
	fread(reinterpret_cast<void*>(&mDoorCount), sizeof(unsigned char), 1, f);
	if (mDoorCount <= 0)
		return;
	mDoors = new DoorInfo[mDoorCount];
	for (size_t i = 0; i < mDoorCount; i++)
	{
		auto door = mDoors[i];
		fread(reinterpret_cast<void*>(&door), sizeof(DoorInfo), 1, f);
		door.ImageInfo = new DoorImageInfo[door.ImageCount];
		for (size_t j = 0; j < door.ImageCount; j++)
		{
			fread(reinterpret_cast<void*>(&door.ImageInfo[i]), sizeof(DoorImageInfo), 1, f);
		}
	}
	// todo 此处可优化存储，考虑用稀疏矩阵什么的
	// 这段代码说明，一个门的范围可以大于一个cell
	mCellDoorIndices = new uint8_t[mHeader->Width * mHeader->Height];
	memset(reinterpret_cast<void*>(mCellDoorIndices), 0, sizeof(uint8_t) * mHeader->Width * mHeader->Height);
	for (size_t i = 0; i < mDoorCount; i++)
	{
		auto door = mDoors[i];
		for (size_t j = 0; j < mDoors[i].ImageCount; j++)
		{
			auto imgInfo = door.ImageInfo[j];
			mCellDoorIndices[imgInfo.PosX + imgInfo.PosY * mHeader->Width] = door.Index;
		}
	}
	fclose(f);
}

int16_t MapData::w()
{
	if (mHeader != nullptr)
		return mHeader->Width;
	else
		return -1;
}

int16_t MapData::h()
{
	if (mHeader != nullptr)
		return mHeader->Height;
	else
		return -1;
}

MapData::TileInfo MapData::TileAt(uint32_t cellX, uint32_t cellY)
{
	auto idx = (cellY / 2) + (cellX / 2) * mHeader->Height / 2;
	return mTiles[idx];
}

MapData::CellInfo MapData::CellAt(uint32_t x, uint32_t y)
{
	return mCells[y + x * mHeader->Height];
}

bool MapData::InMap(uint32_t cellX, uint32_t cellY)
{
	auto v = cellY + cellX * mHeader->Height;
	return cellX < w() && cellY < h() && v >= 0 && v < mCellCount;
}

bool MapData::Walkable(uint32_t cellX, uint32_t cellY)
{
	if (!InMap(cellX, cellY)) return false;
	auto v = cellY + cellX * mHeader->Height;
	return v > 0 && v < mCellCount&& mCells[v].Walkable();
}
