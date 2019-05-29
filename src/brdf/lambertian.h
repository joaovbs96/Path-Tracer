#ifndef LAMBERTIANH
#define LAMBERTIANH

#include "material.h"
#include "onb.h"

class lambertian : public material {
 public:
  lambertian(texture* a) : albedo(a) {}

  float scattering_pdf(const Ray& r_in, const hit_record& rec,
                       const Ray& scattered) const {
    float cosine = dot(rec.normal, unit_vector(scattered.direction()));

    if (cosine < 0) cosine = 0;

    return cosine / PI_F;
  }

  // light that reflects off a diffuse surface has its direction randomized
  bool scatter(const Ray& r_in, const hit_record& hrec,
               scatter_record& srec) const {
    srec.is_specular = false;
    srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.p);
    srec.pdf_ptr = new cosine_pdf(hrec.normal);
    return true;
  }

  texture* albedo;  // 'measure of the diffuse reflection'
};

#endif