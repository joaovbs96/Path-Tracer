#pragma once

#include "../math/float3.hpp"
#include "../math/math.hpp"
#include "rng.hpp"

// https://karthikkaranth.me/blog/generating-random-points-in-a-sphere/
float3 random_in_unit_sphere(uint& seed) {
  float theta = rnd(seed) * TWO_PI;
  float sinTheta = sinf(theta);
  float cosTheta = cosf(theta);

  float phi = acosf(2.0f * rnd(seed) - 1.0f);
  float sinPhi = sinf(phi);
  float cosPhi = cosf(phi);

  float r = cbrtf(rnd(seed));
  float x = r * sinPhi * cosTheta;
  float y = r * sinPhi * sinTheta;
  float z = r * cosPhi;

  return float3(x, y, z);
}

float3 random_in_unit_disk(uint& seed) {
  float a = rnd(seed) * 2.0f * PI;

  float3 xy = float3(sinf(a), cosf(a), 0);

  return xy * sqrtf(rnd(seed));
}