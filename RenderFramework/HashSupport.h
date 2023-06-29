#pragma once
#include <memory>
#include <DirectXMath.h>

namespace std {
	template<>
	class hash<DirectX::XMUINT2>
	{
	public:
		size_t operator()(const DirectX::XMUINT2& sk) const
		{
			return hash<uint32_t>()(sk.x) ^ hash<uint32_t>()(sk.y);
		}
	};

	template<>
	struct equal_to<DirectX::XMUINT2>
	{
	public:
		bool operator()(const DirectX::XMUINT2& k1, const DirectX::XMUINT2& k2) const
		{
			return k1.x == k2.x && k1.y == k2.y;
		}

	};
};