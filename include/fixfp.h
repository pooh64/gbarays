#ifndef FIXFP_H_
#define FIXFP_H_

extern "C" {
#include <gba_types.h>
};

#include <common.h>

typedef uint64_t u64;
typedef int64_t s64;

template <u32 sh> struct fixfp {
	static inline constexpr fixfp fixfpraw(s32 v)
	{
		fixfp res((s32)0);
		res.v = v;
		return res;
	}

	constexpr static u32 scale = (u32)1 << sh;
	constexpr static s32 max_int = (u32)INT32_MAX >> sh;
	constexpr static s32 min_int = (u32)INT32_MIN >> sh;
	constexpr static fixfp max = fixfpraw((s32)INT32_MAX);
	constexpr static fixfp min = fixfpraw((s32)INT32_MIN);

	fixfp() = default;

	/********************************************************/
	/*	DANGEROUS: Inconsistent negative rounding!!!	*/
	/********************************************************/
	inline explicit constexpr fixfp(s32 x_) : v(x_ << sh){};
	inline explicit constexpr fixfp(u32 x_) : v(x_ << sh){};
	/*********************************************************/

	inline explicit constexpr fixfp(float x_) : v((s32)(x_ * scale)){};

	inline explicit constexpr operator s32() const
	{
		return v >> sh;
	}
	inline explicit constexpr operator u32() const
	{
		return v >> sh;
	}
	inline explicit constexpr operator float() const
	{
		return v / (float)scale;
	}
	s32 v;
};

template <u32 sh>
inline fixfp<sh> operator+(fixfp<sh> a, fixfp<sh> b)
{
	return fixfp<sh>::fixfpraw(a.v + b.v);
}

template <u32 sh>
inline fixfp<sh> operator-(fixfp<sh> a, fixfp<sh> b)
{
	return fixfp<sh>::fixfpraw(a.v - b.v);
}

template <u32 sh>
inline fixfp<sh> operator-(fixfp<sh> a)
{
	return fixfp<sh>::fixfpraw(-a.v);
}

template <u32 sh>
bool operator>(fixfp<sh> a, fixfp<sh> b)
{
	return a.v > b.v;
}

template <u32 sh>
bool operator<(fixfp<sh> a, fixfp<sh> b)
{
	return a.v < b.v;
}

template <u32 sh>
inline fixfp<sh> operator*(fixfp<sh> a, fixfp<sh> b)
{
	s64 ext = (s64)a.v * b.v;
	ext = (u64)ext >> sh;
	return fixfp<sh>::fixfpraw((s32)ext);
}

template <u32 sh>
inline fixfp<sh> operator/(fixfp<sh> a, fixfp<sh> b)
{
	assert(b.v);
	s64 ext = (s64)a.v << sh;
	return fixfp<sh>::fixfpraw((s32)(ext / b.v));
}

template <u32 sh>
inline fixfp<sh> inv(fixfp<sh> a)
{
	assert(a.v);
	return fixfp<sh>((s32)1) / a;
}

template <u32 sh>
inline fixfp<sh> pow2(fixfp<sh> a)
{
	return a * a;
}

static inline u16 fastsqrtu32(u32 x)
{
	u16 r = 0, add = 0x8000u;
	for (int i = 0; i < 16; ++i) {
		u16 tmp = r | add;
		if (x >= (u32)tmp * (u32)tmp)
			r = tmp;
		add >>= 1;
	}
	return r;
}

template <u32 sh>
inline fixfp<sh> sqrt(fixfp<sh> a)
{
	constexpr u32 sh_half = sh / 2;
	static_assert(sh_half * 2 == sh);
	return fixfp<sh>::fixfpraw((u32)fastsqrtu32(a.v) << sh_half);
}

#endif