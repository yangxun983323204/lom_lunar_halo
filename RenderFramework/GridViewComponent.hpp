#pragma once
#include "SceneNode.h"
#include "ISceneNodeComponent.h"
#include "GridView.h"

class GridViewComponent : public ISceneNodeComponent
{
public:
	_SCENE_NODE_COMPONENT_ID(GridViewComponent);
	inline void Init(uint32_t cellWidth, uint32_t cellHeight, uint32_t rows, uint32_t cols, CellCreateFunctor createFunctor = {})
	{
		_gridView = std::make_unique<GridView>(cellWidth, cellHeight, rows, cols, createFunctor);
		bEnableTick = true;
	}

	inline GridView* GetView() { return _gridView.get(); }

	virtual void Tick(uint64_t totalMs, uint32_t deltaMs) override
	{
		if (!_gridView)
			return;

		auto wpos = GetSceneNode()->GetWorldPosition();
		_gridView->UpdateViewPoint(wpos.x, wpos.y);
	}

	inline std::string GetDebugInfo() { return _gridView->GetDebugInfo(); }

protected:
	virtual void FillTypeIds(std::vector<uint32_t>& typeIds) override 
	{
		ISceneNodeComponent::FillTypeIds(typeIds);
		typeIds.push_back(GridViewComponent::TypeId);
	}
private:
	unique_ptr<GridView> _gridView;
};