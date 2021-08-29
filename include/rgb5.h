#pragma once

#include <common.h>

// TODO: adapt to fpt3

union __attribute__((may_alias)) rgb5 {
	struct __attribute__((packed)) {
		u32 r : 5;
		u32 g : 5;
		u32 b : 5;
	};
	u16 raw;

	inline rgb5() = default;
	inline constexpr rgb5(u32 r_, u32 g_, u32 b_) : r(r_), g(g_), b(b_) {}
};

inline rgb5 mix(rgb5 x, rgb5 y)
{
	return rgb5(((u32)x.r + y.r) >> 1, ((u32)x.g + y.g) >> 1,
		    ((u32)x.b + y.b) >> 1);
}
