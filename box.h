#ifndef BOXH
#define BOXH

#include "hitable.h"
#include "aarect.h"
#include "flip_normals.h"
#include "hitable_list.h"

class box : public hitable {
	public:
		box() {}
		box(const vec3& p0, const vec3& pl, material *ptr);

		virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const {
			box = aabb(pmin, pmax);
			return true;
		}

		vec3 pmin, pmax;
		hitable *list_ptr;
};

box::box(const vec3& p0, const vec3& p1, material *ptr) {
	pmin = p0;
	pmax = p1;
	hitable **list = new hitable*[6];

	list[0] = new aarect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), 'z', ptr);
	list[1] = new flip_normals(new aarect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), 'z', ptr));
	list[2] = new aarect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), 'y', ptr);
	list[3] = new flip_normals(new aarect(p0.x(), p1.x(), p0.y(), p1.y(), p0.y(), 'y', ptr));
	list[4] = new aarect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), 'x', ptr);
	list[5] = new flip_normals(new aarect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), 'x', ptr));

	list_ptr = new hitable_list(list, 6);
}

bool box::hit(const ray& r, float t0, float t1, hit_record& rec) const {
	return list_ptr->hit(r, t0, t1, rec);
}

#endif // !BOXH
