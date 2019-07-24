#pragma once

#ifndef PDFH
#define PDFH
#include "onb.h"




inline vec3 randomCosineDirection()
{
	float r_pi = 3.14159265358979323846;
	float r1 = (rand() / (RAND_MAX + 1.0));
	float r2 = (rand() / (RAND_MAX + 1.0));
	float z = sqrt(1 - r2);
	float phi = 2 * r_pi * r1;
	float x = cos(phi) *sqrt(r2);
	float y = sin(phi) *sqrt(r2);
	return vec3(x, y, z);
}



vec3 randomInUnitSphere()
{
	vec3 p;
	do
	{
		p = 2.0 * vec3((rand() / (RAND_MAX + 1.0)), (rand() / (RAND_MAX + 1.0)), (rand() / (RAND_MAX + 1.0))) - vec3(1, 1, 1);
	} while (p.squared_length() >= 1.0);
	return p;
}


class PDF
{
public:
	virtual float value(const vec3& direction) const = 0;
	virtual vec3 generate() const = 0;
};

class cosinePDF : public PDF
{
public:
	onb uvw;
	float c_pi = 3.14159265358979323846;
	cosinePDF(const vec3& w)
	{
		uvw.buildFromW(w);
	}
	virtual float value(const vec3& direction) const
	{
		float cosine = dot(unit_vector(direction), uvw.w());
		if (cosine > 0)
		{
			return cosine / c_pi;
		}
		else
			return 0;
	}

	virtual vec3 generate()  const
	{
		return uvw.local(randomCosineDirection());
	}
};


class hitablePDF : public PDF
{
public:
	vec3 o;
	Hitable* ptr;

	hitablePDF(Hitable* p, const vec3& origin) : ptr(p), o(origin) {}
	virtual float value(const vec3& direction) const
	{
		return ptr->pdfValue(o, direction);

	}
	virtual vec3 generate() const
	{
		return ptr->random(o);
	}
};

class mixturePDF : public PDF
{
public:
	PDF* p[2];
	mixturePDF(PDF* p0, PDF* p1)
	{
		p[0] = p0;
		p[1] = p1;
	}
	virtual float value(const vec3& direction) const
	{
		return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
	}
	virtual vec3 generate() const
	{
		if ((rand() / (RAND_MAX + 1.0)) < 0.5)
		{
			return p[0]->generate();
		}
		else
		{
			return p[1]->generate();
		}
	}
};

#endif
