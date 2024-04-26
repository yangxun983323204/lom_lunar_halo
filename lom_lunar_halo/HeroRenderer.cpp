#include "pch.h"
#include "HeroRenderer.h"

void HeroRenderer::Init(HeroData& hero, AnimDatabase& animDB, WilSpriteManager* resMgr)
{

}

void HeroRenderer::OnRender(IGraphic2D* gfx)
{
}

void HeroRenderer::FillTypeIds(std::vector<uint32_t>& typeIds)
{
	IRendererComponent::FillTypeIds(typeIds);
	typeIds.push_back(HeroRenderer::TypeId);
}
