#include "pch.h"
#include "GUIBase.h"
#include <algorithm>

using namespace YX::GUI;
using namespace DirectX;

bool GUIElement::IsVisiable()
{
	if (!_parent.expired())
	{
		auto parent = _parent.lock();
		return parent->IsVisiable() && SelfVisiable;
	}
	else
		return SelfVisiable;
}

bool GUIElement::IsActive()
{
	if (!_parent.expired())
	{
		auto parent = _parent.lock();
		return parent->IsActive() && SelfActive;
	}
	else
		return SelfActive;
}

void GUIElement::SetParent(std::shared_ptr<GUIElement> e)
{
	if (!_parent.expired())
	{
		auto parent = _parent.lock();
		auto children = parent->_children;
		std::remove_if(children.begin(), children.end(), [&e](shared_ptr<GUIElement> n)
			{
				return n.get() == e.get();
			}
		);
	}

	_parent = e;
	e->_children.push_back(std::shared_ptr<GUIElement>(this));
}

DirectX::XMINT2 GUIElement::GetWorldPos()
{
	if (!_parent.expired())
	{
		auto parent = _parent.lock();
		auto pWPos = parent->GetWorldPos();
		return XMINT2{ pWPos.x+LocalPos.x , pWPos.y + LocalPos.y };
	}
	else
		return LocalPos;
}

RECT GUIElement::GetLocalRect()
{
	long xOffset = -(long)(Pivot.x * Width);
	long yOffset = -(long)(Pivot.y * Height);
	return RECT{ xOffset, yOffset, Width + xOffset, yOffset + Height };
}

RECT GUIElement::GetWorldRect()
{
	auto rect = GetLocalRect();
	auto wpos = GetWorldPos();
	rect.left += wpos.x;
	rect.right += wpos.x;
	rect.top += wpos.y;
	rect.bottom += wpos.y;
	return rect;
}

bool GUIElement::FillParent()
{
	if (_parent.expired())
		return false;
	else {
		auto p = _parent.lock();
		Width = p->Width;
		Height = p->Height;
		LocalPos.x = Pivot.x * Width;
		LocalPos.y = Pivot.y * Height;
	}
}