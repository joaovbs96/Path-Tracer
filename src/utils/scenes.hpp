#pragma once

#include "../brdf/glass.hpp"
#include "../brdf/lambertian.hpp"
#include "../brdf/metal.hpp"
#include "../geometry/geometry_list.hpp"
#include "../geometry/sphere.hpp"
#include "../sampling/rng.hpp"
#include "../utils/camera.hpp"

void In_One_Weekend(Geometry_List& scene, Camera& cam, int width, int height,
                    uint& seed) {
  Brdf* ground = new Lambertian(float3(0.5f));
  scene.add(new Sphere(float3(0.f, -1000.f, -1.f), 1000.f, ground));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      float choose_mat = rnd(seed);
      float3 center = float3(a + rnd(seed), 0.2f, b + rnd(seed));
      if (choose_mat < 0.8f) {
        Brdf* mt = new Lambertian(float3(rnd(seed), rnd(seed), rnd(seed)));
        scene.add(new Sphere(center, 0.2f, mt));
      } else if (choose_mat < 0.95f) {
        Brdf* mt = new Metal(float3(0.5f * (1.f + rnd(seed)),   // r
                                    0.5f * (1.f + rnd(seed)),   // g
                                    0.5f * (1.f + rnd(seed))),  // b
                             0.5f * rnd(seed));                 // fuzz
        scene.add(new Sphere(center, 0.2f, mt));
      } else {
        Brdf* mt = new Glass(float3(1.0f), 1.5);
        scene.add(new Sphere(center, 0.2f, mt));
      }
    }
  }

  Brdf* mt0 = new Glass(float3(1.0f), 1.5);
  scene.add(new Sphere(float3(0.0f, 1.0f, 0.0f), 1.f, mt0));

  Brdf* mt2 = new Lambertian(float3(0.4f, 0.2f, 0.1f));
  scene.add(new Sphere(float3(-4.f, 1.f, 0.f), 1.f, mt2));

  Brdf* mt3 = new Metal(float3(0.7f, 0.6f, 0.5f), 0.f);
  scene.add(new Sphere(float3(4.f, 1.f, 0.f), 1.f, mt3));

  const float3 lookfrom(13.0f, 2.0f, 3.0f);
  const float3 lookat(0.0f);
  const float fovy(40.0f);
  const float aspect(float(width) / height);
  const float aperture(0.1f);
  const float dist(10.0f);
  cam = Camera(lookfrom, lookat, fovy, aspect, aperture, dist);
}