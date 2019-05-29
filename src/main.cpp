#include "scenes.hpp"

std::mutex mtx;
float completed_threads;

vec3 color(const Ray &r, hitable *world, hitable *light_shape, int depth,
           bool light) {
  hit_record hrec;

  // world->hit iterate through scene objs to find if there was a
  // hit/intersection.
  if (world->hit(r, 0.001f, FLT_MAX, hrec)) {
    // if there's a hit, Ray can be scattered or absorbed.
    scatter_record srec;
    vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);

    // Ray is scattered/dispersed. Scatter behavior is material dependent.
    if (depth < 50 && hrec.mat_ptr->scatter(r, hrec, srec)) {
      if (srec.is_specular)
        return srec.attenuation *
               color(srec.specular_ray, world, light_shape, depth + 1, light);
      else {
        hitable_pdf plight(light_shape, hrec.p);
        mixture_pdf p(&plight, srec.pdf_ptr);

        Ray scattered = Ray(hrec.p, p.generate(), r.time());
        float pdf = p.value(scattered.direction());
        delete srec.pdf_ptr;

        return emitted +
               srec.attenuation *
                   hrec.mat_ptr->scattering_pdf(r, hrec, scattered) *
                   color(scattered, world, light_shape, depth + 1, light) /
                   pdf;  // recursive/bounce
      }
    } else  // Ray is fully absorbed.
      return emitted;

  } else {  // if the Ray doesn't intersect anything, return 'background color'.

    if (light) {  // sky
      vec3 unit_direction = unit_vector(r.direction());
      float t = 0.5f * (unit_direction.y() + 1.0f);
      return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
    } else  // dark background
      return vec3(0, 0, 0);
  }
}

void thread_func(int i, int j, int w, int h, int samples, Camera cam,
                 hitable *world, hitable *light_source, unsigned char *arr,
                 bool light) {
  std::stringstream ss;
  ss << std::this_thread::get_id();
  s_RndState = std::stoull(ss.str());

  vec3 col(0, 0, 0);

  // for each sample
  for (int s = 0; s < samples; ++s) {
    // each sample has a random offset from
    // the pixel's lower left corner, obtained by:
    float u = float(i + RFG()) / float(w);
    float v = float(j + RFG()) / float(h);

    // (u, v) are always inside the pixel (i, j)

    // trace a Ray
    Ray r = cam.get_ray(u, v);

    // get color of Ray's 'destination'
    col += de_nan(color(r, world, light_source, 0, light));
  }

  // average sample values
  col = vec3(col[0] / float(samples), col[1] / float(samples),
             col[2] / float(samples));

  // gamma correction
  col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

  // save value at pixel array
  arr[(h - j - 1) * 3 * w + 3 * i + 0] = int(255.99 * clamp(col.r()));  // red
  arr[(h - j - 1) * 3 * w + 3 * i + 1] = int(255.99 * clamp(col.g()));  // green
  arr[(h - j - 1) * 3 * w + 3 * i + 2] = int(255.99 * clamp(col.b()));  // blue

  // print progress
  mtx.lock();
  printf("%f%%\n", (++completed_threads * 100.0) / (w * h));
  mtx.unlock();
}

void Render(int i,                  // thread id
            int w,                  // width
            int h,                  // height
            int samples,            // total samples
            Camera cam,             // camera
            hitable *world,         // scene geometry
            hitable *light_source,  // lights
            unsigned char *arr,     // output array
            bool light) {           // miss variable
  std::stringstream ss;
  ss << std::this_thread::get_id();
  s_RndState = std::stoull(ss.str());

  vec3 col(0, 0, 0);

  // TODO: we will need either N different arrays depending on the number of
  // threads or to make use of locks

  // for each column
  // for each line
  // for each sample
  for (int s = 0; s < samples; ++s) {
    // each sample has a random offset from
    // the pixel's lower left corner, obtained by:
    float u = float(i + RFG()) / float(w);
    float v = float(j + RFG()) / float(h);

    // (u, v) are always inside the pixel (i, j)

    // trace a Ray
    Ray r = cam.get_ray(u, v);

    // get color of Ray's 'destination'
    col += de_nan(color(r, world, light_source, 0, light));
  }

  // average sample values
  col = vec3(col[0] / float(samples), col[1] / float(samples),
             col[2] / float(samples));

  // gamma correction
  col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

  // save value at pixel array
  arr[(h - j - 1) * 3 * w + 3 * i + 0] = int(255.99 * clamp(col.r()));  // red
  arr[(h - j - 1) * 3 * w + 3 * i + 1] = int(255.99 * clamp(col.g()));  // green
  arr[(h - j - 1) * 3 * w + 3 * i + 2] = int(255.99 * clamp(col.b()));  // blue

  // print progress
  mtx.lock();
  printf("%f%%\n", (++completed_threads * 100.0) / (w * h));
  mtx.unlock();
}

void do_join(std::thread &t) { t.join(); }

int main() {
  auto start = std::chrono::system_clock::now();
  completed_threads = 0;

  // main parameters
  int samples = 100;
  int scene = 7;
  bool light = true;
  std::string output = "test.png";

  // setting up camera
  // default values
  int w = 400;
  int h = 100;
  vec3 lookfrom(13, 2, 3);
  vec3 lookat(0, 0, 0);
  float dist_to_focus = 10.0;
  float aperture = 0.0;
  float vfov = 20.0;

  hitable *world;  // create scene
  hitable *light_shape = new aarect(213, 343, 227, 332, 554, 'y', 0);
  hitable *glass_sphere = new sphere(vec3(190, 90, 190), 90, 0);
  hitable *a[2];
  a[0] = light_shape;
  a[1] = glass_sphere;
  hitable_list hlist(a, 2);

  switch (scene) {
    case 0:  // In One Weekend test scene
      world = random_scene();
      break;

    case 1:  // model loader test scene
      world = model_scene();
      break;

    case 2:  // diffuse light test #1
      world = light_scene_1();
      light = false;
      break;

    case 3:  // diffuse light test #2
      world = light_scene_2();
      light = false;
      break;

    case 4:  // diffuse light test #2
      world = random_scene_movement();
      break;

    case 5:  // constant medium
      world = medium_scene_1();

      w = 100;
      h = 100;
      lookfrom = vec3(278, 278, -800);
      lookat = vec3(278, 278, 0);

      break;

    case 6:  // cornell box
      world = cornell_box();

      w = 300;
      h = 300;
      light = false;

      lookfrom = vec3(278, 278, -800);
      lookat = vec3(278, 278, 0);
      vfov = 40.0;

      break;

    case 7:  // perlin spheres test scene
      light = true;

      world = two_perlin_spheres();
      break;

    case 8:  // final scene
      world = final("Lowpoly_tree_sample.obj", "assets/");

      w = 500;
      h = 500;
      light = false;
      vfov = 40.0;

      lookfrom = vec3(478, 278, -600);
      lookat = vec3(278, 278, 0);

      break;

    case 9:
      world = cornell_metal_box();

      w = 300;
      h = 300;
      light = false;

      lookfrom = vec3(278, 278, -800);
      lookat = vec3(278, 278, 0);
      vfov = 40.0;

      break;

    default:  // bvh debug scene
      lookfrom = vec3(30, 20, 3);
      world = bvh_debug();
  }

  // camera
  Camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(w) / float(h),
             aperture, dist_to_focus, 0.0, 1.0);

  // pixel array
  unsigned char *arr =
      (unsigned char *)malloc(w * h * 3 * sizeof(unsigned char));

  // render loop for each pixel
  std::vector<std::thread> v;
  for (int j = h - 1; j >= 0; j--)
    for (int i = 0; i < w; i++)
      v.push_back(std::thread(thread_func, i, j, w, h, samples, cam, world,
                              &hlist, arr, light));
  std::for_each(v.begin(), v.end(), do_join);

  // save image
  stbi_write_png((char *)output.c_str(), w, h, 3, arr, 0);

  // print execution time
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cout << diff.count() << " s\n";

  std::system("PAUSE");

  return 0;
}