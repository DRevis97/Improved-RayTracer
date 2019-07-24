#pragma once
#ifndef SPHEREH
#define SPHEREH

#include "Hitable.h"
#include "aabb.h"
#include "onb.h"
#include "pdf.h"

inline vec3 randomToSphere(float radius, float distance_squared)
{
	float s_pi = 3.14159265358979323846;
	float r1 = (rand() / (RAND_MAX + 1.0));
	float r2 = (rand() / (RAND_MAX + 1.0));
	float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
	float phi = 2 * s_pi * r1;
	float x = cos(phi) * sqrt(1 - z * z);
	float y = sin(phi) * sqrt(1 - z * z);
	return vec3(x, y, z);
}



class Sphere : public Hitable
{
	public:
		Material* mat_ptr = NULL;
		vec3 center;
		float radius = 0;
		Sphere() {}
		Sphere(vec3 cen, float r, Material* m) : center(cen), radius(r), mat_ptr(m) {};
		virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const;
		float pdfValue(const vec3& o, const vec3& v) const;
		vec3 random(const vec3& o) const;


};

bool Sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0)
	{
		float temp = (-b - sqrt(b * b - a * c)) / a;
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			getSphereUV((rec.p - center) / radius, rec.u, rec.v);
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(b * b - a * c)) / a;
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			getSphereUV((rec.p - center) / radius, rec.u, rec.v);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

bool Sphere::bounding_box(float t0, float t1, aabb& box) const
{
	box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}

float Sphere::pdfValue(const vec3& o, const vec3& v) const
{
	hit_record rec;
	float p_pi = 3.14159265358979323846;
	if (this->hit(ray(o, v), 0.001, FLT_MAX, rec))
	{
		float cos_theta_max = sqrt(1 - radius * radius / (center - o).squared_length());
		float solid_angle = 2 * p_pi * (1 - cos_theta_max);
		return 1 / solid_angle;
	}
	else
	{
		return 0;
	}
}

vec3 Sphere::random(const vec3& o) const
{
	vec3 direction = center - o;
	float distance_squared = direction.squared_length();
	onb uvw;
	uvw.buildFromW(direction);
	return uvw.local(randomToSphere(radius, distance_squared));
}

#endif

