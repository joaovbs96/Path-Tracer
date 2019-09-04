#pragma once

#include <math.h>
#include <stdlib.h>

class float3 {
 public:
  float3() : x(0.0f), y(0.0f), z(0.0f) {}
  float3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
  float3(float xyz_) : x(xyz_), y(xyz_), z(xyz_) {}

  inline const float3& operator+() const { return *this; }
  inline float3 operator-() const { return float3(-x, -y, -z); }

  inline float3& operator+=(const float3& v);
  inline float3& operator-=(const float3& v);
  inline float3& operator*=(const float3& v);
  inline float3& operator/=(const float3& v);
  inline float3& operator*=(const float t);
  inline float3& operator/=(const float t);

  float x, y, z;
};

inline float3& float3::operator+=(const float3& v) {
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

inline float3& float3::operator-=(const float3& v) {
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

inline float3& float3::operator*=(const float3& v) {
  x *= v.x;
  y *= v.y;
  z *= v.z;
  return *this;
}

inline float3& float3::operator/=(const float3& v) {
  x /= v.x;
  y /= v.y;
  z /= v.z;
  return *this;
}

inline float3& float3::operator*=(const float t) {
  x *= t;
  y *= t;
  z *= t;
  return *this;
}

inline float3& float3::operator/=(const float t) {
  float k = 1.0f / t;
  x *= k;
  y *= k;
  z *= k;
  return *this;
}

inline float3 operator+(const float3& v0, const float3& v1) {
  return float3(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
}

inline float3 operator-(const float3& v0, const float3& v1) {
  return float3(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
}

inline float3 operator*(const float3& v0, const float3& v1) {
  return float3(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z);
}

inline float3 operator/(const float3& v0, const float3& v1) {
  return float3(v0.x / v1.x, v0.y / v1.y, v0.z / v1.z);
}

inline float3 operator*(const float3& v, const float t) {
  return float3(v.x * t, v.y * t, v.z * t);
}

inline float3 operator*(const float t, const float3& v) {
  return float3(v.x * t, v.y * t, v.z * t);
}

inline float3 operator/(const float3& v, const float t) {
  return float3(v.x / t, v.y / t, v.z / t);
}

inline float dot(const float3& v0, const float3& v1) {
  return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

inline float3 cross(const float3& v0, const float3& v1) {
  float x = v0.y * v1.z - v0.z * v1.y;
  float y = -v0.x * v1.z + v0.z * v1.x;
  float z = v0.x * v1.y - v0.y * v1.x;
  return float3(x, y, z);
}

inline float3 sqrt(const float3& v) {
  return float3(sqrtf(v.x), sqrtf(v.y), sqrtf(v.z));
}

inline float length(const float3& v) {
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline float squared_length(const float3& v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline float3 normalize(const float3& v) {
  float k = 1.0f / length(v);
  return v * k;
}