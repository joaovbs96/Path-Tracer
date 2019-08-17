#pragma once

#include <stdint.h>
#include <string>

#include "math/float3.hpp"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

class Framebuffer {
 public:
  static constexpr int colorChannels = 3;

  Framebuffer(int width, int height)
      : width_(width),
        height_(height),
        data_((unsigned char *)malloc(width * height * colorChannels)) {}

  ~Framebuffer() { free(data_); }

  // Set pixel [row, col] to color (r, g, b)
  void set_pixel(int row, int column, uint8_t r, uint8_t g, uint8_t b) {
    const int idx = colorChannels * (row * width_ + column);
    data_[idx + 0] = r;
    data_[idx + 1] = g;
    data_[idx + 2] = b;
  }

  // Converts color to uint8 and set pixel [row, column] to color (r, g, b)
  void set_pixel(int row, int column, float3 color) {
    const int idx = colorChannels * (row * width_ + column);
    data_[idx + 0] = uint8_t(255.99f * color.x);  // r
    data_[idx + 1] = uint8_t(255.99f * color.y);  // g
    data_[idx + 2] = uint8_t(255.99f * color.z);  // b
  }

  // Save framebuffer to .PNG file
  int save(const char *file_name) {
    return stbi_write_png(file_name, width_, height_, colorChannels, data_, 0);
  }

  int width() { return width_; }
  int height() { return height_; }

 private:
  unsigned char *data_;
  int width_;
  int height_;
};