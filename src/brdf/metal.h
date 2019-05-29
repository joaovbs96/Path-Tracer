#ifndef METALH
#define METALH

#include "material.h"

class metal : public material {
 public:
  metal(texture* a, float f) : albedo(a) {
    if (f < 1) {
      fuzz = f;
    } else {
      fuzz = 1;
    }
  }

  // light that reflects off a diffuse surface has its direction randomized
  virtual bool scatter(const Ray& r_in, const hit_record& hrec,
                       scatter_record& srec) const {
    vec3 reflected = reflect(unit_vector(r_in.direction()), hrec.normal);

    srec.specular_ray = Ray(hrec.p, reflected + fuzz * random_in_unit_sphere(),
                            r_in.time());  // resultant randomized Ray
    srec.attenuation = albedo->value(
        hrec.u, hrec.v, hrec.p);  // attenuation, 'measure of the diffuse
                                  // reflection', color dependent
    srec.is_specular = true;
    srec.pdf_ptr = 0;
    return true;
  }

  texture* albedo;  // 'measure of the diffuse reflection'
  float fuzz;
};

#endif