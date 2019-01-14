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

		normal = cross(e1, e2);
		normal = unit_vector(normal);

		d = dot(normal, a);
	};

	virtual bool hit(const ray& r, float tmin, float t_max, hit_record&) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	
	virtual float pdf_value(const vec3& o, const vec3& v) const {
		hit_record rec;

		if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
			float area = rec.normal.length() * 0.5;
			float distance_squared = rec.t * rec.t * v.squared_length();
			float cosine = fabs(dot(v, rec.normal) / v.length());

			return distance_squared / (cosine * area);
		}
		else
			return 0;
	}

	virtual vec3 random(const vec3& o) const;

	vec3 a, b, c;
	vec3 e1, e2;
	vec3 normal; // face/plane normal
	float area;
	float d;
	float scale;
	material *mat_ptr;
};

bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	vec3 pvec = cross(r.direction(), e2);
	float aNum = dot(pvec, e1);

	// Backfacing / nearly parallel, or close to the limit of precision ?
	if (abs(aNum) < 1E-8)
		return false;

	vec3 tvec = r.origin() - a;
	float u = dot(pvec, tvec) / aNum;
	if (u < 0.0 || u > 1.0) 
		return false;

	vec3 qVec = cross(tvec, e1);
	float v = dot(qVec, r.direction()) / aNum;
	if (v < 0.0 || u + v > 1.0) 
		return false;

	float t = dot(qVec, e2) / aNum;
	if (t < t_min || t > t_max) 
		return false;

	// valid intersection
	rec.u = (a.u() * (1.0 - u - v) + b.u() * u + c.u() * v);
	rec.v = (a.v() * (1.0 - u - v) + b.v() * u + c.v() * v);
	rec.t = t;
	rec.p = r.point_at_parameter(rec.t);
	rec.normal = normal;
	rec.mat_ptr = mat_ptr;
	return true;
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

	box = aabb((vec3(minX - 0.0001, minY - 0.0001, minZ - 0.0001)), (vec3(maxX + 0.0001, maxY + 0.0001, maxZ + 0.0001)));

	return true;
}

vec3 triangle::random(const vec3& o) const {
	float r1 = RFG();
	float r2 = RFG();
	float sr1 = sqrt(r1);

	vec3 randomPoint((1.0 - sr1) * a + sr1 * (1.0 - r2) * b + sr1 * r2 * c);

	return randomPoint - o;
}

#endif // ! TRIANGLEH
