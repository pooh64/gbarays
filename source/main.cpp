extern "C" {
#include <gba_console.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_video.h>
};

#include <cstdlib>
#include <cstring>

#include <ray_core.h>
#include <sphere.h>
#include <tests.h>

void demo_sqrt()
{
	for (u32 x = 0; x < SCREEN_WIDTH; x++) {
		auto f = fpt(x << 6);
		f = sqrt(f);
		u32 y = SCREEN_HEIGHT - s32(f);
		MODE3_FB[y][x] = RGB5(31, 31, 31);
	}

	while (1)
		;
}

static inline void sphere_fire(Camera const &cam, Sphere const *sph, u32 nsph,
			       u32 x, u32 y)
{
	fpt fx = fpt((s32)x - SCREEN_WIDTH / 2);
	fpt fd(u32(rand() % 128));
	fd.v = fd.v >> 7;
	fx = (fx + fd) / fpt(u32(SCREEN_WIDTH));

	fpt fy = fpt((s32)y - SCREEN_HEIGHT / 2);
	fd = fpt(u32(rand() % 128));
	fd.v = fd.v >> 7;
	fy = (fy + fd) / fpt(u32(SCREEN_HEIGHT));

	Ray ray = cam.Cast(fx, fy);

	RayHit hit;
	fpt3 rayclr(fpt(u32(1)), fpt(u32(1)), fpt(u32(1)));
	constexpr fpt3 bgclr(fpt(float(0.5)), fpt(float(0.5)), fpt(float(0.5)));
recast:
	for (u32 i = 0;; ++i) {
		bool is_hit = sph[i].Hit(ray, fpt(float(0.01)), fpt::max, hit);
		if (is_hit) {
			rayclr = rayclr * hit.color;
			// break;
			ray = MirrorReflect(hit, ray);
			goto recast;
		}
		if (i == nsph - 1) {
			rayclr = rayclr * bgclr;
			break;
		}
	}
	rgb5 curclr, newclr;
	curclr.raw = MODE3_FB[y][x];
	newclr.raw = to_rgb5(rayclr);
	MODE3_FB[y][x] = mix(curclr, newclr).raw;
}

void clearScreen()
{
	memset(MODE3_FB, 0, sizeof(**MODE3_FB) * SCREEN_WIDTH * SCREEN_HEIGHT);
}

void demo_rt_sphere_hit()
{
	constexpr float ratio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	constexpr float fov = 3.14 / 4.0;

	fpt3 eye(fpt(float(0)), fpt(float(0)), fpt(float(-1.0)));
	fpt3 at(fpt(float(0)), fpt(float(0)), fpt(float(0)));
	fpt3 up(fpt(float(0)), fpt(float(1)), fpt(float(0)));

	Camera cam;
	cam.SetScreen(fpt(fov), fpt(ratio));
	cam.LookAt(eye, at, up);

	Sphere sph[3];
	sph[0].o = fpt3(fpt(float(0)), fpt(float(-0.2)), fpt(float(0)));
	sph[0].r = fpt(float(0.2));
	sph[0].color = from_rgb5<FPT_SHIFT>((u32)RGB5(10, 31, 10));

	sph[1].o = fpt3(fpt(float(0.4)), fpt(float(0.2)), fpt(float(0)));
	sph[1].r = fpt(float(0.3));
	sph[1].color = from_rgb5<FPT_SHIFT>((u32)RGB5(31, 10, 10));

	sph[2].o = fpt3(fpt(float(-0.3)), fpt(float(0.1)), fpt(float(0)));
	sph[2].r = fpt(float(0.15));
	sph[2].color = from_rgb5<FPT_SHIFT>((u32)RGB5(10, 10, 31));

	static constexpr u32 pix_step = 1;

#if 1
	while (1) {
		for (u32 y = 0; y < SCREEN_HEIGHT; y += pix_step) {
			for (u32 x = 0; x < SCREEN_WIDTH; x += pix_step) {
				sphere_fire(cam, sph, ARRAY_SIZE(sph), x, y);
			}
		}
	}
#else
	while (1) {
		scanKeys();
		u16 kd = keysHeld();
		if (kd & KEY_A)
			eye = eye + fpt3(fpt(float(0)), fpt(float(0)),
					 fpt(float(0.01)));
		if (kd & KEY_B)
			eye = eye + fpt3(fpt(float(0)), fpt(float(0)),
					 fpt(float(-0.01)));
		if (kd & KEY_LEFT)
			at = at + fpt3(fpt(float(0.02)), fpt(float(0)),
				       fpt(float(0.0)));
		if (kd & KEY_RIGHT)
			at = at + fpt3(fpt(float(-0.02)), fpt(float(0)),
				       fpt(float(0.0)));
		if (kd) {
			cam.LookAt(eye, at, up);
			clearScreen();
		}

		for (s32 i = 0; i < 50; ++i) {
			u32 x = rand() % SCREEN_WIDTH;
			u32 y = rand() % SCREEN_HEIGHT;
			sphere_fire(cam, sph, x, y);
		}
	}
#endif

	while (1)
		;
}

/*
	Vanilla: 20.8s
	Noinline-vec3: 19.3
	CONF_FIXFPT_NBINV: 13.8
	#define CONF_FIXFPT_FASTSQRT: 13.0
*/

int main()
{
	REG_DISPCNT = MODE_3 | BG2_ON;
	gbarays_tests();
	demo_rt_sphere_hit();

	while (1)
		;
}