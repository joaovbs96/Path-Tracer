#pragma once

#include <vector>

#include "geometry.hpp"

// list initialization taken from @nice_byte's implementation
// https://github.com/nicebyte/ayanami

class Geometry_List : public Geometry {
 public:
  template <class... Args>
  Geometry_List(Args... args) : list_{std::forward<Args>(args)...} {}

  bool hit(const Ray& r, Hit_Record& hit_rec, float t_min,
           float t_max) const override {
    Hit_Record list_rec;  // temp hit record
    bool anyhit = false;
    float closest_hit = t_max;

    // iterates over list of geometries to find the closest intersection
    for (const auto& geom : list_) {
      if (geom->hit(r, list_rec, t_min, closest_hit)) {
        anyhit = true;
        closest_hit = list_rec.t;
        hit_rec = list_rec;
      }
    }

    return anyhit;
  }

  void add(Geometry* geom) { list_.push_back(geom); }

 private:
  std::vector<Geometry*> list_;
};