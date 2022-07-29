#pragma once
#include "pch.h"
#include <memory>
#include <vector>
#include <list>

using std::shared_ptr;
using std::weak_ptr;
using std::vector;
using std::list;

namespace YX
{
	interface IInteractObject
	{
		virtual bool CanHover() = 0;
		virtual bool CanClick() = 0;
		virtual bool CanDrag() = 0;
		virtual bool CanScroll() = 0;
		virtual IInteractObject* GetParent() = 0;
	};

	class MouseEventData
	{
	public:
		enum class InputButton
		{
			Left, Right, Middle,
		};

		enum class FramePressState
		{
			OnPress,OnRelease,Pressed, Released
		};

		InputButton ButtonType;
		FramePressState State;
		weak_ptr<IInteractObject> HoverObject;
		weak_ptr<IInteractObject> PressObject;
		weak_ptr<IInteractObject> DragObject;
		weak_ptr<IInteractObject> RawObject;
		bool EligibleClick;
		int x, y, deltaX, deltaY;
		int scrollDelta;

		MouseEventData();
	};

	interface IPointerHoverHandler
	{
		virtual void OnPointerEnter(MouseEventData evt) = 0;
		virtual void OnPointerExit(MouseEventData evt) = 0;
	};

	interface IPointerClickHandler
	{
		virtual void OnPointerClick(MouseEventData evt) = 0;
	};

	interface IScrollHandler
	{
		virtual void OnScroll(MouseEventData evt) = 0;
	};

	interface IDragHandler
	{
		virtual void OnDragBegin(MouseEventData evt) = 0;
		virtual void OnDrag(MouseEventData evt) = 0;
		virtual void OnDragEnd(MouseEventData evt) = 0;
	};

	interface InteractObjectCaster
	{
		virtual int GetPriority() = 0;
		virtual weak_ptr<IInteractObject> Cast(int screenX,int screenY) = 0;

		bool operator<(InteractObjectCaster& other)
		{
			return GetPriority() < other.GetPriority();
		}
	};

	class EventSystem
	{
	public:
		EventSystem();
		void Update();
		void AddCaster(shared_ptr<InteractObjectCaster> caster);
		void RemoveCaster(shared_ptr<InteractObjectCaster> caster);
	private:

		weak_ptr<IInteractObject> _curr;
		list<shared_ptr<InteractObjectCaster>> _casters;

		MouseEventData _leftEvt;
		MouseEventData _rightEvt;
		MouseEventData _middleEvt;

		int _preX, _preY, _preScroll;

		void UpdateMouseState();
		void UpdateEvent(MouseEventData& evt, int x, int y, int dx, int dy, int scroll, bool down);

		void ProcessMousePress(MouseEventData& evt);
		void ProcessMove(MouseEventData& evt);
		void ProcessDrag(MouseEventData& evt);
		void ProcessWheel(MouseEventData& evt);
	};
}