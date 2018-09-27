#ifndef TRIANGLEH
#define TRIANGLEH

#include "hitable.h"

// derivates from hitable
class triangle : public hitable {
public:
	triangle() {}

	triangle(vec3 aa, vec3 bb, vec3 cc, material *m) : a(aa), b(bb), c(cc), mat_ptr(m) {};

	virtual bool hit(const ray& r, float tmin, float tman, hit_record& rec) const;

	vec3 a, b, c;
	material *mat_ptr;
};

bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	// edge vectors
	vec3 e1 = b - a;
	vec3 e2 = c - a;

	// face normal
	vec3 n = cross(e1, e2); // cross(v1, v2) is orthogonal to the triangle plane
	//n.make_unit_vector(); // TODO: conferir se é unit vector

	vec3 q = cross(r.direction(), e2);
	float aNum = dot(e1, q);

	// Backfacing / nearly parallel, or close to the limit of precision ?
	if ((dot(n, r.direction()) >= 0) || (abs(aNum) <= (1e-7))) { // TODO: abs works?
		return false;
	}

	vec3 s = (r.origin() - a) / aNum;
	vec3 rVec = cross(s, e1);

	float bar0 = dot(s, q);
	float bar1 = dot(rVec, r.direction());
	float bar2 = 1.0 - bar0 - bar1;

	// 'intersected outside triangle'
	if ((bar0 < 0.0) || (bar1 < 0.0) || (bar2 < 0.0))
		return false;

	// valid intersection
	float temp = dot(e2, rVec);
	if (temp < t_max && temp > t_min) {
		rec.t = temp;
		rec.p = r.point_at_parameter(rec.t);
		rec.normal = n;
		rec.mat_ptr = mat_ptr;
		return true;
	}
	
	return false;

}

#endif // ! TRIANGLEH
