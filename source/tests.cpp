#include <fixfp.h>
#include <tests.h>
#include <ray_core.h>

#ifdef NDEBUG
#pragma message("NDEBUG: assert DISABLED!")
#endif

static inline float absf(float a)
{
	return a > 0 ? a : -a;
}

static bool iseqfpt(float a, float b)
{
	return absf(a - b) < 0.01 * absf(a + b);
}

static void fixfpt_test_casts()
{
#define verify_cast(type, val) assert(type(fpt(type(val))) == type(val))
	verify_cast(s32, 0);
	verify_cast(s32, 1);
	verify_cast(s32, 228);
	verify_cast(u32, -42);
	verify_cast(u32, 0x505);
#undef verify_cast
#define verify_cast(type, val) assert(iseqfpt(type(fpt(type(val))), type(val)))
	verify_cast(float, 0.1);
	verify_cast(float, 128.11);
	verify_cast(float, -32.43);
#undef verify_cast
}

static void fixfpt_test_arithm()
{
	s32 as_arr[] = { 123, -3, 17, -66 };
	s32 bs_arr[] = { 66, 92, -33, -19 };
	float af_arr[] = { 23.2, -3.142, 0.72, -9.81 };
	float bf_arr[] = { 6.66, 0.228, -0.123, -24.0 };

	for (u32 i = 0; i < ARRAY_SIZE(as_arr); ++i) {
		s32 as = as_arr[i], bs = bs_arr[i];
		float af = af_arr[i], bf = bf_arr[i];
		(void) as, (void) bs, (void) af, (void) bf, (void) iseqfpt;
#define verify_arithm(type, expr, val) assert(type(expr) == type(val))
		verify_arithm(s32, fpt(as) + fpt(bs), as + bs);
		verify_arithm(s32, fpt(as) - fpt(bs), as - bs);
		verify_arithm(s32, fpt(as) * fpt(bs), as * bs);
		verify_arithm(s32, float(fpt(as) / fpt(bs)),
			      (float)as / (float)bs);
#undef verify_arithm
#define verify_arithm(type, expr, val) assert(iseqfpt(type(expr), type(val)))
		verify_arithm(float, fpt(af) + fpt(bf), af + bf);
		verify_arithm(float, fpt(af) - fpt(bf), af - bf);
		verify_arithm(float, fpt(af) * fpt(bf), af * bf);
		verify_arithm(float, fpt(af) / fpt(bf), af / bf);
		verify_arithm(float, pow2(fpt(af)), af *af);
		verify_arithm(float, sqrt(pow2(fpt(af))), absf(af));
		verify_arithm(float, inv(fpt(af)), 1.0 / af);
#undef verify_arithm
	}
}

static void fixfpt_test_const()
{
	assert(fpt::max > fpt(s32(3)));
	assert(fpt::max > fpt(s32(-3)));
	assert(fpt::min < fpt(s32(3)));
	assert(fpt::max > fpt(s32(-3)));
}

void gbarays_tests()
{
	fixfpt_test_casts();
	fixfpt_test_arithm();
	fixfpt_test_const();
	//assert(!"tests passed :)");
}