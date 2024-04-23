#pragma once
#include <memory>
#include <functional>
#include <DirectXMath.h>
#include "../DirectXTK-main/Inc/SimpleMath.h"

using std::function;

class GridView;

class CellView
{
public:
	enum class Status
	{
		Hide,
		WillShow,
		Show,
	};
public:
	CellView(int rowIdx, int colIdx):
		_rowIdx{ rowIdx }, _colIdx{ colIdx }, _status{ Status::Hide }
	{}

	virtual ~CellView() {}

	inline int GetRowIdx() { return _rowIdx; }
	inline int GetColIdx() { return _colIdx; }
private:
	friend GridView;
	int _rowIdx;
	int _colIdx;
	Status _status;
};

typedef function<void(int, int)> CellNotifyCallback;
typedef function<CellView*(int, int)> CellCreateFunctor;
typedef function<void(CellView*)> CellDestroyFunctor;

class GridView
{
public:
	GridView(uint32_t cellWidth, uint32_t cellHeight, uint32_t rows, uint32_t cols, CellCreateFunctor createFunctor = {}, CellDestroyFunctor destroyFunctor = {});
	~GridView();
	// 设置视口大小和预加载区域
	void SetView(uint32_t up, uint32_t down, uint32_t left, uint32_t right, uint32_t borderX, uint32_t borderY);
	// 更新观察点
	void UpdateViewPoint(int x, int y);
	CellView* GetCellView(int x, int y);


	// 当cell需要显示时的回调
	void SetCellShowCallback(CellNotifyCallback func);
	// 当cell需要隐藏时的回调
	void SetCellHideCallback(CellNotifyCallback func);
	// 当cell处于预加载区域时的回调
	void SetCellWillShowCallback(CellNotifyCallback func);


private:
	DirectX::XMINT2 _viewPoint;
	DirectX::XMUINT4 _viewSize;
	DirectX::XMUINT2 _viewBorder;

	DirectX::XMUINT2 _cellSize;
	DirectX::XMUINT2 _cellCount;
	DirectX::XMUINT2 _gridSize;

	class Inner;
	Inner* _inner;
};

