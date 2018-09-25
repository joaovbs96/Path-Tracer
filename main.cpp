#include <iostream>
#include <fstream>
#include <strstream>
#include <float.h>
#include <random>

#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"

#include "lambertian.h"
#include "metal.h"
#include "dieletric.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

std::random_device device;
std::mt19937 gen(device());
std::uniform_real_distribution<float> dist(0.0, 1.0);

vec3 color(const ray& r, hitable *world, int depth) {
	hit_record rec;

	// world->hit iterate through scene objs to find if there was a hit/intersection.
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		// if there's a hit, ray can be scattered or absorbed.
		ray scattered;
		vec3 attenuation;

		// Ray is scattered/dispersed. Scatter behavior is material dependent.
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1); // recursive/bounce
		}
		else { // ray is fully absorbed.
			return vec3(0, 0, 0);
		}
	}
	else { // if the ray doesn't intersect anything, return 'background color'.
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

hitable *random_scene() {
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = dist(gen);
			vec3 center(a + 0.9 * dist(gen), 0.2, b + 0.9 * dist(gen));

			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) { // diffuse/lambertian - chapter 7
					list[i++] = new sphere(center, 0.2, new lambertian(vec3(dist(gen) * dist(gen), dist(gen) * dist(gen), dist(gen) * dist(gen))));
				}
				else if (choose_mat < 0.95) { // metal - chapter 8
					list[i++] = new sphere(center, 0.2, new metal(vec3(0.5 * (1 + dist(gen)), 0.5*(1 + dist(gen)), 0.5*dist(gen)), 0.5));
				}
				else { // dieletric/glass - chapter 9
					list[i++] = new sphere(center, 0.2, new dieletric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dieletric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list, i);
}

int main() {
	// main parameters
	int w = 1280;
	int h = 720;
	int samples = 50;

	// pixel array
	unsigned char *arr = (unsigned char *)malloc(w * h * 3 * sizeof(unsigned char));

	// create scene
	hitable *world = random_scene();

	// setting up camera
	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.1;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(w)/float(h), aperture, dist_to_focus);

	// render loop for each pixel
	for (int j = h - 1; j >= 0; j--) {
		for (int i = 0; i < w; i++) {
			vec3 col(0, 0, 0);

			// for each sample
			for (int s = 0; s < samples; s++) {
				// each sample has a random offset from 
				// the pixel's lower left corner, obtained by:
				float u = float(i + dist(gen)) / float(w);
				float v = float(j + dist(gen)) / float(h);

				// (u, v) are always inside the pixel (i, j)

				// trace a ray
				ray r = cam.get_ray(u, v);
				
				//vec3 p = r.point_at_parameter(2.0);
				
				// get color of ray's 'destination'
				col += color(r, world, 0);
			}

			// average sample values
			col = vec3(col[0] / float(samples), col[1] / float(samples), col[2] / float(samples));

			// gamma correction
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

			// save value at pixel array
			arr[(h - j - 1)*3*w + 3*i + 0] = int(255.99 * col[0]); // red
			arr[(h - j - 1)*3*w + 3*i + 1] = int(255.99 * col[1]); // green
			arr[(h - j - 1)*3*w + 3*i + 2] = int(255.99 * col[2]); // blue

		}

		// print progress
		std::cout << ((h - j) * 100) / float(h) << "%" << std::endl;
	}

	// save image
	stbi_write_png("img.png", w, h, 3, arr, 0);

	return 0;
}