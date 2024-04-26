#pragma once
#include "../RenderFramework/GridView.h"
#include "../RenderFramework/SceneNode.h"
#include <vector>
#include <map>

class MirWorldRenderManager;
class IGraphic2D;

class MirCellView : public CellView
{
public:
	MirCellView(int rowIdx, int colIdx, MirWorldRenderManager* mgr);
	virtual ~MirCellView() override;

	static void DrawLayer(int layer, CellView* inst, IGraphic2D* graphic);
protected:
	virtual void OnUnload() override;
	virtual void OnLoad() override;
	virtual void OnHide() override;
	virtual void OnShow() override;

	MirWorldRenderManager* _mirWorldMgr;

private:
	void SetUpBg();
	void SetUpMid();
	void SetUpDebugBarrier();

	std::vector<std::vector<weak_ptr<SceneNode>>> _layeredActors;

	static vector<weak_ptr<ISceneNodeComponent>> _tmpCpts;
};