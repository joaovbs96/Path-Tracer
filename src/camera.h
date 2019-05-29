#ifndef CAMERAH
#define CAMERAH
#define _USE_MATH_DEFINES

#include <math.h>
#include "ray.h"

vec3 random_in_unit_disk() {
  float a = RFG() * 2.0f * 3.1415926f;
  vec3 xy(sin(a), cos(a), 0);
  xy *= sqrt(RFG());
  return xy;
}

class Camera {
 public:
  Camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect,
         float aperture, float focus_dist, float t0, float t1) {
    lens_radius = aperture / 2.0f;

    // shutter is open between t0 and t1
    time0 = t0;
    time1 = t1;

    float theta = vfov * PI_F / 180.0f;
    float half_height = tan(theta / 2.0f);
    float half_width = aspect * half_height;

    // where camera is looking from
    origin = lookfrom;

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    lower_left_corner =
        origin - focus_dist * (half_width * u + half_height * v + w);
    horizontal = 2.0f * half_width * focus_dist * u;
    vertical = 2.0f * half_height * focus_dist * v;
  }

  // trace a new Ray
  Ray get_ray(float s, float t) {
    vec3 rd = lens_radius * random_in_unit_disk();
    vec3 offset = u * rd.x() + v * rd.y();
    float time = time0 + RFG() * (time1 - time0);
    return Ray(
        origin + offset,
        lower_left_corner + s * horizontal + t * vertical - origin - offset,
        time);
  }

  vec3 origin;
  vec3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  float time0, time1;
  float lens_radius;
};

#endif  // !CAMERAH
