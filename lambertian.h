#ifndef LAMBERTIANH
#define LAMBERTIANH 

#include "material.h"
#include "texture.h"

class lambertian : public material {
public:
	lambertian(texture *a) : albedo(a) {}

	// light that reflects off a diffuse surface has its direction randomized
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p, r_in.time()); // resultant randomized ray
		attenuation = albedo->value(rec.u, rec.v, rec.p); // attenuation, 'measure of the diffuse reflection', color dependent

		return true;
	}

	texture *albedo; // 'measure of the diffuse reflection'

};

#endif