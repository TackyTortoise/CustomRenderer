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
	bool bUseReflectivityMap = false;
	Texture* reflectivityMap = nullptr;
	bool bUseTransparencyMap = false;
	Texture* transparencyMap = nullptr;
	float m_TextureScale = 1.f;

	Material():texture(nullptr), normalMap(nullptr)
	{}

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

	void SetTexture(const char* path)
	{
		texture = MaterialManager::LoadTexture(path);
		bUseTexture = texture != nullptr;
	}

	void SetNormalMap(const char* path)
	{
		normalMap = MaterialManager::LoadTexture(path);
		bUseNormalMap = normalMap != nullptr;
	}

	void SetReflectivityMap(const char* path)
	{
		reflectivityMap = MaterialManager::LoadTexture(path);
		bUseReflectivityMap = reflectivityMap != nullptr;
	}

	void SetTransparencyMap(const char* path)
	{
		transparencyMap = MaterialManager::LoadTexture(path);
		bUseTransparencyMap = transparencyMap != nullptr;
	}

	void SetTexScale(float s)
	{
		m_TextureScale = s;
	}

	float GetScale() const
	{
		return m_TextureScale;
	}

	Texture* GetTexture() const
	{
		if (!texture)
			std::cout << "Requesting non existant texture" << std::endl;
		return texture;
	}
};
