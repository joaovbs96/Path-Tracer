#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"

// derivates from hitable
class sphere : public hitable {
public:
	sphere() {}

	sphere(vec3 cen, float r, material *m) : center(cen), radius(r), mat_ptr(m) {};

	virtual bool hit(const ray& r, float tmin, float tman, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	virtual float pdf_value(const vec3& o, const vec3& v) const;
	virtual vec3 random(const vec3& o) const;

	vec3 center;
	float radius;
	material *mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center;

	// if the ray hits the sphere, the following equation has two roots:
	// t²dot(B, B) + 2tdot(B,A-C) + dot(A-C,A-C) - R² = 0

	// Using Bhaskara's Formula, we have:
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;

	// if the discriminant is higher than 0, there's a hit.
	if (discriminant > 0) {
		// first root of the sphere equation:
		float temp = (-b - sqrt(b * b - a * c)) / a;

		// surface normals: vector perpendicular to the surface,
		// points out of it by convention.

		// for a sphere, its normal is in the (hitpoint - center).

		// if the first root was a hit,
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
			rec.normal = (rec.p - center) / radius; // get normal
			rec.mat_ptr = mat_ptr; // get material of hitpoint
			return true;
		}

		// second root of the sphere equation:
		temp = (-b + sqrt(b * b - a * c));

		// if the second root was a hit,
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}

	// otherwise, there's no hit.
	return false;
}

bool sphere::bounding_box(float t0, float t1, aabb& box) const {
	box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));

	return true;
}

float sphere::pdf_value(const vec3& o, const vec3& v) const {
	hit_record rec;

	if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
		float cos_theta_max = sqrt(1 - radius * radius / (center - o).squared_length());
		float solid_angle = 2 * M_PI * (1 - cos_theta_max);

		return 1 / solid_angle;
	}
	else
		return 0;
}

vec3 sphere::random(const vec3& o) const {
	vec3 direction = center - o;
	float distance_squared = direction.squared_length();
	onb uvw;
	uvw.build_from_w(direction);

	return uvw.local(random_to_sphere(radius, distance_squared));
}

#endif // ! SPHEREH
