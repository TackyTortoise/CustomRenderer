#include "MaterialManager.h"
#include <iostream>
#include "TextLoader.h"
#include "Material.h"

std::string MaterialManager::m_MaterialLibraryText;
std::vector<TextureInfo> MaterialManager::m_TextureLibrary;
std::vector<MaterialInfo> MaterialManager::m_MaterialLibrary;

MaterialManager::MaterialManager() {}

MaterialManager::~MaterialManager() {}

void MaterialManager::Destroy()
{
	DestroyRemainingTextures();
	DestroyRemainingMaterials();
}

Texture* MaterialManager::LoadTexture(const char* filePath)
{
	auto hash = std::hash<std::string>{}(filePath);
	//check if texture already exists
	for (auto i = 0; i < m_TextureLibrary.size(); ++i)
	{
		if (m_TextureLibrary[i].nameHash == hash)
		{
			++m_TextureLibrary[i].refCount;
			return m_TextureLibrary[i].texture;
		}
	}

	//create new one if it does not
	TextureInfo n;
	n.nameHash = hash;
	n.refCount = 1;
	n.texture = new Texture();
	n.texture->LoadFromFile(filePath);
	m_TextureLibrary.push_back(n);
	return n.texture;
}

Material* MaterialManager::LoadMaterial(const std::string& materialName)
{
	if (m_MaterialLibraryText.size() == 0)
		m_MaterialLibraryText = TextLoader::TxtFileToString("../Data/MaterialLibrary.txt");

	std::string nameLower = materialName;
	std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), tolower);

	auto hash = std::hash<std::string>{}(nameLower);
	for (int i = 0; i < m_MaterialLibrary.size(); ++i)
	{
		if (m_MaterialLibrary[i].nameHash == hash)
		{
			++m_MaterialLibrary[i].refCount;
			return m_MaterialLibrary[i].material;
		}
	}

	//Load material properties
	Material* result = new Material();
	MaterialInfo n;
	n.nameHash = hash;
	n.material = result;
	n.refCount = 1;
	m_MaterialLibrary.push_back(n);

	std::string matStart = "<" + nameLower + ">";
	std::string matEnd = "</" + nameLower + ">";
	auto startInd = m_MaterialLibraryText.find(matStart);
	auto endInd = m_MaterialLibraryText.find(matEnd);

	if (startInd == std::string::npos || endInd == std::string::npos)
	{
		std::cout << "Failed to load material " << materialName << " from material library, using basic material instead" << std::endl;
		return result;
	}

	//Get part that contains material
	std::string matString = m_MaterialLibraryText.substr(startInd + matStart.size(), endInd - startInd - matStart.size());

	//Load color
	auto c = TextLoader::FindColorValueInString(matString, "color");
	result->color = c.r < 0 ? result->color : c.ToCharColor();

	//Load transparency
	auto f = TextLoader::FindFloatValueInString(matString, "transparency");
	result->transparency = f < 0 ? result->transparency : f;

	//Load reflectivity
	f = TextLoader::FindFloatValueInString(matString, "reflectivity");
	result->reflectivity = f < 0 ? result->reflectivity : f;

	//Load roughness
	f = TextLoader::FindFloatValueInString(matString, "roughness");
	result->roughness = f < 0 ? result->roughness : f;

	//Load refraction
	f = TextLoader::FindFloatValueInString(matString, "refractiveindex");
	result->refractiveIndex = f < 0 ? result->refractiveIndex : f;

	//Load shininess
	f = TextLoader::FindFloatValueInString(matString, "shininess");
	result->shininess = f < 0 ? result->shininess : f;

	//Load IsMetallic
	int m = TextLoader::FindIntValueInString(matString, "ismetallic");
	result->bMetallic = m < 0 ? result->bMetallic : m != 0;

	//Load Texture
	std::string tex = TextLoader::FindStringValueInString(matString, "texture");
	if (tex.size() > 0)
	{
		auto path = "../Data/Textures/" + tex;
		result->SetTexture(path.c_str());
	}

	//Load Normal map
	tex = TextLoader::FindStringValueInString(matString, "normalmap");
	if (tex.size() > 0)
	{
		auto path = "../Data/Textures/" + tex;
		result->SetNormalMap(path.c_str());
	}

	//Load Reflectivity Map
	tex = TextLoader::FindStringValueInString(matString, "reflectivitymap");
	if (tex.size() > 0)
	{
		auto path = "../Data/Textures/" + tex;
		result->SetReflectivityMap(path.c_str());
	}

	tex = TextLoader::FindStringValueInString(matString, "transparencymap");
	if (tex.size() > 0)
	{
		auto path = "../Data/Textures/" + tex;
		result->SetTransparencyMap(path.c_str());
	}

	//Load Textures scale
	f = TextLoader::FindFloatValueInString(matString, "texturescale");
	result->m_TextureScale = f < 0 ? result->m_TextureScale : f;

	int srgb = TextLoader::FindIntValueInString(matString, "srgbtexture");
	if (srgb > 0)
		result->texture->SetIsSRGB(srgb != 0);

	return result;
}

bool MaterialManager::UnLoadTexture(const char* filePath)
{
	auto hash = std::hash<std::string>{}(filePath);
	for (auto i = 0; i < m_TextureLibrary.size(); ++i)
	{
		if (m_TextureLibrary[i].nameHash == hash)
		{
			--m_TextureLibrary[i].refCount;
			if (m_TextureLibrary[i].refCount <= 0)
			{
				delete m_TextureLibrary[i].texture;
				m_TextureLibrary[i].texture = nullptr;
				m_TextureLibrary.erase(m_TextureLibrary.begin() + i);
				return true;
			}
		}
	}

	std::cout << "Failed to unload texture " << filePath << std::endl;
	return false;
}

bool MaterialManager::UnLoadTexture(Texture* tex)
{
	//Look for texture in library
	for (auto i = 0; i < m_TextureLibrary.size(); ++i)
	{
		if (m_TextureLibrary[i].texture == tex)
		{
			//Decrease reference count
			--m_TextureLibrary[i].refCount;
			//Destory texture if no references left
			if (m_TextureLibrary[i].refCount <= 0)
			{
				delete m_TextureLibrary[i].texture;
				m_TextureLibrary[i].texture = nullptr;
				m_TextureLibrary.erase(m_TextureLibrary.begin() + i);
			}
			return true;
		}
	}

	std::cout << "Failed to unload texture by pointer" << std::endl;
	SDL_Delay(200.f); //Delay so you can read console
	return false;
}

bool MaterialManager::UnLoadMaterial(Material* mat)
{
	//Look for material in library
	for (auto i = 0; i < m_MaterialLibrary.size(); ++i)
	{
		if (m_MaterialLibrary[i].material == mat)
		{
			//Decrease reference count
			--m_MaterialLibrary[i].refCount;
			//Destory texture if no references left
			if (m_MaterialLibrary[i].refCount <= 0)
			{
				delete m_MaterialLibrary[i].material;
				m_MaterialLibrary[i].material = nullptr;
				m_MaterialLibrary.erase(m_MaterialLibrary.begin() + i);
			}
			return true;
		}
	}

	delete mat;
	return false;
}


void MaterialManager::DestroyRemainingTextures()
{
	//Clear remaining textures from memory for safety
	for (auto& t : m_TextureLibrary)
	{
		delete t.texture;
		t.texture = nullptr;
	}
	m_TextureLibrary.clear();
}

void MaterialManager::DestroyRemainingMaterials()
{
	//Clear materials from memory
	for (auto& m : m_MaterialLibrary)
	{
		delete m.material;
		m.material = nullptr;
	}
	m_MaterialLibrary.clear();
}
