#include "EventSystem.h"

using namespace YX;

void EventSystem::Update()
{
	int screenX = 0, screenY = 0;
	// todo �жϵ�ǰ�¼�����
	for (auto i = _caster.begin(); i != _caster.end(); i++)
	{
		auto pObj = (*i)->Cast(screenX, screenY);
		if (!pObj.expired())
		{
			// todo �¼�����
			_curr = pObj;
			break;
		}
	}
}