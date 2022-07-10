#include "EventSystem.h"

using namespace YX;

void EventSystem::Update()
{
	int screenX = 0, screenY = 0;
	// todo 判断当前事件类型
	for (auto i = _caster.begin(); i != _caster.end(); i++)
	{
		auto pObj = (*i)->Cast(screenX, screenY);
		if (!pObj.expired())
		{
			// todo 事件处理
			_curr = pObj;
			break;
		}
	}
}