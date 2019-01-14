#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>
#include <thread>
#include <ctime>
#include <sstream>
#include <mutex>

#include "camera.h"

#include "cosine_pdf.h"
#include "hitable_pdf.h"
#include "mixture_pdf.h"

#include "bvh_node.h"
#include "hitable_list.h"
#include "moving_sphere.h"
#include "sphere.h"
#include "mesh.h"
#include "aarect.h"
#include "constant_medium.h"
#include "flip_normals.h"
#include "box.h"
#include "rotate.h"
#include "translate.h"

#include "lambertian.h"
#include "metal.h"
#include "diffuse_light.h"
#include "isotropic.h"
#include "dielectric.h"

#include "constant.h"
#include "image.h"
#include "checker.h"
#include "noise_texture.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "OBJ_Loader.h"

std::mutex mtx;
float completed_threads;

vec3 color(const ray& r, hitable *world, hitable *light_shape, int depth, bool light) {
	hit_record hrec;

	// world->hit iterate through scene objs to find if there was a hit/intersection.
	if (world->hit(r, 0.001, FLT_MAX, hrec)) {
		// if there's a hit, ray can be scattered or absorbed.
		scatter_record srec;
		vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);

		// Ray is scattered/dispersed. Scatter behavior is material dependent.
		if (depth < 50 && hrec.mat_ptr->scatter(r, hrec, srec)) {
			
			if (srec.is_specular)
				return srec.attenuation * color(srec.specular_ray, world, light_shape, depth + 1, light);
			else {
				hitable_pdf plight(light_shape, hrec.p);
				mixture_pdf p(&plight, srec.pdf_ptr);

				ray scattered = ray(hrec.p, p.generate(), r.time());
				float pdf = p.value(scattered.direction());
				delete srec.pdf_ptr;

				return emitted + srec.attenuation * hrec.mat_ptr->scattering_pdf(r, hrec, scattered)
					* color(scattered, world, light_shape, depth + 1, light) / pdf; // recursive/bounce
			}
		}
		else // ray is fully absorbed.
			return emitted;

	}
	else { // if the ray doesn't intersect anything, return 'background color'.
		
		if (light) { // sky
			vec3 unit_direction = unit_vector(r.direction());
			float t = 0.5 * (unit_direction.y() + 1.0);
			return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
		}
		else // dark background
			return vec3(0, 0, 0);
	}
}

hitable *load_and_convert(std::string filename, std::string assets_folder, float scale = 1.0) {
	hitable **list = new hitable*[50000]; // TODO: usar vectors
	int l = 0;
	objl::Loader Loader;

	std::cout << "Loading Model" << std::endl;
	bool loadout = Loader.LoadFile((char*)filename.c_str());
	std::cout << std::endl << "Model Loaded" << std::endl << std::endl;

	// Check if obj was loaded
	if (loadout) {

		// Go through each loaded mesh
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {
			// Copy one of the loaded meshes to be our current mesh
			std::cout << '\r' << "                         " << '\r'; // flush line
			std::cout << "Converting Mesh " << (i + 1) << "/" << Loader.LoadedMeshes.size();
			objl::Mesh curMesh = Loader.LoadedMeshes[i];

			// TODO: e se não tiver MTL?

			// get mtl material for mesh
			material *m;
			// if there's no diffuse map, it's a constant color texture
			if (curMesh.MeshMaterial.map_Kd.empty()) {
				m = new lambertian(new constant_texture(
					vec3(curMesh.MeshMaterial.Kd.X, curMesh.MeshMaterial.Kd.Y, curMesh.MeshMaterial.Kd.Z)));

				// Go through every 3rd index and print the
				// save that these indices represent
				for (int j = 0; j < curMesh.Indices.size(); j += 3) {
					int ia = curMesh.Indices[j];
					vec3 a(
						scale * curMesh.Vertices[ia].Position.X,
						scale * curMesh.Vertices[ia].Position.Y,
						scale * curMesh.Vertices[ia].Position.Z,
						0.0, // optional parameters u
						0.0); // optional parameters v

					int ib = curMesh.Indices[j + 1];
					vec3 b(
						scale * curMesh.Vertices[ib].Position.X,
						scale * curMesh.Vertices[ib].Position.Y,
						scale * curMesh.Vertices[ib].Position.Z,
						0.0, // optional parameters u
						0.0); // optional parameters v

					int ic = curMesh.Indices[j + 2];
					vec3 c(
						scale * curMesh.Vertices[ic].Position.X,
						scale * curMesh.Vertices[ic].Position.Y,
						scale * curMesh.Vertices[ic].Position.Z,
						0.0, // optional parameters u
						0.0); // optional parameters v

					vec3 n = cross(b - a, c - a);

					list[l++] = new triangle(a, b, c, n, m, scale);
				}
			}
			else { // otherwise, it's an image texture
				int nx, ny, nn;
				char *fileName = (char*)curMesh.MeshMaterial.map_Kd.c_str();
				unsigned char *tex_data = stbi_load(fileName, &nx, &ny, &nn, 0);
				m = new lambertian(new image_texture(tex_data, nx, ny));

				// Go through every 3rd index and print the
				// save that these indices represent
				for (int j = 0; j < curMesh.Indices.size(); j += 3) {
					int ia = curMesh.Indices[j];
					vec3 a(
						scale * curMesh.Vertices[ia].Position.X,
						scale * curMesh.Vertices[ia].Position.Y,
						scale * curMesh.Vertices[ia].Position.Z,
						curMesh.Vertices[ia].TextureCoordinate.X, // optional parameters u
						curMesh.Vertices[ia].TextureCoordinate.Y); // optional parameters v

					int ib = curMesh.Indices[j + 1];
					vec3 b(
						scale * curMesh.Vertices[ib].Position.X,
						scale * curMesh.Vertices[ib].Position.Y,
						scale * curMesh.Vertices[ib].Position.Z,
						curMesh.Vertices[ib].TextureCoordinate.X, // optional parameters u
						curMesh.Vertices[ib].TextureCoordinate.Y); // optional parameters v

					int ic = curMesh.Indices[j + 2];
					vec3 c(
						scale * curMesh.Vertices[ic].Position.X,
						scale * curMesh.Vertices[ic].Position.Y,
						scale * curMesh.Vertices[ic].Position.Z,
						curMesh.Vertices[ic].TextureCoordinate.X, // optional parameters u
						curMesh.Vertices[ic].TextureCoordinate.Y); // optional parameters v

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
		std::cout << "Failed to Load File. May have failed to find it or it was not an .obj file." << std::endl;
		return NULL;
	}


}

hitable *random_scene() {
	int n = 50000;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
	int i = 1;

	// add spheres
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = RFG();
			vec3 center(a + 0.9 * RFG(), 0.2, b + 0.9 * RFG());

			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {

				if (choose_mat < 0.8) { // diffuse/lambertian - chapter 7
					list[i++] = new sphere(center, 0.2, new lambertian(new constant_texture(
						vec3(RFG() * RFG(), RFG() * RFG(), RFG() * RFG()))));
				}
				else if (choose_mat < 0.95) { // metal - chapter 8
					list[i++] = new sphere(center, 0.2, new metal(new constant_texture(
						vec3(0.5 * (1 + RFG()), 0.5 * (1 + RFG()), 0.5 * RFG())), 0.5));
				}
				else { // dieletric/glass - chapter 9
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}

	//int nx, ny, nn;
	//unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new metal(new constant_texture(vec3(0.7, 0.6, 0.5)), 0.0));

	return new bvh_node(list, i, 0.0, 1.0);
}

hitable *random_scene_movement() {
	int n = 50000;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
	int i = 1;

	// add spheres
	for (int a = -10; a < 10; a++)
		for (int b = -10; b < 10; b++) {
			float choose_mat = RFG();
			vec3 center(a + 0.9 * RFG(), 0.2, b + 0.9 * RFG());

			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {

				if (choose_mat < 0.8) { // diffuse/lambertian - chapter 7
					list[i++] = new moving_sphere(center, center + vec3(0, 0.5 * RFG(), 0), 0.0, 1.0, 0.2,
						new lambertian(new constant_texture(vec3(RFG() * RFG(), RFG() * RFG(), RFG() * RFG()))));
				}
				else if (choose_mat < 0.95) // metal - chapter 8
					list[i++] = new sphere(center, 0.2, new metal(new constant_texture(
						vec3(0.5 * (1 + RFG()), 0.5 * (1 + RFG()), 0.5 * RFG())), 0.5));
				else // dieletric/glass - chapter 9
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
			}
		}

	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new lambertian(new image_texture(tex_data, nx, ny)));
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new metal(new constant_texture(vec3(0.7, 0.6, 0.5)), 0.0));

	return new bvh_node(list, i, 0.0, 1.0);
}

hitable *model_scene() {
	hitable **list = new hitable*[50000]; // only works if face count is under 50k
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
	int l = 1;
	float scale = 1.0;

	// add model
	list[l++] = new translate(load_and_convert("box_stack.obj", "assets/", 1.0), vec3(0, 0, 0));

	//list[i++] = new translate(load_and_convert("box_stack.obj", "assets/", 0.2), vec3(20, 0, 0));

	return new bvh_node(list, l, 0.0, 1.0);
}

hitable *light_scene_1() {
	hitable **list = new hitable*[5];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
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
	hitable **list = new hitable*[4];

	texture *green = new constant_texture(vec3(0, 128 / 255.0, 0));
	texture *red = new constant_texture(vec3(128 / 255.0, 0, 0));

	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(green));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(red));
	list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[3] = new aarect(3, 5, 1, 3, -2, 'z', new diffuse_light(new constant_texture(vec3(4, 4, 4))));

	return new bvh_node(list, 4, 0.0, 1.0);
}

hitable *medium_scene_1() {
	hitable **list = new hitable*[4];

	int l = 0;

	//list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));

	// add light
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	list[l++] = new aarect(123, 423, 147, 412, 554, 'y', light);

	// ground sphere
	list[l++] = new sphere(vec3(278, -930, 145), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));

	// sub surface
	hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));

	// mist?
	/*boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
	list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));*/

	return new bvh_node(list, l, 0.0, 1.0);
}

hitable *cornell_box() {
	hitable **list = new hitable*[8];
	int i = 0;

	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));

	list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'x', green));
	list[i++] = new aarect(0, 555, 0, 555, 0, 'x', red);
	list[i++] = new flip_normals(new aarect(213, 343, 227, 332, 554, 'y', light));
	list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'y', white));
	list[i++] = new aarect(0, 555, 0, 555, 0, 'y', white);
	list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'z', white));

	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

	return new bvh_node(list, i, 0.0, 1.0);
}

hitable *cornell_metal_box() {
	int i = 0;
	hitable **list = new hitable*[9];

	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	material *glass = new dielectric(1.5);
	material *aluminum = new metal(new constant_texture(vec3(0.8, 0.85, 0.88)), 0.0);

	list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'x', green));
	list[i++] = new aarect(0, 555, 0, 555, 0, 'x', red);
	list[i++] = new flip_normals(new aarect(213, 343, 227, 332, 554, 'y', light));
	list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'y', white));
	list[i++] = new aarect(0, 555, 0, 555, 0, 'y', white);
	list[i++] = new flip_normals(new aarect(0, 555, 0, 555, 555, 'z', aluminum));
	
	//list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 430, 165), aluminum), 15), vec3(265, 0, 295));
	list[i++] = new sphere(vec3(190, 90, 190), 90, glass);
	list[i++] = new translate(new rotate_y(load_and_convert("nam.obj", "", 700.0), 180), vec3(350, 0, 0));

	list[i++] = new sphere(vec3(278, 278, -900), 90, light);

	//list[i++] = new translate(new rotate_z(new rotate_x(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), aluminum), -18), -18), -18), vec3(230, 200, 65));

	return new bvh_node(list, i, 0.0, 1.0);
}

hitable *two_perlin_spheres() {
	hitable **list = new hitable*[3];
	material *noisy = new lambertian(new noise_texture(4.0));

	list[0] = new sphere(vec3(0, -1000, 0), 1000, noisy);
	list[1] = new sphere(vec3(0, 2, 0), 2, noisy);
	list[2] = new aarect(3, 5, 1, 3, -2, 'z', new diffuse_light(new constant_texture(vec3(4, 4, 4))));

	return new bvh_node(list, 3, 0.0, 1.0);
}

hitable *bvh_debug() {
	hitable **node_list = new hitable*[2];

	texture *green = new constant_texture(vec3(0, 128 / 255.0, 0));
	texture *red = new constant_texture(vec3(128 / 255.0, 0, 0));

	node_list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(green));
	node_list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(red));

	return new bvh_node(node_list, 2, 0.0, 1.0);
}

hitable *final(std::string filename, std::string assets_folder) {
	// hitable lists
	hitable **list = new hitable*[30];
	hitable **boxlist = new hitable*[10000];
	hitable **boxlist2 = new hitable*[10000];

	// materials
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
	material *brown = new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1)));

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
			float y1 = 100 * (RFG() + 0.01);
			float z1 = z0 + w;

			boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
		}
	}
	list[l++] = new bvh_node(boxlist, b, 0.0, 1.0);

	// perlin sphere
	list[l++] = new rotate_y(new sphere(vec3(220, 280, 300), 80, new lambertian(new noise_texture(0.1))), 15);

	vec3 center(400, 400, 200);

	// moving sphere
	list[l++] = new moving_sphere(center, center + vec3(30, 0, 0), 0, 1, 50, brown);

	// glass sphere
	list[l++] = new sphere(vec3(220, 150, 145), 50, new dielectric(1.5));

	// metal sphere
	list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(new constant_texture(vec3(0.8, 0.8, 0.9)), 10.0));

	// add models
	list[l++] = new translate(load_and_convert(filename, assets_folder, 7.0), vec3(130, 100, 145));
	//list[l++] = new translate(new rotate_y(load_and_convert("newcottage.obj", assets_folder, 5), -35), vec3(480, 80, 40));
	list[l++] = new translate(new rotate_y(load_and_convert("teapot.obj", assets_folder, 1.0), 45), vec3(130, 100, 10));

	// subsurface sphere
	hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));

	// fog
	boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
	list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));

	// earth
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
	list[l++] = new translate(new rotate_y(new sphere(vec3(500, 200, 400), 100, new lambertian(new image_texture(tex_data, nx, ny))), 15), vec3(-200, 0, 150));

	// box of spheres
	for (int j = 0; j < 1000; ++j)
		boxlist2[j] = new sphere(vec3(165 * RFG(), 165 * RFG(), 165 * RFG()), 10, white);
	list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, 1000, 0.0, 1.0), 15), vec3(-100, 270, 395));

	return new bvh_node(list, l, 0.0, 1.0);
}

void thread_func(int i, int j, int w, int h, int samples, camera cam, hitable *world, hitable *light_source, unsigned char *arr, bool light) {
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

		// trace a ray
		ray r = cam.get_ray(u, v);

		// get color of ray's 'destination'
		col += de_nan(color(r, world, light_source, 0, light));
	}

	// average sample values
	col = vec3(col[0] / float(samples), col[1] / float(samples), col[2] / float(samples));

	// gamma correction
	col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

	// save value at pixel array
	arr[(h - j - 1) * 3 * w + 3 * i + 0] = int(255.99 * clamp(col.r())); // red
	arr[(h - j - 1) * 3 * w + 3 * i + 1] = int(255.99 * clamp(col.g())); // green
	arr[(h - j - 1) * 3 * w + 3 * i + 2] = int(255.99 * clamp(col.b())); // blue

	// print progress
	mtx.lock();
	printf("%f%%\n", (++completed_threads * 100.0) / (w * h));
	mtx.unlock();
}

void do_join(std::thread& t) {
	t.join();
}

int main() {
	auto start = std::chrono::system_clock::now();
	completed_threads = 0;

	// main parameters
	int samples = 4000;
	int scene = 9;
	bool light = true;
	std::string output = "output/cornell_nam_test_4000.png";

	// setting up camera
	// default values
	int w = 448;
	int h = 108;
	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 20.0;

	hitable * world; // create scene
	hitable *light_shape = new aarect(213, 343, 227, 332, 554, 'y', 0);
	hitable *glass_sphere = new sphere(vec3(190, 90, 190), 90, 0);
	hitable *a[2];
	a[0] = light_shape;
	a[1] = glass_sphere;
	hitable_list hlist(a, 2);

	switch (scene) {
	case 0: // In One Weekend test scene
		world = random_scene();
		break;

	case 1: // model loader test scene
		world = model_scene();
		break;

	case 2: // diffuse light test #1
		world = light_scene_1();
		light = false;
		break;

	case 3: // diffuse light test #2
		world = light_scene_2();
		light = false;
		break;

	case 4: // diffuse light test #2
		world = random_scene_movement();
		break;

	case 5: // constant medium
		world = medium_scene_1();

		w = 100;
		h = 100;
		lookfrom = vec3(278, 278, -800);
		lookat = vec3(278, 278, 0);

		break;

	case 6: // cornell box
		world = cornell_box();

		w = 300;
		h = 300;
		light = false;

		lookfrom = vec3(278, 278, -800);
		lookat = vec3(278, 278, 0);
		vfov = 40.0;

		break;

	case 7: // perlin spheres test scene
		light = true;

		world = two_perlin_spheres();
		break;

	case 8: // final scene
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

	default: // bvh debug scene
		lookfrom = vec3(30, 20, 3);
		world = bvh_debug();
	}

	// camera
	camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(w) / float(h), aperture, dist_to_focus, 0.0, 1.0);

	// pixel array
	unsigned char *arr = (unsigned char *)malloc(w * h * 3 * sizeof(unsigned char));

	// render loop for each pixel
	std::vector<std::thread> v;
	for (int j = h - 1; j >= 0; j--)
		for (int i = 0; i < w; i++)
			v.push_back(std::thread(thread_func, i, j, w, h, samples, cam, world, &hlist, arr, light));
	std::for_each(v.begin(), v.end(), do_join);

	// save image
	stbi_write_png((char*)output.c_str(), w, h, 3, arr, 0);

	// print execution time
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = end - start;
	std::cout << diff.count() << " s\n";

	std::system("PAUSE");

	return 0;
}