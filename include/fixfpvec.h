#ifndef FIXFPVEC_H_
#define FIXFPVEC_H_

#include <fixfp.h>

template <u32 sh>
struct fixfp3 {
	using fpt = fixfp<sh>;
	union __attribute__((may_alias)) {
		fpt v[3];
		struct {
			fpt x, y, z;
		};
	};
	fixfp3<sh>() = default;
	fixfp3<sh>(fpt x_, fpt y_, fpt z_) : x(x_), y(y_), z(z_)
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
inline fixfp3<sh> operator*(fixfp<sh> f, fixfp3<sh> const &a)
{
	return fixfp3(f * a.x, f * a.y, f * a.z);
}

template <u32 sh>
inline fixfp<sh> dot(fixfp3<sh> const &a, fixfp3<sh> const &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <u32 sh>
inline fixfp3<sh> cross(fixfp3<sh> const &a, fixfp3<sh> const &b)
{
	return fixfp3<sh>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
			  a.x * b.y - a.y * b.x);
}

template <u32 sh>
inline fixfp<sh> len(fixfp3<sh> const &a)
{
	return sqrt(dot(a, a));
}

template <u32 sh>
inline fixfp3<sh> normalize(fixfp3<sh> const &a)
{
	assert(len(a).v);
	return inv(len(a)) * a;
}

#endif