#ifndef MESHH
#define MESHH

#include "triangle.h"

class mesh : public triangle {
public:
	mesh() {}

	mesh(triangle **l, int n) {
		list = l;
		list_size = n;
	}

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	triangle **list;
	int list_size;
};

bool mesh::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;

	// Iterates list of triangles to check if ray intersects/hits anything
	// If so, returns true. Otherwise, false.
	for (int i = 0; i < list_size; i++) {
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}

class meshList : public mesh {
public:
	meshList() {}

	meshList(mesh **l, int n) {
		list = l;
		list_size = n;
	}

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	mesh **list;
	int list_size;
};

bool meshList::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;

	// Iterates list of meshes to check if ray intersects/hits anything
	// If so, returns true. Otherwise, false.
	for (int i = 0; i < list_size; i++) {
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}

#endif // !MESHH