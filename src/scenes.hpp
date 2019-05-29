#ifndef SCENESHPP
#define SCENESHPP

#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <strstream>
#include <thread>
#include <vector>

#include "camera.h"

#include "pdf/cosine_pdf.h"
#include "pdf/hitable_pdf.h"
#include "pdf/mixture_pdf.h"

#include "geometry/aarect.h"
#include "geometry/box.h"
#include "geometry/bvh_node.h"
#include "geometry/constant_medium.h"
#include "geometry/flip_normals.h"
#include "geometry/hitable_list.h"
#include "geometry/mesh.h"
#include "geometry/moving_sphere.h"
#include "geometry/rotate.h"
#include "geometry/sphere.h"
#include "geometry/translate.h"

#include "brdf/dielectric.h"
#include "brdf/diffuse_light.h"
#include "brdf/isotropic.h"
#include "brdf/lambertian.h"
#include "brdf/metal.h"

#include "texture/checker.h"
#include "texture/constant.h"
#include "texture/image.h"
#include "texture/noise_texture.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

hitable *load_and_convert(std::string filename, std::string assets_folder,
                          float scale = 1.0) {
  hitable **list = new hitable *[50000];  // TODO: usar vectors
  /*int l = 0;
  objl::Loader Loader;

  std::cout << "Loading Model" << std::endl;
  bool loadout = Loader.LoadFile((char *)filename.c_str());
  std::cout << std::endl << "Model Loaded" << std::endl << std::endl;

  // Check if obj was loaded
  if (loadout) {
    // Go through each loaded mesh
    for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {
      // Copy one of the loaded meshes to be our current mesh
      std::cout << '\r' << "                         " << '\r';  // flush line
      std::cout << "Converting Mesh " << (i + 1) << "/"
                << Loader.LoadedMeshes.size();
      objl::Mesh curMesh = Loader.LoadedMeshes[i];

      // TODO: e se n�o tiver MTL?

      // get mtl material for mesh
      material *m;
      // if there's no diffuse map, it's a constant color texture
      if (curMesh.MeshMaterial.map_Kd.empty()) {
        m = new lambertian(new constant_texture(
            vec3(curMesh.MeshMaterial.Kd.X, curMesh.MeshMaterial.Kd.Y,
                 curMesh.MeshMaterial.Kd.Z)));

        // Go through every 3rd index and print the
        // save that these indices represent
        for (int j = 0; j < curMesh.Indices.size(); j += 3) {
          int ia = curMesh.Indices[j];
          vec3 a(scale * curMesh.Vertices[ia].Position.X,
                 scale * curMesh.Vertices[ia].Position.Y,
                 scale * curMesh.Vertices[ia].Position.Z,
                 0.0,   // optional parameters u
                 0.0);  // optional parameters v

          int ib = curMesh.Indices[j + 1];
          vec3 b(scale * curMesh.Vertices[ib].Position.X,
                 scale * curMesh.Vertices[ib].Position.Y,
                 scale * curMesh.Vertices[ib].Position.Z,
                 0.0,   // optional parameters u
                 0.0);  // optional parameters v

          int ic = curMesh.Indices[j + 2];
          vec3 c(scale * curMesh.Vertices[ic].Position.X,
                 scale * curMesh.Vertices[ic].Position.Y,
                 scale * curMesh.Vertices[ic].Position.Z,
                 0.0,   // optional parameters u
                 0.0);  // optional parameters v

          vec3 n = cross(b - a, c - a);

          list[l++] = new triangle(a, b, c, n, m, scale);
        }
      } else {  // otherwise, it's an image texture
        int nx, ny, nn;
        char *fileName = (char *)curMesh.MeshMaterial.map_Kd.c_str();
        unsigned char *tex_data = stbi_load(fileName, &nx, &ny, &nn, 0);
        m = new lambertian(new image_texture(tex_data, nx, ny));

        // Go through every 3rd index and print the
        // save that these indices represent
        for (int j = 0; j < curMesh.Indices.size(); j += 3) {
          int ia = curMesh.Indices[j];
          vec3 a(scale * curMesh.Vertices[ia].Position.X,
                 scale * curMesh.Vertices[ia].Position.Y,
                 scale * curMesh.Vertices[ia].Position.Z,
                 curMesh.Vertices[ia]
                     .TextureCoordinate.X,  // optional parameters u
                 curMesh.Vertices[ia]
                     .TextureCoordinate.Y);  // optional parameters v

          int ib = curMesh.Indices[j + 1];
          vec3 b(scale * curMesh.Vertices[ib].Position.X,
                 scale * curMesh.Vertices[ib].Position.Y,
                 scale * curMesh.Vertices[ib].Position.Z,
                 curMesh.Vertices[ib]
                     .TextureCoordinate.X,  // optional parameters u
                 curMesh.Vertices[ib]
                     .TextureCoordinate.Y);  // optional parameters v

          int ic = curMesh.Indices[j + 2];
          vec3 c(scale * curMesh.Vertices[ic].Position.X,
                 scale * curMesh.Vertices[ic].Position.Y,
                 scale * curMesh.Vertices[ic].Position.Z,
                 curMesh.Vertices[ic]
                     .TextureCoordinate.X,  // optional parameters u
                 curMesh.Vertices[ic]
                     .TextureCoordinate.Y);  // optional parameters v

          vec3 n = cross(b - a, c - a);

          list[l++] = new triangle(a, b, c, n, m, scale);
        }
      }
    }
    std::cout << std::endl;
    std::cout << "Meshes Converted" << std::endl;
    return new bvh_node(list, l, 0.0, 1.0);
  }

  // If it wasn't, output an error
  else {
    // Output Error
    std::cout << "Failed to Load File. May have failed to find it or it was "
                 "not an .obj file."
              << std::endl;
    return NULL;
  }*/

  return new bvh_node(list, 0, 0.0, 1.0);
}

hitable *random_scene() {
  int n = 50000;
  hitable **list = new hitable *[n + 1];
  list[0] =
      new sphere(vec3(0, -1000, 0), 1000,
                 new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
  int i = 1;

  // add spheres
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      float choose_mat = RFG();
      vec3 center(a + 0.9f * RFG(), 0.2f, b + 0.9f * RFG());

      if ((center - vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {
        if (choose_mat < 0.8) {  // diffuse/lambertian - chapter 7
          list[i++] =
              new sphere(center, 0.2f,
                         new lambertian(new constant_texture(vec3(
                             RFG() * RFG(), RFG() * RFG(), RFG() * RFG()))));
        } else if (choose_mat < 0.95f) {  // metal - chapter 8
          list[i++] = new sphere(
              center, 0.2f,
              new metal(
                  new constant_texture(vec3(0.5f * (1 + RFG()),
                                            0.5f * (1 + RFG()), 0.5f * RFG())),
                  0.5f));
        } else {  // dieletric/glass - chapter 9
          list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
        }
      }
    }
  }

  // int nx, ny, nn;
  // unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
  list[i++] =
      new sphere(vec3(4, 1, 0), 1.0,
                 new lambertian(new constant_texture(vec3(0.4f, 0.2f, 0.1f))));
  list[i++] = new sphere(vec3(0, 1, 0), 1.0f, new dielectric(1.5f));
  list[i++] =
      new sphere(vec3(-4, 1, 0), 1.0f,
                 new metal(new constant_texture(vec3(0.7f, 0.6f, 0.5f)), 0.0f));

  return new bvh_node(list, i, 0.0f, 1.0f);
}

hitable *random_scene_movement() {
  int n = 50000;
  hitable **list = new hitable *[n + 1];
  list[0] =
      new sphere(vec3(0, -1000, 0), 1000,
                 new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
  int i = 1;

  // add spheres
  for (int a = -10; a < 10; a++)
    for (int b = -10; b < 10; b++) {
      float choose_mat = RFG();
      vec3 center(a + 0.9f * RFG(), 0.2f, b + 0.9f * RFG());

      if ((center - vec3(4, 0.2f, 0)).length() > 0.9f) {
        if (choose_mat < 0.8) {  // diffuse/lambertian - chapter 7
          list[i++] = new moving_sphere(
              center, center + vec3(0, 0.5f * RFG(), 0), 0.0f, 1.0f, 0.2f,
              new lambertian(new constant_texture(
                  vec3(RFG() * RFG(), RFG() * RFG(), RFG() * RFG()))));
        } else if (choose_mat < 0.95f)  // metal - chapter 8
          list[i++] = new sphere(
              center, 0.2f,
              new metal(
                  new constant_texture(vec3(0.5f * (1 + RFG()),
                                            0.5f * (1 + RFG()), 0.5f * RFG())),
                  0.5f));
        else  // dieletric/glass - chapter 9
          list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
      }
    }

  int nx, ny, nn;
  unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
  list[i++] = new sphere(vec3(4, 1, 0), 1.0f,
                         new lambertian(new image_texture(tex_data, nx, ny)));
  list[i++] = new sphere(vec3(0, 1, 0), 1.0f, new dielectric(1.5f));
  list[i++] =
      new sphere(vec3(-4, 1, 0), 1.0f,
                 new metal(new constant_texture(vec3(0.7f, 0.6f, 0.5f)), 0.0f));

  return new bvh_node(list, i, 0.0f, 1.0f);
}

hitable *model_scene() {
  hitable **list =
      new hitable *[50000];  // only works if face count is under 50k
  list[0] =
      new sphere(vec3(0, -1000, 0), 1000,
                 new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
  int l = 1;
  float scale = 1.0;

  // add model
  list[l++] = new translate(load_and_convert("box_stack.obj", "assets/", 1.0),
                            vec3(0, 0, 0));

  // list[i++] = new translate(load_and_convert("box_stack.obj", "assets/",
  // 0.2), vec3(20, 0, 0));

  return new bvh_node(list, l, 0.0, 1.0);
}

hitable *light_scene_1() {
  hitable **list = new hitable *[5];
  list[0] =
      new sphere(vec3(0, -1000, 0), 1000,
                 new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
  int i = 1;

  // add spheres
  int nx, ny, nn;
  unsigned char *tex_data_globe = stbi_load("earth.jpg", &nx, &ny, &nn, 0);

  // checker texture
  texture *globe = new image_texture(tex_data_globe, nx, ny);
  texture *white = new constant_texture(vec3(4, 4, 4));
  texture *checker = new checker_texture(globe, white);

  list[i++] = new sphere(vec3(4, 1, 0), 1.0, new diffuse_light(globe));
  list[i++] = new sphere(vec3(0, 1, 0), 1.0, new diffuse_light(checker));
  list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new diffuse_light(white));

  return new bvh_node(list, i, 0.0, 1.0);
}

hitable *light_scene_2() {
  hitable **list = new hitable *[4];

  texture *green = new constant_texture(vec3(0, 128 / 255.0f, 0));
  texture *red = new constant_texture(vec3(128 / 255.0f, 0, 0));

  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(green));
  list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(red));
  list[2] = new sphere(vec3(0, 7, 0), 2,
                       new diffuse_light(new constant_texture(vec3(4, 4, 4))));
  list[3] = new aarect(3, 5, 1, 3, -2, 'z',
                       new diffuse_light(new constant_texture(vec3(4, 4, 4))));

  return new bvh_node(list, 4, 0.0f, 1.0f);
}

hitable *medium_scene_1() {
  hitable **list = new hitable *[4];

  int l = 0;

  // list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new
  // constant_texture(vec3(0.5, 0.5, 0.5))));

  // add light
  material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
  list[l++] = new aarect(123, 423, 147, 412, 554, 'y', light);

  // ground sphere
  list[l++] =
      new sphere(vec3(278, -930, 145), 1000,
                 new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));

  // sub surface
  hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
  list[l++] = boundary;
  list[l++] = new constant_medium(boundary, 0.2f,
                                  new constant_texture(vec3(0.2f, 0.4f, 0.9f)));

  // mist?
  /*boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
  list[l++] = new constant_medium(boundary, 0.0001, new
  constant_texture(vec3(1.0, 1.0, 1.0)));*/

  return new bvh_node(list, l, 0.0f, 1.0f);
}

hitable *cornell_box() {
  hitable **list = new hitable *[8];
  int i = 0;

  material *red =
      new lambertian(new constant_texture(vec3(0.65f, 0.05f, 0.05f)));
  material *white =
      new lambertian(new constant_texture(vec3(0.73f, 0.73f, 0.73f)));
  material *green =
      new lambertian(new constant_texture(vec3(0.12f, 0.45f, 0.15f)));
  material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));

  list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'x', green));
  list[i++] = new aarect(0, 555, 0, 555, 0, 'x', red);
  list[i++] = new flip_normals(new aarect(213, 343, 227, 332, 554, 'y', light));
  list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'y', white));
  list[i++] = new aarect(0, 555, 0, 555, 0, 'y', white);
  list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'z', white));

  list[i++] = new translate(
      new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
      vec3(130, 0, 65));
  list[i++] = new translate(
      new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15),
      vec3(265, 0, 295));

  return new bvh_node(list, i, 0.0, 1.0);
}

hitable *cornell_metal_box() {
  int i = 0;
  hitable **list = new hitable *[9];

  material *red =
      new lambertian(new constant_texture(vec3(0.65f, 0.05f, 0.05f)));
  material *white =
      new lambertian(new constant_texture(vec3(0.73f, 0.73f, 0.73f)));
  material *green =
      new lambertian(new constant_texture(vec3(0.12f, 0.45f, 0.15f)));
  material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
  material *glass = new dielectric(1.5f);
  material *aluminum =
      new metal(new constant_texture(vec3(0.8f, 0.85f, 0.88f)), 0.0f);

  list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'x', green));
  list[i++] = new aarect(0, 555, 0, 555, 0, 'x', red);
  list[i++] = new flip_normals(new aarect(213, 343, 227, 332, 554, 'y', light));
  list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'y', white));
  list[i++] = new aarect(0, 555, 0, 555, 0, 'y', white);
  list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'z', aluminum));

  // list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165,
  // 430, 165), aluminum), 15), vec3(265, 0, 295));
  list[i++] = new sphere(vec3(190, 90, 190), 90, glass);
  list[i++] =
      new translate(new rotate_y(load_and_convert("nam.obj", "", 700.0), 180),
                    vec3(350, 0, 0));

  list[i++] = new sphere(vec3(278, 278, -900), 90, light);

  // list[i++] = new translate(new rotate_z(new rotate_x(new rotate_y(new
  // box(vec3(0, 0, 0), vec3(165, 165, 165), aluminum), -18), -18), -18),
  // vec3(230, 200, 65));

  return new bvh_node(list, i, 0.0, 1.0);
}

hitable *two_perlin_spheres() {
  hitable **list = new hitable *[3];
  material *noisy = new lambertian(new noise_texture(4.0));

  list[0] = new sphere(vec3(0, -1000, 0), 1000, noisy);
  list[1] = new sphere(vec3(0, 2, 0), 2, noisy);
  list[2] = new aarect(3, 5, 1, 3, -2, 'z',
                       new diffuse_light(new constant_texture(vec3(4, 4, 4))));

  return new bvh_node(list, 3, 0.0, 1.0);
}

hitable *bvh_debug() {
  hitable **node_list = new hitable *[2];

  texture *green = new constant_texture(vec3(0, 128 / 255.0f, 0));
  texture *red = new constant_texture(vec3(128 / 255.0f, 0, 0));

  node_list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(green));
  node_list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(red));

  return new bvh_node(node_list, 2, 0.0f, 1.0f);
}

hitable * final(std::string filename, std::string assets_folder) {
  // hitable lists
  hitable **list = new hitable *[30];
  hitable **boxlist = new hitable *[10000];
  hitable **boxlist2 = new hitable *[10000];

  // materials
  material *white =
      new lambertian(new constant_texture(vec3(0.73f, 0.73f, 0.73f)));
  material *ground =
      new lambertian(new constant_texture(vec3(0.48f, 0.83f, 0.53f)));
  material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
  material *brown =
      new lambertian(new constant_texture(vec3(0.7f, 0.3f, 0.1f)));

  int l = 0;

  // light panel
  list[l++] = new aarect(123, 423, 147, 412, 554, 'y', light);

  // boxes/ground
  int b = 0;
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 20; ++j) {
      float w = 100;
      float x0 = -1000 + i * w;
      float z0 = -1000 + j * w;
      float y0 = 0;
      float x1 = x0 + w;
      float y1 = 100 * (RFG() + 0.01f);
      float z1 = z0 + w;

      boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
    }
  }
  list[l++] = new bvh_node(boxlist, b, 0.0f, 1.0f);

  // perlin sphere
  list[l++] = new rotate_y(new sphere(vec3(220, 280, 300), 80,
                                      new lambertian(new noise_texture(0.1f))),
                           15);

  vec3 center(400, 400, 200);

  // moving sphere
  list[l++] =
      new moving_sphere(center, center + vec3(30, 0, 0), 0, 1, 50, brown);

  // glass sphere
  list[l++] = new sphere(vec3(220, 150, 145), 50, new dielectric(1.5));

  // metal sphere
  list[l++] =
      new sphere(vec3(0, 150, 145), 50,
                 new metal(new constant_texture(vec3(0.8f, 0.8f, 0.9f)), 10.0));

  // add models
  list[l++] = new translate(load_and_convert(filename, assets_folder, 7.0f),
                            vec3(130, 100, 145));
  // list[l++] = new translate(new rotate_y(load_and_convert("newcottage.obj",
  // assets_folder, 5), -35), vec3(480, 80, 40));
  list[l++] = new translate(
      new rotate_y(load_and_convert("teapot.obj", assets_folder, 1.0f), 45),
      vec3(130, 100, 10));

  // subsurface sphere
  hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
  list[l++] = boundary;
  list[l++] = new constant_medium(boundary, 0.2f,
                                  new constant_texture(vec3(0.2f, 0.4f, 0.9f)));

  // fog
  boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5f));
  list[l++] = new constant_medium(boundary, 0.0001f,
                                  new constant_texture(vec3(1.0f, 1.0f, 1.0f)));

  // earth
  int nx, ny, nn;
  unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
  list[l++] = new translate(
      new rotate_y(
          new sphere(vec3(500, 200, 400), 100,
                     new lambertian(new image_texture(tex_data, nx, ny))),
          15),
      vec3(-200, 0, 150));

  // box of spheres
  for (int j = 0; j < 1000; ++j)
    boxlist2[j] =
        new sphere(vec3(165 * RFG(), 165 * RFG(), 165 * RFG()), 10, white);
  list[l++] =
      new translate(new rotate_y(new bvh_node(boxlist2, 1000, 0.0, 1.0), 15),
                    vec3(-100, 270, 395));

  return new bvh_node(list, l, 0.0f, 1.0f);
}

#endif