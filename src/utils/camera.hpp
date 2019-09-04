#pragma once

#include "ray.hpp"

class Camera {
 public:
  Camera(float3 origin, float aspect_h, float aspect_v)
      : aspect_h_(aspect_h),
        aspect_v_(aspect_v),
        origin_(origin),
        lower_left_(-aspect_h_ / 2.0f, -aspect_v_ / 2.0f, -1.0f) {}

  Ray make_ray(float u, float v) {
    float3 uv = float3(u * aspect_h_, v * aspect_v_, 0.0f);
    return Ray(origin_, lower_left_ + uv - origin_);
  }

 private:
  float aspect_h_;
  float aspect_v_;
  float3 origin_;
  float3 lower_left_;
};