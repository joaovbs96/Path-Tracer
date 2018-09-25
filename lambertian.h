#ifndef LAMBERTIANH
#define LAMBERTIANH 

#include "material.h"

class lambertian : public material {
public:
	lambertian(const vec3& a) : albedo(a) {}

	// light that reflects off a diffuse surface has its direction randomized
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p); // resultant randomized ray
		attenuation = albedo; // attenuation, 'measure of the diffuse reflection'

		return true;
	}

	vec3 albedo;

};

#endif