#include "pch.h"
#include "HeroRenderer.h"

void HeroRenderer::Init(HeroData& hero, AnimDatabase& animDB, WilSpriteManager* resMgr)
{

}

void HeroRenderer::OnRender(IGraphic2D* gfx)
{
}

void HeroRenderer::FillTypeIds(std::set<uint32_t>& set)
{
	IRendererComponent::FillTypeIds(set);
	set.insert(HeroRenderer::TypeId);
}
