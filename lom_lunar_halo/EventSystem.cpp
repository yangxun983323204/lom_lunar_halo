#include "pch.h"
#include "EventSystem.h"

using namespace YX;

MouseEventData::MouseEventData():
	ButtonType{ MouseEventData::InputButton::Left },
	State{ MouseEventData::FramePressState::Released },
	HoverObject{},
	PressObject{},
	DragObject{},
	RawObject{},
	EligibleClick{false},
	x{0},y{0},deltaX{0},deltaY{0},
	scrollDelta{0}
{

}

EventSystem::EventSystem():
	_casters{},
	_leftEvt{},
	_rightEvt{},
	_middleEvt{},
	_preX{0},_preY{0},
	_preScroll{0}
{

}

void EventSystem::Update()
{
	UpdateMouseState();

	ProcessMousePress(_leftEvt);
	ProcessMove(_leftEvt);
	ProcessDrag(_leftEvt);

	ProcessMousePress(_rightEvt);
	ProcessDrag(_rightEvt);

	ProcessMousePress(_middleEvt);
	ProcessDrag(_middleEvt);

	ProcessWheel(_leftEvt);
}

void EventSystem::AddCaster(shared_ptr<InteractObjectCaster> caster)
{
	for (auto i = _casters.begin(); i != _casters.end(); i++)
	{
		if ((*i) < caster)
		{
			_casters.insert(i, caster);
			return;
		}
	}

	_casters.push_back(caster);
}

void EventSystem::RemoveCaster(shared_ptr<InteractObjectCaster> caster)
{
	_casters.remove(caster);
}

void EventSystem::UpdateMouseState()
{
	auto state = DirectX::Mouse::Get().GetState();
	auto dx = state.x - _preX;
	auto dy = state.y - _preY;
	auto dScroll = state.scrollWheelValue - _preScroll;
	UpdateEvent(_leftEvt, state.x, state.y, dx, dx, dScroll, state.leftButton);
	UpdateEvent(_rightEvt, state.x, state.y, dx, dx, dScroll, state.rightButton);
	UpdateEvent(_middleEvt, state.x, state.y, dx, dx, dScroll, state.middleButton);
	_preX = state.x;
	_preY = state.y;
	_preScroll = state.scrollWheelValue;
}

void EventSystem::UpdateEvent(MouseEventData& evt, int x, int y, int dx, int dy, int scroll, bool down)
{
	if (down) {
		switch (evt.State)
		{
		case MouseEventData::FramePressState::OnPress:
		case MouseEventData::FramePressState::Pressed:
			evt.State = MouseEventData::FramePressState::Pressed;
			break;
		case MouseEventData::FramePressState::OnRelease:
		case MouseEventData::FramePressState::Released:
			evt.State = MouseEventData::FramePressState::OnPress;
			break;
		default:
			break;
		}
	}
	else {
		switch (evt.State)
		{
		case MouseEventData::FramePressState::OnPress:
		case MouseEventData::FramePressState::Pressed:
			evt.State = MouseEventData::FramePressState::OnRelease;
			break;
		case MouseEventData::FramePressState::OnRelease:
		case MouseEventData::FramePressState::Released:
			evt.State = MouseEventData::FramePressState::Pressed;
			break;
		default:
			break;
		}
	}

	evt.x = x;
	evt.y = y;
	evt.deltaX = dx;
	evt.deltaY = dy;
	evt.scrollDelta = scroll;
}

void EventSystem::ProcessMousePress(MouseEventData& evt)
{
	auto castObj = Cast(evt.x, evt.y);
	auto clickObj = castObj.lock()->FindHandler([](auto p) {return p.lock()->CanClick(); });
}
void EventSystem::ProcessMove(MouseEventData& evt)
{
	auto castObj = Cast(evt.x, evt.y);
	auto hoverObj = castObj.lock()->FindHandler([](auto p) {return p.lock()->CanHover(); });
}
void EventSystem::ProcessDrag(MouseEventData& evt)
{
	auto castObj = Cast(evt.x, evt.y);
	auto dragObj = castObj.lock()->FindHandler([](auto p) {return p.lock()->CanDrag(); });
}
void EventSystem::ProcessWheel(MouseEventData& evt)
{
	auto castObj = Cast(evt.x, evt.y);
	auto scrollObj = castObj.lock()->FindHandler([](auto p) {return p.lock()->CanScroll(); });
}

weak_ptr<IInteractObject> EventSystem::Cast(int screenX, int screenY)
{
	weak_ptr<IInteractObject> tar{};
	for (auto i = _casters.begin(); i != _casters.end(); i++)
	{
		tar = (*i)->Cast(screenX, screenX);
		if (!tar.expired())
		{
			break;
		}
	}

	return tar;
}