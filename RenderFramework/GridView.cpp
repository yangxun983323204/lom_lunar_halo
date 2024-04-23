#include "GridView.h"

using namespace DirectX;

class GridView::Inner
{
public:
	Inner(XMUINT2 cellSize, XMUINT2 gridSize, CellCreateFunctor createFunctor = {}, CellDestroyFunctor destroyFunctor = {}) :
		_onCellShow{}, _onCellHide{}, _onCellWillShow{},
		_cellSize{ cellSize }, _gridSize{gridSize},
		_preViewRect{}, _preRoiRect{},
		_viewRect{}, _roiRect{},
		_viewSize{}, _border{}
	{
		_gridRect = { 0,0,(long)gridSize.x, (long)gridSize.y };
	}

	inline uint32_t ViewUp() { return _viewSize.x; }
	inline uint32_t ViewDown() { return _viewSize.y; }
	inline uint32_t ViewLeft() { return _viewSize.z; }
	inline uint32_t ViewRight() { return _viewSize.w; }
	inline uint32_t ViewWidth() { return _viewSize.w + _viewSize.z; }
	inline uint32_t ViewHeight() { return _viewSize.x + _viewSize.y; }

	void Init(XMINT2 viewPoint, XMUINT4 viewSize, XMUINT2 border)
	{
		_viewSize = viewSize;
		_border = border;

		UpdateRect(viewPoint);
	}

	inline int GetRow(int y) 
	{
		return y / _cellSize.y; 
	}

	inline int GetCol(int x)
	{
		return x / _cellSize.x;
	}

	CellView* GetCellView(int x, int y)
	{
		x = GetCol(x);
		y = GetRow(y);
		return _cells[y * _cols + x];
	}

	void UpdateRect(XMINT2 viewPoint)
	{
		_preViewRect = _viewRect;
		_preRoiRect = _roiRect;

		_viewRect.x = viewPoint.x - ViewLeft();
		_viewRect.y = viewPoint.y - ViewDown();
		_viewRect.width = ViewWidth();
		_viewRect.height = ViewHeight();

		_roiRect.x = viewPoint.x - ViewLeft() - _border.x;
		_roiRect.y = viewPoint.y - ViewDown() - _border.y;
		_roiRect.width = ViewWidth() + _border.x * 2;
		_roiRect.height = ViewHeight() + _border.y * 2;

		AlignRect(_viewRect);
		AlignRect(_roiRect);
		TryTriggerEvent();
	}
	// 使rect范围向外扩充，与格子边框对齐
	void AlignRect(SimpleMath::Rectangle& rect)
	{
		auto newX = (long)(floor((float)rect.x / _cellSize.x) * _cellSize.x);
		auto newY = (long)(floor((float)rect.y / _cellSize.y) * _cellSize.y);
		rect.width += rect.x - newX;
		rect.height += rect.y - newY;
		rect.x = newX;
		rect.y = newY;
		rect.width += _cellSize.x - rect.width % _cellSize.x;
		rect.height += _cellSize.y - rect.height % _cellSize.y;
	}

	void TryTriggerEvent()
	{
		if (_preViewRect == _viewRect)// 区域覆盖的格子未变化
			return;

		auto intersectView = SimpleMath::Rectangle::Intersect(_preViewRect, _viewRect);
		// 计算更新
		long startY = _preRoiRect.y;
		long stopY = _preRoiRect.y + _preRoiRect.height;
		long startX = _preRoiRect.x;
		long stopX = _preRoiRect.x + _preRoiRect.width;

		for (long y = startY; y < stopY; y+=_cellSize.y)
		{
			for (long x = startX; x < stopX; x+=_cellSize.x)
			{
				if (!_gridRect.Contains(x, y))
					continue;

				if (intersectView.Contains(x, y))
					continue;

				if (_preViewRect.Contains(x, y))
					_onCellHide(x / _cellSize.x, y / _cellSize.y);
			}
		}

		startY = _roiRect.y;
		stopY = _roiRect.y + _roiRect.height;
		startX = _roiRect.x;
		stopX = _roiRect.x + _roiRect.width;

		for (long y = _roiRect.y; y < stopY; y += _cellSize.y)
		{
			for (long x = _roiRect.x; x < stopX; x += _cellSize.x)
			{
				if (!_gridRect.Contains(x, y))
					continue;

				if (intersectView.Contains(x, y))
					continue;

				_onCellWillShow(x / _cellSize.x, y / _cellSize.y);
				if (_viewRect.Contains(x, y))
					_onCellShow(x / _cellSize.x, y / _cellSize.y);
			}
		}
		//
		_preViewRect = _viewRect;
		_preRoiRect = _roiRect;
	}

public:
	CellNotifyCallback _onCellShow;
	CellNotifyCallback _onCellHide;
	CellNotifyCallback _onCellWillShow;

	XMUINT2 _cellSize;
	XMUINT2 _gridSize;

	SimpleMath::Rectangle _gridRect;

	SimpleMath::Rectangle _preViewRect;
	SimpleMath::Rectangle _preRoiRect;
	SimpleMath::Rectangle _viewRect;
	SimpleMath::Rectangle _roiRect;

	XMUINT4 _viewSize;
	XMUINT2 _border;

	// @todo 重构中
	uint32_t _cols;
	uint32_t _rows;
	CellView** _cells;
};

GridView::GridView(uint32_t cellWidth, uint32_t cellHeight, uint32_t rows, uint32_t cols, CellCreateFunctor createFunctor, CellDestroyFunctor destroyFunctor):
	_viewPoint{}, _viewSize{}, _viewBorder{},
	_cellSize{cellWidth, cellHeight}, _cellCount{cols, rows}, _gridSize{cellWidth*cols, cellHeight*rows}
{
	_inner = new GridView::Inner(_cellSize, _gridSize, createFunctor, destroyFunctor);
}

GridView::~GridView()
{
	delete _inner;
}
void GridView::SetView(uint32_t up, uint32_t down, uint32_t left, uint32_t right, uint32_t borderX, uint32_t borderY)
{
	_viewSize = { up, down, left, right };
	_viewBorder = { borderX, borderY };
	_inner->Init({ -10000000,-10000000 }, _viewSize, _viewBorder);
}

void GridView::UpdateViewPoint(int x, int y)
{
	_viewPoint.x = x;
	_viewPoint.y = y;
	_inner->UpdateRect(_viewPoint);
}

CellView* GridView::GetCellView(int x, int y)
{
	return _inner->GetCellView(x, y);
}

void GridView::SetCellShowCallback(CellNotifyCallback func)
{
	_inner->_onCellShow = func;
}

void GridView::SetCellHideCallback(CellNotifyCallback func)
{
	_inner->_onCellHide = func;
}

void GridView::SetCellWillShowCallback(CellNotifyCallback func)
{
	_inner->_onCellWillShow = func;
}
