#pragma once
#include "../Math/Color.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Texture.h"
#include "MaterialManager.h"

struct Material
{
	Color color = Color(255,0,255);
	float transparency = 0.f;
	float reflectivity = 0.f;
	float roughness = 0.f;
	float refractiveIndex = 1.f;
	float shininess = 50.f;
	bool bMetallic = false;
	bool bUseTexture = false;
	Texture* texture = nullptr;
	bool bUseNormalMap = false;
	Texture* normalMap = nullptr;
	bool bUseReflectivityMap = false;
	Texture* reflectivityMap = nullptr;
	bool bUseTransparencyMap = false;
	Texture* transparencyMap = nullptr;
	float m_TextureScale = 1.f;

	Material();

	~Material();	

	void SetTexture(const char* path);

	void SetNormalMap(const char* path);

	void SetReflectivityMap(const char* path);

	void SetTransparencyMap(const char* path);

	void SetTexScale(float s){ m_TextureScale = s; }

	void SetTexIsSRGB(bool v) const { texture->SetIsSRGB(v); }

	float GetScale() const	{ return m_TextureScale; }

	Texture* GetTexture() const
	{
		if (!texture)
			std::cout << "Requesting non existant texture" << std::endl;
		return texture;
	}
};
