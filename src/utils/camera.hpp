#pragma once

#include "../sampling/sampling.hpp"
#include "ray.hpp"

class Camera {
 public:
  Camera(const float3 &lookfrom,  // where camera is looking from
         const float3 &lookat,    // where camera is looking at
         float vfov,        // vertical field of view(top to bottom in degrees)
         float aspect,      // aspect ratio
         float aperture) {  // camera aperrtura(blur)
    origin_ = lookfrom;

    lens_radius_ = aperture / 2.0f;

    const float theta = vfov * PI / 180.0f;
    const float half_h = tanf(theta / 2.0f);
    const float half_w = aspect * half_h;

    const float3 vup = float3(0.0f, 1.0f, 0.0f);  // y is up
    w_ = normalize(lookfrom - lookat);
    u_ = normalize(cross(vup, w_));
    v_ = cross(w_, u_);

    const float focus_dist = length(lookfrom - lookat);
    lower_left_ = origin_ - focus_dist * (half_w * u_ + half_h * v_ + w_);
    horizontal_ = 2.0f * half_w * focus_dist * u_;
    vertical_ = 2.0f * half_h * focus_dist * v_;
  }

  // completar
  Ray make_ray(float s, float t, uint &seed) {
    const float3 rd = lens_radius_ * random_in_unit_disk(seed);
    const float3 uv = u_ * rd.x + v_ * rd.y;
    const float3 direction = lower_left_ + s * horizontal_ + t * vertical_;
    return Ray(origin_ + uv, direction - origin_ - uv);
  }

 private:
  float3 origin_;
  float3 lower_left_;
  float3 horizontal_;
  float3 vertical_;
  float3 u_, v_, w_;
  float lens_radius_;
};