#ifndef SPHERE_H_
#define SPHERE_H_

#include <ray_core.h>

struct Sphere : public Shape {
	fpt r;
	fpt3 o;
	virtual bool Hit(Ray const &r, fpt tmin, fpt tmax, RayHit &hit) const override;
};

#endif