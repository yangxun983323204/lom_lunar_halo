#pragma once
#include <vector>
#include <memory>
#include "Sprite.hpp"

class Animation
{
public:
	Animation():
		_frames{}, _spanMs{300}, Loop{true},
		_frameChanged{ true }, _idx{0}
	{}
	inline void SetSpan(uint16_t ms) { _spanMs = ms; }
	inline uint16_t GetSpan() { return _spanMs; }
	inline void AddFrame(std::weak_ptr<Sprite> frame) { _frames.push_back(frame); }
	inline void ClearFrame() { _frames.clear(); }
	void Update(uint32_t totalMs);
	inline bool FrameChanged() { return _frameChanged; }
	inline void ResetFlag() { _frameChanged = false; }
	inline std::weak_ptr<Sprite> GetCurrentFrame() { return _frames[_idx]; }

	bool Loop;
private:
	std::vector<std::weak_ptr<Sprite>> _frames;
	uint16_t _spanMs;
	bool _frameChanged;
	uint16_t _idx;
};

