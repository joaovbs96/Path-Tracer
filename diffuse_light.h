#ifndef DIFFUSE_LIGHTH
#define DIFFUSE_LIGHTH

#include "material.h"
#include "texture.h"

class diffuse_light : public material {
	public:
		diffuse_light(texture *a) : emit(a) {}

		virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const {
			if (dot(rec.normal, r_in.direction()) < 0.0)
				return emit->value(u, v, p);
			else
				return vec3(0, 0, 0);
		}

		texture *emit;
};

#endif // !DIFFUSE_LIGHTH

