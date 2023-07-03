#pragma once
#include "SceneNode.h"
#include "ISceneNodeComponent.h"
#include "GridView.h"

class GridViewComponent : public ISceneNodeComponent
{
public:
	SCENE_NODE_COMPONENT('gvc');
	inline void Init(uint32_t cellWidth, uint32_t cellHeight, uint32_t rows, uint32_t cols)
	{
		_gridView = std::make_unique<GridView>(cellWidth, cellHeight, rows, cols);
	}

	inline GridView* GetView() { return _gridView.get(); }

	inline void SyncNodePos()
	{
		if (!_gridView)
			return;

		auto wpos = GetSceneNode()->GetWorldPosition();
		_gridView->UpdateViewPoint(wpos.x, wpos.y);
	}
private:
	unique_ptr<GridView> _gridView;
};