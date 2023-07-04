#include "Animation.h"

void Animation::Update(uint32_t totalMs)
{
	auto n = totalMs / _spanMs;
	auto i = n % _frames.size();
	if (i!=_idx)
	{
		_frameChanged = true;
		_idx = i;
	}
}
