#pragma once

#include "brdf.hpp"

class Glass : public Brdf {
 public:
  Glass(const float3& albedo, const float ior) : albedo_(albedo), ior_(ior) {}

  virtual float3 sample(const float3& p, const float3& wo, const float3& n,
                        uint& seed) const {
    float3 nn = n;
    float3 wo_ = -wo;
    float ni_over_nt;
    float cosine = dot(wo_, nn);
    bool is_refracted = false;
    float3 refracted;

    // Ray is exiting the object
    if (cosine > 0.0f) {
      is_refracted = refract(wo_, -nn, ior_, refracted);
      cosine = (ior_ * cosine) / length(wo_);

      // TODO: Beer-Lambert Law
    }

    // Ray is entering the object
    else {
      is_refracted = refract(wo_, nn, 1.0f / ior_, refracted);
      cosine = -cosine / length(wo_);
    }

    // Importance sample the Fresnel term
    float reflect_prob;
    if (is_refracted)
      reflect_prob = schlick(cosine, ior_);
    else
      reflect_prob = 1.0f;

    // Ray should be reflected or refracted
    if (rnd(seed) < reflect_prob)
      return reflect(wo_, n);
    else
      return refracted;
  }

  virtual float3 eval(const float3& p, const float3& wo, const float3& wi,
                      const float3& n, float& pdf) const {
    return albedo_;
  }

 private:
  float3 albedo_;
  float ior_;
};