#pragma once

#include <common.h>

template <u32 sh>
struct fixfp3 {
	using fpt = fixfp<sh>;
	union __attribute__((may_alias)) {
		fpt v[3];
		struct {
			fpt x, y, z;
		};
	};
	inline constexpr fixfp3<sh>() = default;
	inline constexpr fixfp3<sh>(fpt x_, fpt y_, fpt z_)
	    : x(x_), y(y_), z(z_)
	{
	}
};

template <u32 sh>
inline fixfp3<sh> operator+(fixfp3<sh> const &a, fixfp3<sh> const &b)
{
	return fixfp3(a.x + b.x, a.y + b.y, a.z + b.z);
}

template <u32 sh>
inline fixfp3<sh> operator-(fixfp3<sh> const &a, fixfp3<sh> const &b)
{
	return fixfp3(a.x - b.x, a.y - b.y, a.z - b.z);
}

template <u32 sh>
inline fixfp3<sh> operator*(fixfp3<sh> const &a, fixfp3<sh> const &b)
{
	return fixfp3(a.x * b.x, a.y * b.y, a.z * b.z);
}

template <u32 sh>
fixfp3<sh> operator*(fixfp<sh> f, fixfp3<sh> const &a)
{
	return fixfp3(f * a.x, f * a.y, f * a.z);
}

template <u32 sh>
fixfp<sh> dot(fixfp3<sh> const &a, fixfp3<sh> const &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <u32 sh>
fixfp3<sh> cross(fixfp3<sh> const &a, fixfp3<sh> const &b)
{
	return fixfp3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
		      a.x * b.y - a.y * b.x);
}

template <u32 sh>
inline fixfp<sh> len(fixfp3<sh> const &a)
{
	return sqrt(dot(a, a));
}

template <u32 sh>
inline fixfp<sh> rlen(fixfp3<sh> const &a)
{
	return rsqrt(dot(a, a));
}

template <u32 sh>
fixfp3<sh> normalize(fixfp3<sh> const &a)
{
	assert(len(a).v);
	return rlen(a) * a;
}

template <u32 sh>
inline fixfp3<sh> reflect(fixfp3<sh> const &d, fixfp3<sh> const &n)
{
	return d - fixfp<sh>(u32(2)) * dot(d, n) * n;
}

template <u32 sh>
inline constexpr fixfp3<sh> from_rgb5(u32 rgb5)
{
	auto c = fixfp3<sh>(fixfp<sh>(rgb5), fixfp<sh>(rgb5 >> 5),
			    fixfp<sh>(rgb5 >> 10));
	c.x.v >>= 5;
	c.y.v >>= 5;
	c.z.v >>= 5; // TODO: single shifts
	return c;
}

template <u32 sh>
inline constexpr u32 to_rgb5(fixfp3<sh> clr)
{
	clr.x.v <<= 5;
	clr.y.v <<= 5;
	clr.z.v <<= 5;
	return u32(clr.x) | (u32(clr.y) << 5) | (u32(clr.z) << 10);
}
