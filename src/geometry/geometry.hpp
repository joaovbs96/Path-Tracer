#pragma once

#include "../ray.hpp"

struct Hit_Record {
  float t;
  float3 p;
  float3 normal;
};

class Geometry {
 public:
  virtual bool hit(const Ray& r, Hit_Record& hit_rec, float t_min,
                   float t_max) const = 0;
};