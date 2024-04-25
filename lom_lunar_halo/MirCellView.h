#pragma once
#include "../RenderFramework/GridView.h"
#include "../RenderFramework/SceneNode.h"
#include <vector>

class MirWorldRenderManager;

class MirCellView : public CellView
{
public:
	MirCellView(int rowIdx, int colIdx, MirWorldRenderManager* mgr);
	virtual ~MirCellView() override;

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
	void SetUpDebugFrame();

	std::vector<weak_ptr<SceneNode>> _actors;
	weak_ptr<SceneNode> _barrierTex;
	weak_ptr<SceneNode> _frameLeftTex;
	weak_ptr<SceneNode> _frameBottomTex;
};