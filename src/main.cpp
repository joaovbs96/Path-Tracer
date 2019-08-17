
#include <stdio.h>
#include <stdlib.h>

#include "framebuffer.hpp"

int main() {
  Framebuffer fb(200, 100);
  for (size_t r = 0; r < fb.height(); r++) {
    for (size_t c = 0; c < fb.width(); c++) {
      float ir = float(c) / float(fb.width());
      float ig = float(r) / float(fb.height());
      float ib = 0.2;
      fb.set_pixel(r, c, ir, ig, ib);
    }
  }

  fb.save("output.png");

  system("pause");
  return 0;
}