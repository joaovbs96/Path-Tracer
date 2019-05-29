#ifndef PERLINH
#define PERLINH

#include "vec3.h"

inline float trilinear_interp(float c[2][2][2], float u, float v, float w) {
  float accum = 0;

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++)
        accum += (i * u + (1 - i) * (1 - u)) * (j * u + (1 - j) * (1 - v)) *
                 (k * w + (1 - k) * (1 - w)) * c[i][j][k];

  return accum;
}

inline float perlin_interp(vec3 c[2][2][2], float u, float v, float w) {
  float uu = u * u * (3.0f - 2.0f * u);
  float vv = v * v * (3.0f - 2.0f * v);
  float ww = w * w * (3.0f - 2.0f * w);
  float accum = 0;

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++) {
        vec3 weight_v(u - i, v - j, w - k);
        accum += (i * uu + (1.0f - i) * (1.0f - uu)) *
                 (j * vv + (1.0f - j) * (1.0f - vv)) *
                 (k * ww + (1.0f - k) * (1.0f - ww)) *
                 dot(c[i][j][k], weight_v);
      }

  return accum;
}

class Perlin {
 public:
  float noise(const vec3 &p) const {
    float u = p.x() - floor(p.x());
    float v = p.y() - floor(p.y());
    float w = p.z() - floor(p.z());

    int i = int(floor(p.x()));
    int j = int(floor(p.y()));
    int k = int(floor(p.z()));
    vec3 c[2][2][2];

    for (int di = 0; di < 2; di++)
      for (int dj = 0; dj < 2; dj++)
        for (int dk = 0; dk < 2; dk++)
          c[di][dj][dk] =
              ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^
                     perm_z[(k + dk) & 255]];

    return perlin_interp(c, u, v, w);
  }

  float turb(const vec3 &p, int depth = 7) const {
    float accum = 0;
    vec3 temp_p = p;
    float weight = 1.0;

    for (int i = 0; i < depth; i++) {
      accum += weight * noise(temp_p);
      weight *= 0.5;
      temp_p *= 2;
    }

    return fabs(accum);
  }

  static vec3 *ranvec;
  static int *perm_x;
  static int *perm_y;
  static int *perm_z;
};

static vec3 *perlin_generate() {
  vec3 *p = new vec3[256];

  for (int i = 0; i < 256; ++i)
    p[i] = unit_vector(vec3(-1 + 2 * RFG(), -1 + 2 * RFG(), -1 + 2 * RFG()));

  return p;
}

void permute(int *p, int n) {
  for (int i = n - 1; i > 0; i--) {
    int target = int(RFG() * (i + 1));
    int tmp = p[i];

    p[i] = p[target];
    p[target] = tmp;
  }

  return;
}

static int *perlin_generate_perm() {
  int *p = new int[256];

  for (int i = 0; i < 256; i++) p[i] = i;

  permute(p, 256);

  return p;
}

vec3 *Perlin::ranvec = perlin_generate();
int *Perlin::perm_x = perlin_generate_perm();
int *Perlin::perm_y = perlin_generate_perm();
int *Perlin::perm_z = perlin_generate_perm();

#endif  // !PERLINH
