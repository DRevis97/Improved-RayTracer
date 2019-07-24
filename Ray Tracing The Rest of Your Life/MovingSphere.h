#pragma once
#ifndef MOVINGSPHEREH
#define MOVINGSPHEREH

#include "Hitable.h"


class MovingSphere : public Hitable {

public:

	vec3 center0;
	vec3 center1;
	float time0;
	float time1;
	float radius;
	Material* mat_ptr;

	MovingSphere()
	{

	}

	MovingSphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, Material* m) : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m)
	{

	}

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	vec3 center(float time) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

};

vec3 MovingSphere::center(float time) const
{
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool MovingSphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	vec3 oc = r.origin() - center(r.time());
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0)
	{
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.pointAtParameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

bool MovingSphere::bounding_box(float t0, float t1, aabb& box) const
{
	aabb box0(center(t0) - vec3(radius, radius, radius), center(t0) + vec3(radius, radius, radius));
	aabb box1(center(t1) - vec3(radius, radius, radius), center(t1) + vec3(radius, radius, radius));
	box = box.surrounding_box(box0, box1);
	return true;
}


#endif // !MOVINGSPHEREH
