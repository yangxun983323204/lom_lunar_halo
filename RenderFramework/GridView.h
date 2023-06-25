#pragma once
#include <functional>
#include <DirectXMath.h>
#include "../DirectXTK-main/Inc/SimpleMath.h"

using std::function;

typedef function<void(int, int)> CellNotifyCallback;

class GridView
{
public:
	GridView(int cellWidth, int cellHeight, int rows, int cols);
	// 设置视口大小
	void SetView(int width, int height);
	// 设置预加载区域
	void SetPrepareBorder(int width, int height);
	// 更新观察点
	void UpdateViewPoint(int x, int y);


	// 当cell需要显示时的回调
	void SetCellShowCallback(CellNotifyCallback func);
	// 当cell需要隐藏时的回调
	void SetCellHideCallback(CellNotifyCallback func);
	// 当cell处于预加载区域时的回调
	void SetCellWillShowCallback(CellNotifyCallback func);


private:
	CellNotifyCallback _onCellShow;
	CellNotifyCallback _onCellHide;
	CellNotifyCallback _onCellWillShow;

	DirectX::XMINT2 _viewPoint;
	DirectX::XMINT2 _viewSize;
	DirectX::XMINT2 _viewBorder;

	DirectX::XMINT2 _cellSize;
	DirectX::XMINT2 _cellCount;
	DirectX::XMINT2 _gridSize;

	DirectX::SimpleMath::Rectangle _viewRect;
	DirectX::SimpleMath::Rectangle _roiRect;
};

