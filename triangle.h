#ifndef TRIANGLEH
#define TRIANGLEH

#include "hitable.h"

// derivates from hitable
class triangle : public hitable {
public:
	triangle() {}

	triangle(vec3& aa, vec3& bb, vec3& cc, vec3& nn, material *m, float s=1.0) : a(aa), b(bb), c(cc), normal(nn), mat_ptr(m), scale(s) {
		// edge vectors
		e1 = b - a;
		e2 = c - a;

		d = dot(normal, a);
	};

	virtual bool hit(const ray& r, float tmin, float t_max, hit_record&) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

	vec3 a, b, c;
	vec3 e1, e2;
	vec3 normal; // face/plane normal
	float d;
	float scale;
	material *mat_ptr;
};

bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	vec3 q = cross(r.direction(), e2);
	float aNum = dot(e1, q);

	// Backfacing / nearly parallel, or close to the limit of precision ?
	if (((abs(dot(normal, r.direction()))) < (1e-7)) || (abs(aNum) <= (1e-7))) { // two sided triangle
		return false;
	}

	vec3 s = (r.origin() - a) / aNum;
	vec3 rVec = cross(s, e1);

	// 'intersected outside triangle'
	float barB = dot(s, q);
	if (barB < 0.0)
		return false;
	
	float barC = dot(rVec, r.direction());
	if (barC < 0.0)
		return false;

	float barA = 1.0 - barB - barC;
	if (barA < 0.0)
		return false;

	// valid intersection
	float temp = dot(e2, rVec);
	if (temp < t_max && temp > t_min) {
		rec.u = (a.u() * barA + b.u() * barB + c.u() * barC);
		rec.v = (a.v() * barA + b.v() * barB + c.v() * barC);
		rec.t = temp;
		rec.p = r.point_at_parameter(rec.t);
		rec.normal = normal;
		rec.mat_ptr = mat_ptr;
		return true;
	}
	
	return false;

}

bool triangle::bounding_box(float t0, float t1, aabb& box) const{
	// find min and max iterating through vertices
	// min(minX, minY, minZ)
	float minX = ffmin(ffmin(a.x(), b.x()), c.x());
	float minY = ffmin(ffmin(a.y(), b.y()), c.y());
	float minZ = ffmin(ffmin(a.z(), b.z()), c.z());
	
	// max(maxX, maxY, maxZ)
	float maxX = ffmax(ffmax(a.x(), b.x()), c.x());
	float maxY = ffmax(ffmax(a.y(), b.y()), c.y());
	float maxZ = ffmax(ffmax(a.z(), b.z()), c.z());

	box = aabb((vec3(minX, minY, minZ)), (vec3(maxX, maxY, maxZ)));

	return true;
}

#endif // ! TRIANGLEH
