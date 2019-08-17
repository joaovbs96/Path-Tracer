
#include <stdio.h>
#include <stdlib.h>

#include "framebuffer.hpp"
#include "geometry/sphere.hpp"
#include "ray.hpp"

float3 get_color(const Ray& r) {
  Sphere obj(float3(0.0f, 0.0f, -1.0f), 0.5f);
  if (obj.hit(r)) return float3(1.0f, 0.0f, 0.0f);

  float t = 0.5f * (r.direction().y + 1.0f);
  return (1.0f - t) * float3(1.0f) + t * float3(0.5f, 0.7f, 1.0f);
}

int main() {
  stbi_flip_vertically_on_write(1);

  Framebuffer fb(200, 100);

  float3 lower_left(-2.0f, -1.0f, -1.0f);
  float3 horizontal(4.0f, 0.0f, 0.0f);
  float3 vertical(0.0f, 2.0f, 0.0f);
  float3 origin(0.0f);

  for (size_t r = 0; r < fb.height(); r++) {
    for (size_t c = 0; c < fb.width(); c++) {
      float u = float(c) / float(fb.width());
      float v = float(r) / float(fb.height());

      Ray ray(origin, lower_left + u * horizontal + v * vertical);
      float3 color = get_color(ray);

      fb.set_pixel(r, c, color);
    }
  }

  fb.save("output.png");

  system("pause");
  return 0;
}