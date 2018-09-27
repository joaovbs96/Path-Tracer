#ifndef MESHH
#define MESHH

#include "triangle.h"
#include <vector>

class mesh : public triangle {
public:
	mesh() {}

	mesh(std::vector<triangle>& l) {
		list = l;
	}

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	std::vector<triangle> list;
};

bool mesh::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;

	// Iterates list of triangles to check if ray intersects/hits anything
	// If so, returns true. Otherwise, false.
	for (int i = 0; i < list.size(); i++) {
		if (list[i].hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}

#endif // !MESHH