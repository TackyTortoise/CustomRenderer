#include "Material.h"
#include "TextLoader.h"
#include <locale>

Material::Material() :texture(nullptr), normalMap(nullptr) {}

Material::~Material()
{
	if (texture)
	{
		MaterialManager::UnLoadTexture(texture);
		texture = nullptr;
	}
	if (normalMap)
	{
		MaterialManager::UnLoadTexture(normalMap);
		normalMap = nullptr;
	}
	if (reflectivityMap)
	{
		MaterialManager::UnLoadTexture(reflectivityMap);
		reflectivityMap = nullptr;
	}
	if (transparencyMap)
	{
		MaterialManager::UnLoadTexture(transparencyMap);
		transparencyMap = nullptr;
	}
}

void Material::SetTexture(const char* path)
{
	if (texture)
	{
		MaterialManager::UnLoadTexture(texture);
		texture = nullptr;
	}
	texture = MaterialManager::LoadTexture(path);
	bUseTexture = texture != nullptr;
}

void Material::SetNormalMap(const char* path)
{
	if (normalMap)
	{
		MaterialManager::UnLoadTexture(normalMap);
		normalMap = nullptr;
	}
	normalMap = MaterialManager::LoadTexture(path);
	bUseNormalMap = normalMap != nullptr;
}

void Material::SetReflectivityMap(const char* path)
{
	if (reflectivityMap)
	{
		MaterialManager::UnLoadTexture(reflectivityMap);
		reflectivityMap = nullptr;
	}
	reflectivityMap = MaterialManager::LoadTexture(path);
	bUseReflectivityMap = reflectivityMap != nullptr;
}

void Material::SetTransparencyMap(const char* path)
{
	if (transparencyMap)
	{
		MaterialManager::UnLoadTexture(transparencyMap);
		transparencyMap = nullptr;
	}
	transparencyMap = MaterialManager::LoadTexture(path);
	bUseTransparencyMap = transparencyMap != nullptr;
}
