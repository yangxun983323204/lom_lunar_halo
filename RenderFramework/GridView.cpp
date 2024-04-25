#include "GridView.h"
#include <format>

using namespace DirectX;

void CellView::SetStatus(Status newStatus)
{
	auto old = _status;
	_status = newStatus;
	switch (_status)
	{
	case CellView::Status::Unload:
		if (old != _status)
		{
			OnUnload();
		}
		break;
	case CellView::Status::Hide:
		if (old == CellView::Status::Show)
		{
			OnHide();
		}
		break;
	case CellView::Status::Load:
		if (old == CellView::Status::Unload)
		{
			OnLoad();
		}
		break;
	case CellView::Status::Show:
		if (old != CellView::Status::Unload && old != _status)
		{
			OnShow();
		}
		break;
	default:
		break;
	}
}

class GridView::Inner
{
public:
	Inner(XMUINT2 cellSize, XMUINT2 gridSize, CellCreateFunctor createFunctor = {}) :
		_onCellShow{}, _onCellHide{}, _onCellWillShow{},
		_cellSize{ cellSize }, _gridSize{gridSize},
		_preViewRect{}, _preRoiRect{},
		_viewRect{}, _roiRect{}, _persistRect{},
		_viewSize{}, _border{}
	{
		_cols = gridSize.x / cellSize.x;
		_rows = gridSize.y / cellSize.y;

		_gridRect = { 0,0,(long)gridSize.x, (long)gridSize.y };
		_cells = new CellView*[_cols * _rows];
		for (size_t x = 0; x < _cols; x++)
		{
			for (size_t y = 0; y < _rows; y++)
			{
				_cells[y * _cols + x] = createFunctor ? createFunctor(y, x) : new CellView(y, x);
			}
		}
	}

	~Inner() 
	{
		for (size_t x = 0; x < _cols; x++)
		{
			for (size_t y = 0; y < _rows; y++)
			{
				delete _cells[y * _cols + x];
			}
		}

		delete[] _cells;
		_cells = nullptr;
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
		auto col = GetCol(x);
		auto row = GetRow(y);
		return _cells[row * _cols + col];
	}

	CellView* GetCellViewDirect(int col, int row)
	{
		return _cells[row * _cols + col];
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

		_persistRect.x = viewPoint.x - ViewLeft() - _border.x * 3;
		_persistRect.y = viewPoint.y - ViewDown() - _border.y * 3;
		_persistRect.width = ViewWidth() + _border.x * 2 * 3;
		_persistRect.height = ViewHeight() + _border.y * 2 * 3;

		TryTriggerEvent();
	}

	// 得到使rect范围向外扩充，与格子边框对齐后的cell范围
	SimpleMath::Rectangle GetAlignCellRect(SimpleMath::Rectangle rect)
	{
		auto minX = rect.x;
		auto maxX = rect.x + rect.width;
		auto minY = rect.y;
		auto maxY = rect.y + rect.height;

		minX = (long)floor((float)minX / _cellSize.x);
		minY = (long)floor((float)minY / _cellSize.y);
		maxX = (long)ceil((float)maxX / _cellSize.x);
		maxY = (long)ceil((float)maxY / _cellSize.y);

		if (minX < 0) minX = 0;
		else if (minX > _cols) minX = _cols;

		if (maxX < 0) maxX = 0;
		else if (maxX > _cols) maxX = _cols;

		if (minY < 0) minY = 0;
		else if (minY > _rows) minY = _rows;

		if (maxY < 0) maxY = 0;
		else if (maxY > _rows) maxY = _rows;

		return { minX, minY, maxX-minX, maxY-minY };
	}

	void TryTriggerEvent()
	{
		auto preCellViewRect = GetAlignCellRect(_preViewRect);
		auto cellViewRect = GetAlignCellRect(_viewRect);

		if (preCellViewRect == cellViewRect)// 区域覆盖的格子未变化
			return;

		auto intersectView = SimpleMath::Rectangle::Intersect(preCellViewRect, cellViewRect);

		auto gridCellRect = GetAlignCellRect(_gridRect);
		auto preCellRoiRect = GetAlignCellRect(_preRoiRect);
		auto cellRoiRect = GetAlignCellRect(_roiRect);
		auto persistRect = GetAlignCellRect(_persistRect);

		// 计算更新
		long startY = preCellRoiRect.y;
		long stopY = preCellRoiRect.y + preCellRoiRect.height;
		long startX = preCellRoiRect.x;
		long stopX = preCellRoiRect.x + preCellRoiRect.width;

		// 处理旧矩形
		for (long y = startY; y < stopY; y++)
		{
			for (long x = startX; x < stopX; x++)
			{
				if (!gridCellRect.Contains(x, y))
					continue;

				if (intersectView.Contains(x, y))
					continue;

				auto* cellView = GetCellViewDirect(x, y);
				cellView->SetStatus(CellView::Status::Hide);
				if (!persistRect.Contains(x, y))
				{
					cellView->SetStatus(CellView::Status::Unload);
				}

				//if (preCellViewRect.Contains(x, y)) 
				//{
				//	if (cellView->_status != CellView::Status::Hide)
				//	{
				//		cellView->_status = CellView::Status::Hide;
				//		cellView->OnHide();
				//		_onCellHide(cellView);
				//		__hideCnt++;
				//	}
				//}
			}
		}

		startY = cellRoiRect.y;
		stopY = cellRoiRect.y + cellRoiRect.height;
		startX = cellRoiRect.x;
		stopX = cellRoiRect.x + cellRoiRect.width;
		// 处理新矩形
		for (long y = cellRoiRect.y; y < stopY; y++)
		{
			for (long x = cellRoiRect.x; x < stopX; x++)
			{
				if (!gridCellRect.Contains(x, y))
					continue;

				if (intersectView.Contains(x, y))
					continue;

				auto* cellView = GetCellViewDirect(x, y); 
				cellView->SetStatus(CellView::Status::Load);
				if (cellViewRect.Contains(x, y))
				{
					cellView->SetStatus(CellView::Status::Show);
				}

				/*if (cellView->_status != CellView::Status::Load)
				{
					cellView->_status = CellView::Status::Load;
					cellView->OnLoad();
					_onCellWillShow(cellView);
					__loadCnt++;
				}
				
				if (cellViewRect.Contains(x, y)) 
				{
					if (cellView->_status != CellView::Status::Show)
					{
						cellView->_status = CellView::Status::Show;
						cellView->OnShow();
						_onCellShow(cellView);
						__showCnt++;
					}
				}*/
			}
		}
		//
		_preViewRect = _viewRect;
		_preRoiRect = _roiRect;
		// 统计信息
		__unloadCnt = 0;
		__hideCnt = 0;
		__loadCnt = 0;
		__showCnt = 0;
		for (size_t x = 0; x < _cols; x++)
		{
			for (size_t y = 0; y < _rows; y++)
			{
				auto s = _cells[y * _cols + x]->_status;
				switch (s)
				{
				case CellView::Status::Unload:
					__unloadCnt++;
					break;
				case CellView::Status::Load:
					__loadCnt++;
					break;
				case CellView::Status::Hide:
					__hideCnt++;
					break;
				case CellView::Status::Show:
					__showCnt++;
					break;
				default:
					break;
				}
			}
		}
	}

public:
	CellNotifyCallback _onCellShow;
	CellNotifyCallback _onCellHide;
	CellNotifyCallback _onCellWillShow;

	XMUINT2 _cellSize;
	XMUINT2 _gridSize;

	SimpleMath::Rectangle _gridRect;

	// 上一帧可视区
	SimpleMath::Rectangle _preViewRect;
	// 上一帧预加载视区
	SimpleMath::Rectangle _preRoiRect;
	// 可视视区
	SimpleMath::Rectangle _viewRect;
	// 预加载视区
	SimpleMath::Rectangle _roiRect;
	// 资源保持视区
	SimpleMath::Rectangle _persistRect;

	XMUINT4 _viewSize;
	XMUINT2 _border;

	// @todo 重构中
	uint32_t _cols;
	uint32_t _rows;
	CellView** _cells;

	int __hideCnt;
	int __loadCnt;
	int __showCnt;
	int __unloadCnt;
};

GridView::GridView(uint32_t cellWidth, uint32_t cellHeight, uint32_t rows, uint32_t cols, CellCreateFunctor createFunctor):
	_viewPoint{}, _viewSize{}, _viewBorder{},
	_cellSize{cellWidth, cellHeight}, _cellCount{cols, rows}, _gridSize{cellWidth*cols, cellHeight*rows}
{
	_inner = new GridView::Inner(_cellSize, _gridSize, createFunctor);
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

std::string GridView::GetDebugInfo()
{
	return std::format("unload:{0}, load:{1}, show:{2}, hide:{3}, total:{4}",
		_inner->__unloadCnt,
		_inner->__loadCnt,
		_inner->__showCnt,
		_inner->__hideCnt,
		_inner->__unloadCnt + _inner->__loadCnt + _inner->__showCnt + _inner->__hideCnt);
}
