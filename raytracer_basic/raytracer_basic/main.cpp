#include <iostream>
#include <fstream>
#include <strstream>
#include "sphere.h"
#include "hitable_list.h"
#include <float.h>
#include "camera.h"
#include "material.h"
#include <random>

std::random_device device;
std::mt19937 gen(device());
std::uniform_real_distribution<float> dist(0.0, 1.0);

vec3 color(const ray& r, hitable *world, int depth) {
	hit_record rec;

	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}
	}
	else {
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
				if (choose_mat < 0.8) { // diffuse
					list[i++] = new sphere(center, 0.2, new lambertian(vec3(dist(gen) * dist(gen), dist(gen) * dist(gen), dist(gen) * dist(gen))));
				}
				else if (choose_mat < 0.95) { // metal
					list[i++] = new sphere(center, 0.2, new metal(vec3(0.5 * (1 + dist(gen)), 0.5*(1 + dist(gen)), 0.5*dist(gen)), 0.5));
				}
				else {
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
	std::ofstream img;
	int w = 20;
	int h = 10;
	int samples = 50;

	img.open("img.ppm");

	img << "P3" << std::endl;
	img << w << " " << h << std::endl;
	img << "255" << std::endl;

	/*hitable *list[5];
	list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.3));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dieletric(1.5));
	list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dieletric(1.5));
	
	hitable *world = new hitable_list(list, 5);*/

	hitable *world = random_scene();

	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.1;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(w)/float(h), aperture, dist_to_focus);

	for (int j = h - 1; j >= 0; j--) {
		for (int i = 0; i < w; i++) {
			vec3 col(0, 0, 0);

			for (int s = 0; s < samples; s++) {
				float u = float(i + dist(gen)) / float(w);
				float v = float(j + dist(gen)) / float(h);

				ray r = cam.get_ray(u, v);
				
				vec3 p = r.point_at_parameter(2.0);
				
				col += color(r, world, 0);
			}

			col = vec3(col[0] / float(samples), col[1] / float(samples), col[2] / float(samples));
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);

			img << ir << " " << ig << " " << ib << std::endl;
		}
		std::cout << ((h - j) * 100) / float(h) << "%" << std::endl;
	}

	img.close();

	return 0;
}