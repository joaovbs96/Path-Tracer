#ifndef MESHH
#define MESHH

#include "triangle.h"
#include <vector>

class mesh : public triangle {
public:
	mesh() {}

	mesh(std::vector<triangle>& l, float s=1.0) {
		list = l;
		scale = s;
	}

	virtual bool hit(const Ray& r, float tmin, float tmax, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

	std::vector<triangle> list;
	float scale;
};

bool mesh::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	float closest_so_far = t_max;

	// Iterates list of triangles to check if Ray intersects/hits anything
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

bool mesh::bounding_box(float t0, float t1, aabb& box) const {
	if (list.size() < 1)
		return false;

	aabb temp_box;
	bool first_true = list[0].bounding_box(t0, t1, temp_box);

	if (!first_true)
		return false;
	else
		box = temp_box;

	for (int i = 1; i < list.size(); i++) {
		if (list[i].bounding_box(t0, t1, temp_box))
			box = surrounding_box(box, temp_box);
		else
			return false;
	}

	return true;
}

#endif // !MESHH