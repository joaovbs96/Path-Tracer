#pragma once

#include "../utils/prd.hpp"
#include "../utils/ray.hpp"

class Geometry {
 public:
  virtual bool hit(const Ray& r, Hit_Record& hit_rec, float t_min,
                   float t_max) const = 0;
};