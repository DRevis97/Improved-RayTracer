#pragma once
#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

vec3 randomInUnitDisk()
{
	vec3 p;
	do {
		p = 2.0 * vec3((rand() / (RAND_MAX + 1.0)), (rand() / (RAND_MAX + 1.0)), 0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
	return p;
}

class Camera
{
	public:
		float pi = 3.14159265358979323846;
		vec3 origin;
		vec3 lower_left_corner;
		vec3 horizontal;
		vec3 vertical;
		vec3 u, v, w;
		float time0;
		float time1;

		float lens_radius;

		Camera(vec3 look_from, vec3 look_at, vec3 v_up, float vfov, float aspect, float aperture, float focus_distance, float t0, float t1)
		{
			time0 = t0;
			time1 = t1;
			lens_radius = aperture / 2;
			float theta = vfov * pi / 180;
			float half_height = tan(theta / 2);
			float half_width = aspect * half_height;
			origin = look_from;
			w = unit_vector(look_from - look_at);
			u = unit_vector(cross(v_up, w));
			v = cross(w, u);
			
			lower_left_corner = origin - half_width *focus_distance* u - half_height *focus_distance* v - focus_distance* w;
			horizontal = 2 * half_width * focus_distance * u;
			vertical = 2 * half_height * focus_distance * v;
			

		}

		ray getRay(float s, float t)
		{
			vec3 rd = lens_radius * randomInUnitDisk();
			vec3 offset = u * rd.x() + v * rd.y();
			float time = time0 + (rand() / (RAND_MAX + 1.0)) * (time1 - time0);
			return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, time);
		}


		
};

#endif
