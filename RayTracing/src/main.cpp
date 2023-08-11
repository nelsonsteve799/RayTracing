//-----------------------------------------------------------------------//
#include "Math/vec3.h"
#include "ray.h"
#include "hitable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <iostream>
#include <fstream>
#include <cfloat>
#include <random>

Math::vec3 color(const ray& r, hitable* world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.0001f, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mtr_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}
	}
	else {
		vec3 unit_direction = unit_vector(r.direction());
		real t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
	}
}

int main(int argc, char** argv[]) {
	int nx = 200;
	int ny = 100;
	int ns = 100;
	std::ofstream img("picture.ppm");
	//vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
	//vec3 horizontal(4.0f, 0.0f, 0.0f);
	//vec3 vertical(0.0f, 2.0f, 0.0f);
	//vec3 origin(0.0f, 0.0f, 0.0f);
	hitable* list[4];
	list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2)));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8)));
	hitable* world = new hitable_list(list, 4);
	camera cam;
	if (img.is_open()) {
		img << "P3" << std::endl << nx << " " << ny << std::endl << "255" << std::endl;
		for (int j = ny - 1; j >= 0; j--) {
			//std::cout << "progress\n";
;			for (int i = 0; i < nx; i++) {
				vec3 col(0, 0, 0);
				for (int s = 0; s < ns; s++) {
					real random_number = ((real)rand() / (RAND_MAX)) + 1;
					real u = real(i + random_number) / real(nx);
					random_number = ((real)rand() / (RAND_MAX)) + 1;
					real v = real(j + random_number) / real(ny);
					ray r = cam.get_ray(u, v);
					vec3 p = r.point_at_parameter(2.0);
					col += color(r, world, 0);
				}
				col /= real(ns);
				col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
				int ir = int(255.99 * col[0]);
				int ig = int(255.99 * col[1]);
				int ib = int(255.99 * col[2]);

				img << ir << " " << ig << " " << ib << std::endl;
			}
		}
	}
	img.close();
}