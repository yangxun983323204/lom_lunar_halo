#pragma once
#include <functional>

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
};

