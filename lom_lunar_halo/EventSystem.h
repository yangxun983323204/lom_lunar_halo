#pragma once
#include <basetyps.h>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::weak_ptr;
using std::vector;

namespace YX
{
	interface IPointerEnterHandler
	{
		virtual void OnPointerEnter() = 0;
	};

	interface IPointerExitHandler
	{
		virtual void OnPointerExit() = 0;
	};

	interface IPointerClickHandler
	{
		virtual void OnPointerClick() = 0;
	};

	interface IScrollHandler
	{
		virtual void OnScroll(float dx,float dy) = 0;
	};

	interface IInteractObject
	{
		virtual bool CanHandlePointerEnter() = 0;
		virtual bool CanHandlePointerExit() = 0;
		virtual bool CanHandlePointerClick() = 0;
		virtual bool CanHandleScroll() = 0;
		virtual IInteractObject* GetParent() = 0;
	};

	interface InteractObjectCaster
	{
		virtual int GetPriority() = 0;
		virtual weak_ptr<IInteractObject> Cast(int screenX,int screenY) = 0;
	};

	class EventSystem
	{
	public:
		void Update();
		void AddCaster(InteractObjectCaster* caster);
		void RemoveCaster(InteractObjectCaster* caster);
	private:

		weak_ptr<IInteractObject> _curr;
		vector<InteractObjectCaster*> _caster;
	};
}