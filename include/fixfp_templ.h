#pragma once

#include <common.h>
#include <tuple>
#include <utility>

template <u32 sh>
struct fixfp {
	static inline constexpr fixfp makeraw(s32 v)
	{
		fixfp res((s32)0);
		res.v = v;
		return res;
	}

	constexpr static u32 scale = (u32)1 << sh;
	constexpr static u32 fp_mask = scale - 1;
	constexpr static s32 max_int = (s32)INT32_MAX >> sh;
	constexpr static s32 min_int = (s32)INT32_MIN >> sh;
	constexpr static fixfp<sh> max = makeraw((s32)INT32_MAX);
	constexpr static fixfp<sh> min = makeraw((s32)INT32_MIN);
	constexpr static fixfp<sh> null = makeraw((s32)0);
	constexpr static fixfp<sh> one = makeraw((s32)1);

	inline constexpr fixfp() = default;

	inline explicit constexpr fixfp(s32 x_) : v(x_ << sh){};
	inline explicit constexpr fixfp(u32 x_) : v(x_ << sh){};

	inline explicit constexpr fixfp(float x_) : v((s32)(x_ * scale)){};

	/********************************************************/
	/*	DANGEROUS: Inconsistent negative rounding!!!	*/
	/********************************************************/
	inline explicit constexpr operator s32() const { return v >> sh; }
	inline explicit constexpr operator u32() const { return v >> sh; }
	/*********************************************************/
	inline explicit constexpr operator float() const
	{
		return v / (float)scale;
	}
	s32 v;
};

template <u32 sh>
inline constexpr fixfp<sh> operator+(fixfp<sh> a, fixfp<sh> b)
{
	return fixfp<sh>::makeraw(a.v + b.v);
}

template <u32 sh>
inline constexpr fixfp<sh> operator-(fixfp<sh> a, fixfp<sh> b)
{
	return fixfp<sh>::makeraw(a.v - b.v);
}

template <u32 sh>
inline constexpr fixfp<sh> operator-(fixfp<sh> a)
{
	return fixfp<sh>::makeraw(-a.v);
}

template <u32 sh>
inline constexpr bool operator>(fixfp<sh> a, fixfp<sh> b)
{
	return a.v > b.v;
}

template <u32 sh>
inline constexpr bool operator<(fixfp<sh> a, fixfp<sh> b)
{
	return a.v < b.v;
}

template <u32 sh>
inline constexpr fixfp<sh> operator*(fixfp<sh> a, fixfp<sh> b)
{
	s64 ext = (s64)a.v * b.v;
	ext = (u64)ext >> sh;
	return fixfp<sh>::makeraw((s32)ext);
}

template <u32 sh>
inline constexpr fixfp<sh> pow2(fixfp<sh> a)
{
	return a * a;
}

template <u32 sh>
inline constexpr fixfp<sh> abs(fixfp<sh> a)
{
	a.v = a.v > 0 ? a.v : -a.v;
	return a;
}

// shift raw value as unsigned
template <u32 sh>
inline constexpr fixfp<sh> ushift(fixfp<sh> a, s32 s)
{
	a.v = s > 0 ? (u32)a.v << s : (u32)a.v >> (-s);
	return a;
}

// extract sign and abs
template <u32 sh>
inline constexpr std::pair<fixfp<sh>, s32> extract_sign(fixfp<sh> a)
{
	s32 sgn = 1;
	if (a.v < 0) {
		sgn = -1;
		a.v = -a.v;
	}
	return std::make_pair(a, sgn);
}

// return e : fpt = n * e^2, n ~ [0.5, 1)
// works only with unsigned fpt
template <u32 sh>
inline constexpr s32 ugetpow(fixfp<sh> a)
{
	return (s32)32 - __builtin_clz(a.v) - sh;
}

/******************************************************************************/
/* Inversion and division */

template <u32 sh>
inline constexpr fixfp<sh> __nbinv(fixfp<sh> a)
{
	assert(a.v);
	s32 sgn;
	std::tie(a, sgn) = extract_sign(a);
	s32 shift = -ugetpow(a);
	a = ushift(a, shift); // unsigned and normalized

	constexpr fixfp<sh> c1 = fixfp<sh>(float(48.0 / 17.0));
	constexpr fixfp<sh> c2 = fixfp<sh>(float(-32.0 / 17.0));
	constexpr fixfp<sh> two = fixfp<sh>(s32(2));

	fixfp<sh> x = c1 + c2 * a;
	for (int i = 0; i < 1; ++i)
		x = x * (two - a * x);

	x = ushift(x, shift);
	x.v *= sgn;
	return x;
}

#ifdef CONF_FIXFPT_NBINV
template <u32 sh>
constexpr fixfp<sh> operator/(fixfp<sh> a, fixfp<sh> b)
{
	assert(b.v);
	return a * __nbinv<sh>(b);
}

template <u32 sh>
constexpr fixfp<sh> inv(fixfp<sh> a)
{
	assert(a.v);
	return __nbinv<sh>(a);
}
#else
template <u32 sh>
inline constexpr fixfp<sh> operator/(fixfp<sh> a, fixfp<sh> b)
{
	assert(b.v);
	s64 ext = (s64)a.v << sh;
	return fixfp<sh>::makeraw((s32)(ext / b.v));
}

template <u32 sh>
inline constexpr fixfp<sh> inv(fixfp<sh> a)
{
	assert(a.v);
	return fixfp<sh>((s32)1) / a;
}
#endif
/******************************************************************************/
/* Square roots */

#ifdef CONF_FIXFPT_FASTSQRT
template <u32 sh>
inline constexpr fixfp<sh> __nbrsqrt(fixfp<sh> a)
{
	union __attribute__((may_alias)) fprepr {
		struct __attribute__((packed)) {
			u32 m : sh;
			u32 expb : 32 - sh;
		};
		u32 raw;
	};
	constexpr fixfp<sh> A =
	    fixfp<sh>(float(-3.0 / 2.0 * (0.0430357 - float(sh))));
	constexpr fixfp<sh> th = fixfp<sh>(float(3.0 / 2.0));

	assert(a.v > 0);

	s32 exp = ugetpow(a) - 1;
	fixfp<sh> x = ushift(a, -exp); // normalize to [1, 2)
	fprepr fpr{{.m = (u32)x.v, .expb = exp + sh}};

	fpr.raw = A.v - (fpr.raw >> 1); // make approx

	x.v = fpr.m | ((u32)1 << sh);
	x = ushift(x, fpr.expb - sh); // get approx

	a.v = (u32)a.v >> 1;
	a.v = -a.v;
	x = x * (th + a * x * x); // NR step
	// x = x * (th + a * x * x);
	return x;
}

template <u32 sh>
constexpr fixfp<sh> rsqrt(fixfp<sh> a)
{
	assert(a.v > 0);
	return __nbrsqrt(a);
}

template <u32 sh>
constexpr fixfp<sh> sqrt(fixfp<sh> a)
{
	assert(a.v > 0);
	return a * __nbrsqrt(a);
}
#else
static inline constexpr u16 fastsqrtu32(u32 x)
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
inline constexpr fixfp<sh> sqrt(fixfp<sh> a)
{
	constexpr u32 sh_half = sh / 2;
	static_assert(sh_half * 2 == sh);
	return fixfp<sh>::makeraw((u32)fastsqrtu32(a.v) << sh_half);
}

template <u32 sh>
inline constexpr fixfp<sh> rsqrt(fixfp<sh> a)
{
	return inv(sqrt(a));
}

#endif
