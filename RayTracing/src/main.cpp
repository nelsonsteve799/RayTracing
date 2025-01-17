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

hitable* random_scene() {
	int n = 500;
	hitable** list = new hitable * [n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			real random_number = ((real)rand() / (RAND_MAX));
			real choose_mat = random_number;
			real rand1 = ((real)rand() / (RAND_MAX));
			real rand2 = ((real)rand() / (RAND_MAX));
			vec3 center(a + 0.9 * rand1, 0.2, b + 0.9 * rand2);
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {
					rand1 = ((real)rand() / (RAND_MAX));
					rand2 = ((real)rand() / (RAND_MAX));
					real rand3 = ((real)rand() / (RAND_MAX));
					real rand4 = ((real)rand() / (RAND_MAX));
					real rand5 = ((real)rand() / (RAND_MAX));
					real rand6 = ((real)rand() / (RAND_MAX));
					list[i++] = new sphere(center, 0.2, new lambertian(vec3(rand1 * rand2, rand3 * rand4, rand5 * rand6)));
				}
				else if (choose_mat < 0.95) {
					rand1 = ((real)rand() / (RAND_MAX));
					rand2 = ((real)rand() / (RAND_MAX));
					real rand3 = ((real)rand() / (RAND_MAX));
					list[i++] = new sphere(center, 0.2, new metal(vec3(0.5 * (1 + rand1), 0.5 * (1 + rand2), 0.5 * (1 + rand3))));
				}
			}
			else {
				list[i++] = new sphere(center, 0.2, new dielectric(1.5));
			}
		}
	}
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5)));

	return new hitable_list(list, i);
}

int main(int argc, char** argv[]) {
	int multiple = 4;
	int nx = multiple * 200;
	int ny = multiple * 100;
	int ns = multiple * 100;
	std::ofstream img("picture.ppm");
	//hitable* list[6];
	//list[0] = new sphere(vec3(0, 0, -2), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
	//list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	//list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2)));
	//list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
	//list[5] = new sphere(vec3(0, 0.0, -1), 0.5, new lambertian(vec3(0.5, 0.0, 0.5)));
	//list[4] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
	//hitable* world = new hitable_list(list, 6);
	hitable* world = random_scene();
	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	//real dist_to_focus = (lookfrom - lookat).length();
	real dist_to_focus = 10.0;
	real aperture = 0.1;
	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, real(nx) / real(ny), aperture, dist_to_focus);
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