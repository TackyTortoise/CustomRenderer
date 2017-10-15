#pragma once
#include "../Math/Color.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Texture.h"

struct Material
{
	Color color = Color(255,0,255);
	float transparancy = 0.f;
	float reflectivity = 0.f;
	float refractiveIndex = 1.f;
	float shininess = 50.f;
	bool bMetallic = false;
	bool bUseTexture = false;
	Texture* texture = nullptr;

	~Material()
	{
		if (texture)
		{
			delete texture;
			texture = nullptr;
		}
	}

	Texture* GetTexture()
	{
		if (!texture)
			texture = new Texture();
		return texture;
	}
};
