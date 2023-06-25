#include "GridView.h"

GridView::GridView(int cellWidth, int cellHeight, int rows, int cols):
	_onCellShow{}, _onCellHide{}, _onCellWillShow{},
	_viewPoint{}, _viewSize{}, _viewBorder{},
	_cellSize{cellWidth, cellHeight}, _cellCount{cols, rows}, _gridSize{cellWidth*cols, cellHeight*rows},
	_viewRect{}, _roiRect{}
{
}

void GridView::SetView(int width, int height)
{
	_viewSize = { width, height };
}

void GridView::SetPrepareBorder(int width, int height)
{
	_viewBorder = { width, height };
}

void GridView::UpdateViewPoint(int x, int y)
{
	_viewPoint.x = x;
	_viewPoint.y = y;
	// todo 计算新增要显示的、隐藏的、准备显示的格子
}

void GridView::SetCellShowCallback(CellNotifyCallback func)
{
	_onCellShow = func;
}

void GridView::SetCellHideCallback(CellNotifyCallback func)
{
	_onCellHide = func;
}

void GridView::SetCellWillShowCallback(CellNotifyCallback func)
{
	_onCellWillShow = func;
}
