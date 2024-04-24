#pragma once
#include "../RenderFramework/GridView.h"

class MirWorldRenderManager;

class MirCellView : public CellView
{
public:
	virtual ~MirCellView() override;

protected:
	virtual void OnUnload() override;
	virtual void OnLoad() override;
	virtual void OnHide() override;
	virtual void OnShow() override;

	MirWorldRenderManager* _mirWorldMgr;
};