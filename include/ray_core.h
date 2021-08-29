#pragma once

#include <fixfp_types.h>
#include <rgb5.h>

struct Ray {
	fpt3 o, d;

	inline fpt3 PointAt(fpt t) const
	{
		return o + t * d;
	}
};

struct RayHit {
	fpt3 n;
	fpt t;
	fpt3 color;
};

static inline Ray MirrorReflect(RayHit const &h, Ray const &r)
{
	Ray refl;
	refl.o = r.o + h.t * r.d;
	refl.d = reflect(r.d, h.n);
	return refl;
}

struct Shape {
	virtual bool Hit(Ray const &r, fpt tmin, fpt tmax,
			 RayHit &hit) const = 0;
};

struct Camera {
	fpt3 pos, dir, axis_x, axis_y;
	fpt ratio; // res_x / res_y
	fpt fov; // axis_y in rad

	void SetScreen(fpt fov_, fpt ratio_)
	{
		fov = fov_;
		ratio = ratio_;
	}

	void LookAt(fpt3 const &eye, fpt3 const &at, fpt3 const &up)
	{
		pos = eye;
		auto v = at - eye;
		dir = normalize(at - eye);
		v = cross(dir, up);
		axis_x = normalize(cross(dir, up));
		axis_y = cross(axis_x, dir);

		axis_x = (ratio * fov) * axis_x;
		axis_y = fov * axis_y;
	}

	Ray Cast(fpt x, fpt y) const
	{
		return Ray{ .o = pos, .d = x * axis_x + y * axis_y + dir };
	}
};
