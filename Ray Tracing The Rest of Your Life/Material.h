#pragma once
#ifndef MATERIALH
#define MATERIALH

#include "ray.h"
#include "Hitable.h"
#include "Texture.h"
#include "pdf.h"
#include <iostream>

struct hit_record;

struct scatter_record
{
	ray specular_ray;
	bool is_specular;
	vec3 attenuation;
	PDF* pdf_ptr;
};



float schlick(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}


bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant > 0)
	{
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else
	{
		return false;
	}
}

vec3 reflect(const vec3& v, const vec3& n)
{
	return v - 2 * dot(v, n) * n;
}

inline vec3 random_cosine_direction()
{
	float r1 = (rand() / (RAND_MAX + 1.0));
	float r2 = (rand() / (RAND_MAX + 1.0));
	float z = sqrt(1 - r2);
	float phi = 2 * pi * r1;
	float x = cos(phi) * 2 * sqrt(r2);
	float y = sin(phi) * 2 * sqrt(r2);
	return vec3(x, y, z);
}

class Material
{
	public:
		virtual bool Scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec) const
		{
			return false;
		}
		virtual float scatteringPdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
		{
			return false;
		}
		virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const
		{
			return vec3(0, 0, 0);
		}
};

class Lambertian : public Material
{
public:

	Texture* albedo;

	Lambertian(Texture* a) : albedo(a)
	{

	}

	float scatteringPdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
	{
		float cosine = dot(rec.normal, unit_vector(scattered.direction()));
		float l_pi = 3.14159265358979323846;
		if (cosine < 0)
		{
			cosine = 0;
		}

		return cosine / l_pi;
	}

	bool Scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec) const
	{
		srec.is_specular = false;
		srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.p);
		srec.pdf_ptr = new cosinePDF(hrec.normal);
		return true;

	}

};


class Isotropic : public Material
{
public:
	Texture* albedo;
	Isotropic(Texture* a) : albedo(a) {}
	virtual bool Scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
	{
		scattered = ray(rec.p, randomInUnitSphere());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}

};



class diffuseLight : public Material
{
public:
	Texture* emit;
	diffuseLight(Texture* a) : emit(a) {}
	//virtual bool Scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const { return false; }
	virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const
	{
		if (dot(rec.normal, r_in.direction()) < 0.0)
		{
			return emit->value(u, v, p);
		}
		else
		{
			return vec3(0, 0, 0);
		}
	}
};




class Metal : public Material
{
	public:

		vec3 albedo;
		float fuzz;

		Metal(const vec3& a, float f) : albedo(a)
		{
			if (f < 1)
			{
				fuzz = f;
			}
			else
			{
				fuzz = 1;
			}
		}

		virtual bool Scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec) const
		{
			vec3 reflected = reflect(unit_vector(r_in.direction()), hrec.normal);
			srec.specular_ray = ray(hrec.p, reflected + fuzz * randomInUnitSphere());
			srec.attenuation = albedo;
			srec.is_specular = true;
			srec.pdf_ptr = 0;
			return true;
		}
};

class dielectric : public Material
{
	public:
		float ref_idx;
		dielectric(float ri) : ref_idx(ri) {}
		virtual bool Scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec) const {
			srec.is_specular = true;
			srec.pdf_ptr = 0;
			srec.attenuation = vec3(1.0, 1.0, 1.0);
			vec3 outward_normal;
			vec3 reflected = reflect(r_in.direction(), hrec.normal);
			vec3 refracted;
			float ni_over_nt;
			float reflect_prob;
			float cosine;
			if (dot(r_in.direction(), hrec.normal) > 0) {
				outward_normal = -hrec.normal;
				ni_over_nt = ref_idx;
				cosine = ref_idx * dot(r_in.direction(), hrec.normal) / r_in.direction().length();
			}
			else {
				outward_normal = hrec.normal;
				ni_over_nt = 1.0 / ref_idx;
				cosine = -dot(r_in.direction(), hrec.normal) / r_in.direction().length();
			}
			if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
				reflect_prob = schlick(cosine, ref_idx);
			}
			else {
				reflect_prob = 1.0;
			}
			if ((rand() / (RAND_MAX + 1.0)) < reflect_prob) {
				srec.specular_ray = ray(hrec.p, reflected);
			}
			else {
				srec.specular_ray = ray(hrec.p, refracted);
			}
			return true;
		}

};

#endif 

