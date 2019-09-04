#pragma once

#include "../math/math.hpp"
#include "geometry.hpp"

class Sphere : public Geometry {
 public:
  Sphere(const float3& center, float radius, Brdf* material)
      : center_(center), radius_(radius), material_(material) {}

  bool hit(const Ray& r, Hit_Record& hit_rec, float t_min,
           float t_max) const override {
    // translates ray origin by sphere center
    const float3 oc = r.origin() - center_;

    const float a = dot(r.direction(), r.direction());
    const float b = dot(oc, r.direction()) * 2.0f;
    const float c = dot(oc, oc) - squared(radius_);
    const float d = squared(b) - 4.0f * a * c;

    // if the discriminant is < 0, no real roots exist
    if (d < 0.0f) return false;

    // checks if either real root falls in the valid range.

    // first real root
    float t = (-b - sqrtf(d)) / (2.0f * a);
    if (t >= t_min && t <= t_max) {
      hit_rec.t = t;
      hit_rec.p = r.point_at(t);
      hit_rec.normal = normalize(hit_rec.p - center_);
      hit_rec.brdf = material_;
      return true;
    }

    // second real root
    t = (-b + sqrtf(d)) / (2.0f * a);
    if (t >= t_min && t <= t_max) {
      hit_rec.t = t;
      hit_rec.p = r.point_at(t);
      hit_rec.normal = normalize(hit_rec.p - center_);
      hit_rec.brdf = material_;
      return true;
    }

    return false;
  }

 private:
  float3 center_;
  float radius_;
  Brdf* material_;
};