#ifndef DIELECTRICH
#define DIELECTRICH 

#include "material.h"

class dielectric : public material {
public:
	dielectric(float ri) : ref_idx(ri) {}

	virtual bool scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec) const {
		srec.is_specular = true;
		srec.pdf_ptr = 0;
		srec.attenuation = vec3(1.0, 1.0, 1.0);

		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), hrec.normal);
		vec3 refracted;

		float ni_over_nt;
		float reflect_prob;
		float cosine;

		if (dot(r_in.direction(), hrec.normal) > 0) {
			outward_normal = -hrec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx * dot(r_in.direction(), hrec.normal) / r_in.direction().length();
		}
		else {
			outward_normal = hrec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), hrec.normal) / r_in.direction().length();
		}

		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
			reflect_prob = schlick(cosine, ref_idx);
		}
		else {
			reflect_prob = 1.0;
		}

		if (RFG() < reflect_prob) {
			srec.specular_ray = ray(hrec.p, reflected);
		}
		else {
			srec.specular_ray = ray(hrec.p, refracted);
		}

		return true;
	}

	float ref_idx;
};

#endif