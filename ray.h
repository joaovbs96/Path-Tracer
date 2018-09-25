#ifndef RAYH
#define RAYH
#include "vec3.h"

class ray {
public:
	ray() {}

	// A: ray origin
	// B: ray direction
	// t: parameter that changes 'destination' along ray/half-line

	ray(const vec3& a, const vec3& b) {
		A = a;
		B = b;
	}

	vec3 origin() const {
		return A;
	}

	vec3 direction() const {
		return B;
	}

	vec3 point_at_parameter(float t) const {
		return A + (t*B);
	}

	vec3 A;
	vec3 B;
};

#endif