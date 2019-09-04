
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "brdf/glass.hpp"
#include "brdf/lambertian.hpp"
#include "brdf/metal.hpp"
#include "geometry/geometry_list.hpp"
#include "geometry/sphere.hpp"
#include "sampling/rng.hpp"
#include "sampling/sampling.hpp"
#include "utils/camera.hpp"
#include "utils/framebuffer.hpp"
#include "utils/ray.hpp"

float3 trace(Ray& r, Geometry* scene, uint& seed) {
  float3 throughput = float3(1.0f);

  // iterative version of recursion
  for (int depth = 0; depth < 50; depth++) {
    Hit_Record rec;

    // ray hit
    if (scene->hit(r, rec, 0.001f, FLT_MAX)) {
      // gets next direction
      float3 wi = rec.brdf->sample(rec.p, -r.direction(), rec.normal, seed);

      // accumulates color
      float pdf;
      throughput *= rec.brdf->eval(rec.p, -r.direction(), wi, rec.normal, pdf);

      // updates ray
      r = Ray(rec.p, wi);
    }

    // ray missed, samples background
    else {
      float t = 0.5f * (r.direction().y + 1.0f);
      float3 color = (1.0f - t) * float3(1.0f) + t * float3(0.5f, 0.7f, 1.0f);

      return throughput * color;  // return accumulated color
    }
  }

  // ray reached max depth: cancel recursion
  return float3(0.0f);
}

int main() {
  int samples = 100;
  stbi_flip_vertically_on_write(1);
  unsigned int seed = tea<64>(2, 5);

  Camera cam(float3(0.f), 4.0f, 2.0f);

  Framebuffer fb(200, 100);

  Geometry_List scene{new Sphere(float3(0.0f, 0.0f, -1.0f), 0.5f,
                                 new Lambertian(float3(0.6f, 0.3f, 0.3f))),
                      new Sphere(float3(0.0f, -100.5f, -1.0f), 100.0f,
                                 new Lambertian(float3(0.8f, 0.8f, 0.0f)))};

  scene.add(new Sphere(float3(1.0f, 0.0f, -1.0f), 0.5f,
                       new Metal(float3(0.8f, 0.6f, 0.2f))));

  scene.add(new Sphere(float3(-1.0f, 0.0f, -1.0f), 0.5f,
                       new Glass(float3(1.f), 1.5f)));

  for (int r = 0; r < fb.height(); r++) {
    for (int c = 0; c < fb.width(); c++) {
      float3 color;
      for (int s = 0; s < samples; s++) {
        float u = float(c + rnd(seed)) / float(fb.width());
        float v = float(r + rnd(seed)) / float(fb.height());

        Ray ray = cam.make_ray(u, v);
        color += trace(ray, &scene, seed);
      }

      fb.set_pixel(r, c, sqrt(color / float(samples)));
    }
  }

  fb.save("output.png");

  system("pause");
  return 0;
}