#pragma once

#include "../math/math.hpp"
#include "geometry.hpp"

class Sphere : public Geometry {
 public:
  Sphere(const float3& center, float radius)
      : center_(center), radius_(radius) {}

  bool hit(const Ray& r) override {
    // translates ray origin by sphere center
    const float3 oc = r.origin() - center_;

    const float a = dot(r.direction(), r.direction());
    const float b = dot(oc, r.direction()) * 2.0f;
    const float c = dot(oc, oc) - squared(radius_);
    const float d = b * b - 4.0f * a * c;

    return d > 0.f;
  }

 private:
  float3 center_;
  float radius_;
};