#include <iostream>
#include <cstdlib>
#include <cmath>
#include <random>

#include "Sphere.h"
#include "MovingSphere.h"
#include "HitableList.h"
#include "Material.h"
#include "Camera.h"
#include "float.h"
#include "BVH.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "surfaceTexture.h"
#include "aarect.h"
#include "Box.h"
#include "constantMedium.h"
#include "pdf.h"


inline vec3 de_nan(const vec3& c)
{
	vec3 temp = c;
	if (!(temp[0] == temp[0])) temp[0] = 0;
	if (!(temp[1] == temp[1])) temp[1] = 0;
	if (!(temp[2] == temp[2])) temp[2] = 0;
	return temp;
}


vec3 color(const ray& r, Hitable* world, Hitable* light_shape, int depth) {
	hit_record hrec;
	if (world->hit(r, 0.001, FLT_MAX, hrec)) {
		scatter_record srec;
		vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
		if (depth < 50 && hrec.mat_ptr->Scatter(r, hrec, srec))
		{
			if (srec.is_specular)
			{
				return srec.attenuation * color(srec.specular_ray, world, light_shape, depth + 1);
			}
			else
			{
				hitablePDF plight(light_shape, hrec.p);
				mixturePDF p(&plight, srec.pdf_ptr);
				ray scattered = ray(hrec.p, p.generate(), r.time());
				float pdf_value = p.value(scattered.direction());
				delete srec.pdf_ptr;
				return emitted + srec.attenuation * hrec.mat_ptr->scatteringPdf(r, hrec, scattered) * color(scattered, world, light_shape, depth + 1) / pdf_value;
			}
			
		}
		else
			return emitted;
	}
	else
		return vec3(0, 0, 0);
}

Hitable* randomScene()
{
	int n = 500;
	Hitable** list = new Hitable * [n + 1];
	Texture* checker = new checkerTexture(new constantTexture(vec3(0.2, 0.3, 0.1)), new constantTexture(vec3(0.9, 0.9, 0.9)));
	list[0] = new Sphere(vec3(0, -1000, 0), 1000, new Lambertian(checker));
	int i = 1;
	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float choose_mat = (rand() / (RAND_MAX + 1.0));
			vec3 center(a + 0.9 * (rand() / (RAND_MAX + 1.0)), 0.2, b + 0.9 * (rand() / (RAND_MAX + 1.0)));
			if ((center - vec3(4, 0, 2.0)).length() > 0.9)
			{
				if (choose_mat < 0.8)
				{
					
					list[i++] = new MovingSphere(center, center+vec3(0,0.5* (rand() / (RAND_MAX + 1.0)), 0), 0.0, 1.0, 0.2, new Lambertian(new constantTexture(vec3(((rand() / (RAND_MAX + 1.0)) * (rand() / (RAND_MAX + 1.0))), ((rand() / (RAND_MAX + 1.0)) * (rand() / (RAND_MAX + 1.0))), ((rand() / (RAND_MAX + 1.0)) * (rand() / (RAND_MAX + 1.0)))))));
				}
				else if (choose_mat < 0.95)
				{
					
					list[i++] = new Sphere(center, 0.2, new Metal(vec3(0.5 * (1 + (rand() / (RAND_MAX + 1.0))), 0.5 * (1 + (rand() / (RAND_MAX + 1.0))), 0.5 * (1 + (rand() / (RAND_MAX + 1.0)))), 0.5 * (rand() / (RAND_MAX + 1.0))));
				}
				else
				{
					list[i++] = new Sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
		
	}

	list[i++] = new Sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new Sphere(vec3(-4, 1, 0), 1.0, new Lambertian(new constantTexture(vec3(0.4, 0.2, 0.1))));
	list[i++] = new Sphere(vec3(4, 1, 0), 1.0, new Metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new HitableList(list, i);
	
}


Hitable* two_spheres()
{
	Texture* checker = new checkerTexture(new constantTexture(vec3(0.2, 0.3, 0.1)), new constantTexture(vec3(0.9, 0.9, 0.9)));
	int n = 50;
	Hitable** list = new Hitable* [n + 1];
	list[0] = new Sphere(vec3(0, -10, 0), 10, new Lambertian(checker));
	list[1] = new Sphere(vec3(0,  10, 0), 10, new Lambertian(checker));
	return new HitableList(list, 2);
}

Hitable* two_perlin_spheres()
{
	Texture* perlin_texture = new noiseTexture(4);
	Hitable** list = new Hitable * [2];
	list[0] = new Sphere(vec3(0, -1000, 0), 1000, new Lambertian(perlin_texture));
	list[1] = new Sphere(vec3(0, 2, 0), 2, new Lambertian(perlin_texture));
	return new HitableList(list, 2);
}

Hitable* earth()
{
	int nx, ny, nn;
	unsigned char* tex_data = stbi_load("C:/Graphics/Ray Tracing 2/Ray Tracing The Next Week/earthmap.jpg", &nx, &ny, &nn, 0);
	Material* mat = new Lambertian(new imageTexture(tex_data, nx, ny));
	return new Sphere(vec3(0, 0, 0), 2, mat);
}

Hitable* simple_light()
{
	Texture* perlin_texture = new noiseTexture(4);
	Hitable** list = new Hitable* [4];
	list[0] = new Sphere(vec3(0, -1000, 0), 1000, new Lambertian(perlin_texture));
	list[1] = new Sphere(vec3(0, 2, 0), 2, new Lambertian(perlin_texture));
	list[2] = new Sphere(vec3(0, 7, 0), 2, new diffuseLight(new constantTexture(vec3(4, 4, 4))));
	list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuseLight(new constantTexture(vec3(4, 4, 4))));
	return new HitableList(list, 4);
}

void cornell_box(Hitable** scene, Camera** cam, float aspect)
{
	Hitable** list = new Hitable * [8];
	int i = 0;
	Material* red = new Lambertian(new constantTexture(vec3(0.65, 0.05, 0.05)));
	Material* white = new Lambertian(new constantTexture(vec3(0.73, 0.73, 0.73)));
	Material* green = new Lambertian(new constantTexture(vec3(0.12, 0.45, 0.15)));
	Material* light = new diffuseLight(new constantTexture(vec3(15, 15, 15)));

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	//Material* glass = new dielectric(1.5);
	list[i++] = new translate(new rotate_y(new Box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	Material* aluminum = new Metal(vec3(0.8, 0.85, 0.88), 0.0);
	//list[i++] = new Sphere(vec3(190, 90, 190), 90, white);
	list[i++] = new translate(new rotate_y(new Box(vec3(0, 0, 0), vec3(165, 330, 165), aluminum), 15), vec3(265, 0, 295));
	*scene = new HitableList(list, i);
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	*cam = new Camera(lookfrom, lookat, vec3(0, 1, 0), vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}

Hitable* cornell_smoke() {
	Hitable** list = new Hitable * [8];
	int i = 0;
	Material* red = new Lambertian(new constantTexture(vec3(0.65, 0.05, 0.05)));
	Material* white = new Lambertian(new constantTexture(vec3(0.73, 0.73, 0.73)));
	Material* green = new Lambertian(new constantTexture(vec3(0.12, 0.45, 0.15)));
	Material* light = new diffuseLight(new constantTexture(vec3(7, 7, 7)));
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	Hitable* b1 = new translate(new rotate_y(new Box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	Hitable* b2 = new translate(new rotate_y(new Box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
	list[i++] = new constantMedium(b1, 0.01, new constantTexture(vec3(1.0, 1.0, 1.0)));
	list[i++] = new constantMedium(b2, 0.01, new constantTexture(vec3(0.0, 0.0, 0.0)));
	return new HitableList(list, i);
}

Hitable* final()
{
	int nb = 20;
	Hitable** list = new Hitable * [30];
	Hitable** box_list = new Hitable * [10000];
	Hitable** box_list2 = new Hitable * [10000];
	Material* white = new Lambertian(new constantTexture(vec3(0.73, 0.73, 0.73)));
	Material* ground = new Lambertian(new constantTexture(vec3(0.48, 0.83, 0.53)));

	int b = 0;
	for (int i = 0; i < nb; i++)
	{
		for (int j = 0; j < nb; j++)
		{
			float w = 100;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100 * ((rand() / (RAND_MAX + 1.0)) + 0.01);
			float z1 = z0 + w;
			box_list[b++] = new Box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
		}
	}
	int l = 0;
	list[l++] = new BVHNode(box_list, b, 0, 1);
	Material* light = new diffuseLight(new constantTexture(vec3(7, 7, 7)));
	list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
	vec3 center(400, 400, 200);
	list[l++] = new MovingSphere(center, center + vec3(30, 0, 0), 0, 1, 50, new Lambertian(new constantTexture(vec3(0.7, 0.3, 0.1))));
	list[l++] = new Sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
	list[l++] = new Sphere(vec3(0, 150, 145), 50, new Metal(vec3(0.8, 0.8, 0.9), 10.0));
	Hitable* boundary = new Sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constantMedium(boundary, 0.2, new constantTexture(vec3(0.2, 0.4, 0.9)));
	boundary = new Sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
	list[l++] = new constantMedium(boundary, 0.0001, new constantTexture(vec3(1.0, 1.0, 1.0)));
	int nx, ny, nn;
	unsigned char* tex_data = stbi_load("C:/Graphics/Ray Tracing 2/Ray Tracing The Next Week/earthmap.jpg", &nx, &ny, &nn, 0);
	Material* emat = new Lambertian(new imageTexture(tex_data, nx, ny));
	list[l++] = new Sphere(vec3(400, 200, 400), 100, emat);
	Texture* perlin_texture = new noiseTexture(0.1);
	list[l++] = new Sphere(vec3(220, 280, 300), 80, new Lambertian(perlin_texture));
	int ns = 1000;
	for (int j = 0; j < ns; j++)
	{
		box_list2[j] = new Sphere(vec3(165 * (rand() / (RAND_MAX + 1.0)), 165 * (rand() / (RAND_MAX + 1.0)), 165 * (rand() / (RAND_MAX + 1.0))), 10, white);
	}
	list[l++] = new translate(new rotate_y(new BVHNode(box_list2, ns, 0.0, 1.0), 15), vec3(-100, 270, 395));
	return new HitableList(list, l);

}

int main()
{
	
	int nx = 500;
	int ny = 500;
	int ns = 1000;
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	Hitable* world;
	Camera* cam;
	float aspect = float(ny) / float(nx);
	cornell_box(&world, &cam, aspect);
	Hitable* light_shape = new xz_rect(213, 343, 227, 332, 554, 0);
	Hitable* glass_sphere = new Sphere(vec3(190, 90, 190), 90, 0);
	Hitable* a[1];
	a[0] = light_shape;
	//a[1] = glass_sphere;
	HitableList hlist(a, 1);
	for (int i = ny - 1; i >= 0; i--)
	{
		for (int j = 0; j < nx; j++)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++)
			{
				float u = float(j + (rand() / (RAND_MAX + 1.0))) / float(nx);
				float v = float(i + (rand() / (RAND_MAX + 1.0))) / float(ny);
				ray r = cam->getRay(u, v);
				
				vec3 p = r.pointAtParameter(2.0);
				col += de_nan(color(r, world, &hlist,0));
			}
			
			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			
			std::cout << ir << " " << ig << " " << ib << "\n";
		}
	}
}