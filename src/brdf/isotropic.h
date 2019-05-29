#ifndef ISOTROPICH
#define ISOTROPICH

#include "material.h"

class isotropic : public material {
 public:
  isotropic(texture* a) : albedo(a) {}

  virtual bool scatter(const Ray& r_in, const hit_record& rec,
                       vec3& attenuation, Ray& scattered) const {
    scattered = Ray(rec.p, random_in_unit_sphere());
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  }

  texture* albedo;
};

#endif  // !ISOTROPICH
