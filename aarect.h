#ifndef AARECTH
#define AARECTH

#include "hitable.h"

class aarect : public hitable {
	public:
		aarect() {}

		aarect(float _a0, float _a1, float _b0, float _b1, float _k, char _ax, material *mat) : 
											a0(_a0), a1(_a1), b0(_b0), b1(_b1), k(_k), ax(_ax), mp(mat) {};

		virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;

		virtual bool bounding_box(float t0, float t1, aabb& box) const {
			switch (ax) {
				case 'x': // x axis alligned
					box = aabb(vec3(k - 0.0001, a0, b0), vec3(k + 0.0001, a1, b1));
					return true;

				case 'y': // y axis alligned
					box = aabb(vec3(a0, k - 0.0001, b0), vec3(a1, k + 0.0001, b1));
					return true;

				case 'z': // z axis alligned
					box = aabb(vec3(a0, b0, k - 0.0001), vec3(a1, b1, k + 0.0001));
					return true;

				default:
					std::cout << "Error: Rectangle not alligned correctly" << std::endl;
					return false;
			}
			
		}

		virtual float pdf_value(const vec3& o, const vec3& v) const {
			hit_record rec;

			if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
				float area = (a1 - a0) * (b1 - b0);
				float distance_squared = rec.t * rec.t * v.squared_length();
				float cosine = fabs(dot(v, rec.normal) / v.length());

				return distance_squared / (cosine * area);
			}

			else
				return 0;
		}

		virtual vec3 random(const vec3& o) const {
			vec3 random_point;
			
			switch (ax) {
				case 'x':
					random_point = vec3(k, a0 + RFG() * (a1 - a0), b0 + RFG()*(b1 - b0));
					break;

				case 'y':
					random_point = vec3(a0 + RFG() * (a1 - a0), k, b0 + RFG()*(b1 - b0));
					break;

				case 'z':
					random_point = vec3(a0 + RFG() * (a1 - a0), b0 + RFG()*(b1 - b0), k);
					break;

				default:
					std::cout << "Error: Rectangle not alligned correctly" << std::endl;
					exit(1);
			}

			return random_point - o;
		}

		material *mp;
		float a0, a1, b0, b1, k;
		char ax;
};

bool aarect::hit(const ray& r, float t0, float t1, hit_record& rec) const {
	float t, a, b;
	vec3 normal;
	
	switch (ax) {
		case 'x': // x axis alligned
			t = (k - r.origin().x()) / r.direction().x();
			a = r.origin().y() + t * r.direction().y();
			b = r.origin().z() + t * r.direction().z();
			normal = vec3(1, 0, 0);
			break;

		case 'y': // y axis alligned
			t = (k - r.origin().y()) / r.direction().y();
			a = r.origin().x() + t * r.direction().x();
			b = r.origin().z() + t * r.direction().z();
			normal = vec3(0, 1, 0);
			break;

		case 'z': // z axis alligned
			t = (k - r.origin().z()) / r.direction().z();
			a = r.origin().x() + t * r.direction().x();
			b = r.origin().y() + t * r.direction().y();
			normal = vec3(0, 0, 1);
			break;

		default:
			std::cout << "Error: Rectangle not alligned correctly" << std::endl;
			return false;

	}

	if (t < t0 || t > t1)
		return false;

	if (a < a0 || a > a1 || b < b0 || b > b1)
		return false;

	rec.u = (a - a0) / (a1 - a0);
	rec.v = (b - b0) / (b1 - b0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.point_at_parameter(t);
	rec.normal = normal;
	
	return true;
}

#endif // !AARECTH
