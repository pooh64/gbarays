#include <sphere.h>

bool Sphere::Hit(Ray const &ray, fpt tmin, fpt tmax, RayHit &hit) const
{
	fpt3 tmp = ray.o - o;
	fpt a = dot(ray.d, ray.d);
	fpt b = fpt((s32) 2) * dot(ray.d, tmp);
	fpt c = dot(tmp, tmp) - r * r;
	fpt discr = b * b - fpt((s32) 4) * a * c;
	if (discr < fpt((s32) 0))
		return false;
	discr = sqrt(discr);
	fpt t = (-b - discr) / (fpt((s32) 2) * a);
	if (t < tmin)
		t = (-b + discr) / (fpt((s32) 2) * a);
	if (t < tmin || t > tmax)
		return false;
	hit.t = t;
	hit.n = normalize(ray.PointAt(t) - o);
	return true;
}