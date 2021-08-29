#pragma once

#include <ray_core.h>

struct Sphere : public Shape {
	fpt r;
	fpt3 o;
	fpt3 color;
	virtual bool Hit(Ray const &r, fpt tmin, fpt tmax,
			 RayHit &hit) const override;
};
