#pragma once

#include "../sampling/sampling.hpp"
#include "../utils/prd.hpp"
#include "../utils/ray.hpp"

#include <algorithm>

// Some functions here are based on PBRT's implementation
// https://github.com/mmp/pbrt-v3/blob/9f717d847a807793fa966cf0eaa366852efef167/src/core/reflection.h

class Brdf {
 public:
  virtual float3 sample(const float3& p, const float3& wo, const float3& n,
                        uint& seed) const = 0;

  virtual float3 eval(const float3& p, const float3& wo, const float3& wi,
                      const float3& n, float& pdf) const = 0;
};

inline float3 reflect(const float3& i, const float3& n) {
  return i - 2.0f * n * dot(n, i);
}

inline bool refract(const float3& wi, const float3& n, const float ni_o_nt,
                    float3& r) {
  const float3 ui = normalize(wi);
  const float dt = dot(wi, n);
  float d = 1.0f - squared(ni_o_nt) * (1.0f - squared(dt));

  // Handle total internal reflection for transmission
  if (d > 0.0f) {
    r = ni_o_nt * (ui - n * dt) - n * sqrtf(d);
    return true;
  }

  return false;
}

inline float schlick(float cosine, float ref_idx) {
  float r0 = (1.f - ref_idx) / (1.f + ref_idx);
  r0 = r0 * r0;
  return r0 + (1.f - r0) * powf((1.f - cosine), 5.f);
}