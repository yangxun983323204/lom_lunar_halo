#include "GridView.h"

using namespace DirectX;

class GridView::Inner
{
public:
	Inner(XMUINT2 cellSize, XMUINT2 gridSize) :
		_onCellShow{}, _onCellHide{}, _onCellWillShow{},
		_cellSize{ cellSize }, _gridSize{gridSize},
		_preViewRect{}, _preRoiRect{},
		_viewRect{}, _roiRect{},
		_halfViewWidth{}, _halfViewHeight{},
		_halfRoiWidth{}, _halfRoiHeight{}
	{
		_gridRect = { 0,0,(long)gridSize.x, (long)gridSize.y };
	}

	void Init(XMINT2 viewPoint, XMUINT2 viewSize, XMUINT2 roiSize)
	{
		_halfViewWidth = viewSize.x / 2;
		_halfViewHeight = viewSize.y / 2;
		_halfRoiWidth = roiSize.x / 2;
		_halfRoiHeight = roiSize.y / 2;

		_viewRect.x = viewPoint.x - _halfViewWidth;
		_viewRect.y = viewPoint.y - _halfViewHeight;
		_viewRect.width = viewSize.x;
		_viewRect.height = viewSize.y;

		_roiRect.x = viewPoint.x - _halfRoiWidth;
		_roiRect.y = viewPoint.y - _halfRoiHeight;
		_roiRect.width = roiSize.x;
		_roiRect.height = roiSize.y;

		AlignRect(_viewRect);
		AlignRect(_roiRect);
		TryTriggerEvent();
	}

	void UpdateRect(XMINT2 viewPoint)
	{
		_preViewRect = _viewRect;
		_preRoiRect = _roiRect;

		_viewRect.x = viewPoint.x - _halfViewWidth;
		_viewRect.y = viewPoint.y - _halfViewHeight;
		_viewRect.width = _halfViewWidth * 2;
		_viewRect.height = _halfViewHeight * 2;

		_roiRect.x = viewPoint.x - _halfRoiWidth;
		_roiRect.y = viewPoint.y - _halfRoiHeight;
		_roiRect.width = _halfRoiWidth * 2;
		_roiRect.height = _halfRoiHeight * 2;

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
		auto intersect = SimpleMath::Rectangle::Intersect(_preRoiRect, _roiRect);
		if (intersect == _roiRect)// 区域覆盖的格子未变化
			return;

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

				if (intersect.Contains(x, y))
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

				if (intersect.Contains(x, y))
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

	uint32_t _halfViewWidth, _halfViewHeight;
	uint32_t _halfRoiWidth, _halfRoiHeight;
};

GridView::GridView(uint32_t cellWidth, uint32_t cellHeight, uint32_t rows, uint32_t cols):
	_viewPoint{}, _viewSize{}, _viewBorder{},
	_cellSize{cellWidth, cellHeight}, _cellCount{cols, rows}, _gridSize{cellWidth*cols, cellHeight*rows}
{
	_inner = new GridView::Inner(_cellSize, _gridSize);
}

GridView::~GridView()
{
	delete _inner;
}
void GridView::SetView(uint32_t width, uint32_t height, uint32_t borderX, uint32_t borderY)
{
	_viewSize = { width, height };
	_viewBorder = { borderX, borderY };
	XMUINT2 roi = { width + borderX, height + borderY };
	_inner->Init({ -10000000,-10000000 }, _viewSize, roi);
}

void GridView::UpdateViewPoint(int x, int y)
{
	_viewPoint.x = x;
	_viewPoint.y = y;
	_inner->UpdateRect(_viewPoint);
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
