#ifndef METALH
#define METALH 

#include "material.h"
#include "texture.h"

class metal : public material {
public:
	metal(texture *a, float f) : albedo(a) {
		if (f < 1) {
			fuzz = f;
		}
		else {
			fuzz = 1;
		}
	}

	// light that reflects off a diffuse surface has its direction randomized
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time()); // resultant randomized ray
		attenuation = albedo->value(rec.u, rec.v, rec.p); // attenuation, 'measure of the diffuse reflection', color dependent

		return (dot(scattered.direction(), rec.normal) > 0);
	}

	texture *albedo; // 'measure of the diffuse reflection'
	float fuzz;
};

#endif