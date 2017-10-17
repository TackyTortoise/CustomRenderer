#pragma once
#include "../Math/Color.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Texture.h"
#include "MaterialManager.h"

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
	bool bUseNormalMap = false;
	Texture* normalMap = nullptr;

	~Material()
	{
		if (texture)
		{
			MaterialManager::UnLoadTexture(texture);
		}

		if (normalMap)
		{
			MaterialManager::UnLoadTexture(normalMap);
		}
	}

	Texture* GetTexture() const
	{
		if (!texture)
			std::cout << "Requesting non existant texture" << std::endl;
		return texture;
	}
};
