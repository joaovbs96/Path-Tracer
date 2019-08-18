
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.hpp"
#include "framebuffer.hpp"
#include "geometry/geometry_list.hpp"
#include "geometry/sphere.hpp"
#include "math/rng.hpp"
#include "ray.hpp"

float3 get_color(const Ray& r, Geometry* scene) {
  Hit_Record hit_rec;
  if (scene->hit(r, hit_rec, 0.0f, FLT_MAX))
    return hit_rec.normal * 0.5f + float3(0.5f);

  float t = 0.5f * (r.direction().y + 1.0f);
  return (1.0f - t) * float3(1.0f) + t * float3(0.5f, 0.7f, 1.0f);
}

int main() {
  int samples = 100;
  stbi_flip_vertically_on_write(1);
  unsigned int seed = tea<64>(2, 5);

  Camera cam(float3(0.f), 4.0f, 2.0f);

  Framebuffer fb(200, 100);

  Geometry_List scene{new Sphere(float3(0.0f, 0.0f, -1.0f), 0.5f),
                      new Sphere(float3(0.0f, -100.5f, -1.0f), 100.0f)};

  scene.add(new Sphere(float3(0.0f, 1.0f, -1.0f), 0.5f));

  for (int r = 0; r < fb.height(); r++) {
    for (int c = 0; c < fb.width(); c++) {
      float3 color;
      for (int s = 0; s < samples; s++) {
        float u = float(c + rnd(seed)) / float(fb.width());
        float v = float(r + rnd(seed)) / float(fb.height());

        Ray ray = cam.make_ray(u, v);
        color += get_color(ray, &scene);
      }

      fb.set_pixel(r, c, color / float(samples));
    }
  }

  fb.save("output.png");

  system("pause");
  return 0;
}