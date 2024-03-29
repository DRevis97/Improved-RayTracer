#pragma once
#ifndef TEXTUREH
#define TEXTUREH

#include "Perlin.h"


class Texture {
public:
	virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constantTexture : public Texture
{
public:
	vec3 color;
	constantTexture() {}
	constantTexture(vec3 c) : color(c) {}
	virtual vec3 value(float u, float v, const vec3& p) const
	{
		return color;
	}

};

class checkerTexture : public Texture
{
public:
	Texture* odd;
	Texture* even;

	checkerTexture() {}
	checkerTexture(Texture* t0, Texture* t1) : even(t0), odd(t1) {}
	virtual vec3 value(float u, float v, const vec3& p) const
	{
		float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
		if (sines < 0) return odd->value(u, v, p);
		else return even->value(u, v, p);
	}
};

class noiseTexture : public Texture
{
public:
	Perlin noise;
	float scale;

	noiseTexture() {}
	noiseTexture(float sc) : scale(sc) {}
	virtual vec3 value(float u, float v, const vec3& p) const
	{
		return vec3(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
	}
};


#endif
