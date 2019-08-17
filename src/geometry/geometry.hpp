#pragma once

#include "../ray.hpp"

class Geometry {
 public:
  virtual bool hit(const Ray& r) = 0;
};