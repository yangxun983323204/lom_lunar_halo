#include "pch.h"
#include "GUIBase.h"
#include <algorithm>
#include "../StringCodec.hpp"

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

void GUIElement::SetParent(std::shared_ptr<GUIElement> self, std::shared_ptr<GUIElement> parent)
{
	if (self!=0 && self.get()!=this)
	{
		throw "GUIElement::SetParent的self参数请传入0或者自身的shared_ptr";
	}
	if (!_parent.expired())
	{
		auto parent = _parent.lock();
		auto children = parent->_children;
		std::remove_if(children.begin(), children.end(), [&parent](shared_ptr<GUIElement> n)
			{
				return n.get() == parent.get();
			}
		);
	}

	_parent = parent;
	if (self != 0)
		parent->_children.push_back(self);
	else
		parent->_children.push_back(std::shared_ptr<GUIElement>(this));

	if (_delayFill)
	{
		FillParent();
		_delayFill = false;
	}
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

RECT GUIElement::GetScreenRect()
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

bool GUIElement::HitTest(int sx, int sy)
{
	if (!(Interactable && IsVisiable() && IsActive()))
		return false;

	RECT sRect = GetScreenRect();
	return sx > sRect.left &&
		sx<sRect.right&&
		sy>sRect.bottom &&
		sy < sRect.top;
}

void GUIElement::LoadXml(tinyxml2::XMLElement* e)
{
	char* pbuffer;
	if(e->QueryStringAttribute("Id", (const char**)&pbuffer) == tinyxml2::XML_SUCCESS)
		Id = Utf8ToWString(pbuffer);

	Interactable = e->BoolAttribute("Interactable", true);
	SelfVisiable = e->BoolAttribute("SelfVisiable", true);
	SelfActive = e->BoolAttribute("SelfActive", true);
	Width = e->IntAttribute("Width", 0);
	Height = e->IntAttribute("Height", 0);
	LocalPos.x = e->IntAttribute("LocalPos.x", 0);
	LocalPos.y = e->IntAttribute("LocalPos.y", 0);
	Pivot.x = e->FloatAttribute("Pivot.x", 0);
	Pivot.y = e->FloatAttribute("Pivot.x", 0);
	bool fillParent = e->BoolAttribute("FillParent", false);
	if (fillParent && !_parent.expired())
	{
		FillParent();
	}
	else if(fillParent) {
		_delayFill = true;
	}
}

extern void DrawTexture(ID3D11ShaderResourceView* srv, RECT rect);

void Graphic::Draw()
{
	if (!_sprite)
		return;

	DrawTexture(this->GetSprite()->SRV().Get(), this->GetScreenRect());
}