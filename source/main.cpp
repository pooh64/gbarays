extern "C" {
#include <gba_video.h>
#include <gba_console.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
};

#include <fixfp.h>
#include <fixfpvec.h>
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

static inline void sphere_fire(Camera const &cam, Sphere const &sph, u32 x, u32 y)
{
	RayHit hit;
	fpt fx = fpt((s32) x - SCREEN_WIDTH  / 2) / fpt((u32) SCREEN_WIDTH);
	fpt fy = fpt((s32) y - SCREEN_HEIGHT / 2) / fpt((u32) SCREEN_HEIGHT);
	Ray ray = cam.Cast(fx, fy);
	bool is_hit = sph.Hit(ray, fpt::min, fpt::max, hit);
	u16 clr;
	if (is_hit)
		clr = RGB5(30, 30, 30);
	else
	clr = RGB5(10, 10, 0);
	MODE3_FB[y][x] = clr;
}

void demo_rt_sphere_hit()
{
	constexpr float ratio = (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT;
	constexpr float fov = 3.14 / 4.0;

	fpt3 eye(fpt(float(0)), fpt(float(0)), fpt(float(-1.0)));
	fpt3  at(fpt(float(0)), fpt(float(0)), fpt(float(0)));
	fpt3  up(fpt(float(0)), fpt(float(1)), fpt(float(0)));

	Camera cam;
	cam.SetScreen(fpt(fov), fpt(ratio));
	cam.LookAt(eye, at, up);

	Sphere sph;
	sph.o = fpt3(fpt(float(0)), fpt(float(0)), fpt(float(0)));
	sph.r = fpt(float(0.3));

	static constexpr u32 pix_step = 1;

#if 0
	for (u32 y = 0; y < SCREEN_HEIGHT; y += pix_step) {
		for (u32 x = 0; x < SCREEN_WIDTH; x += pix_step) {
			sphere_fire(cam, sph, x, y);
		}
	}
#else
	while (1) {
		u32 x = rand() % SCREEN_WIDTH;
		u32 y = rand() % SCREEN_HEIGHT;
		sphere_fire(cam, sph, x, y);
	}
#endif

	while(1);
}

int main()
{
	REG_DISPCNT = MODE_3 | BG2_ON;
	gbarays_tests();
	demo_rt_sphere_hit();

	while (1)
		;
}