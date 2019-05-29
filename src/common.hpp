#pragma once

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <random>

// Math defines
#ifndef PI_F
#define PI_F 3.141592654f
#endif

#ifndef PI_D
#define PI_D 3.14159265358979323846264338327950288
#endif

// Axis type
typedef enum { X_AXIS, Y_AXIS, Z_AXIS } AXIS;

// Random Generators
thread_local uint32_t s_RndState = 1;

// XorShift32(RIG) & randomNumber(RFG) by Aras Pranckevicius
// source:
// https://github.com/aras-p/ToyPathTracer/blob/master/Cpp/Source/Maths.cpp#L5-L18
static uint32_t RIG() {  // random int generator
  uint32_t x = s_RndState;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 15;
  s_RndState = x;
  return x;
}

float RFG() {  // random float generator
  return (RIG() & 0xFFFFFF) / 16777216.0f;
}

inline float clamp(float value) { return value > 1.0f ? 1.0f : value; }