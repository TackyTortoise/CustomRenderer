#include "MaterialManager.h"
#include <iostream>

std::vector<TextureInfo> MaterialManager::m_TextureLibrary;

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
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
	for (auto i = 0; i < m_TextureLibrary.size(); ++i)
	{
		if (m_TextureLibrary[i].texture == tex)
		{
			--m_TextureLibrary[i].refCount;
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
	return false;
}

void MaterialManager::DestroyRemainingTextures()
{
	//Clear textures from memory
	for (auto& t : m_TextureLibrary)
	{
		delete t.texture;
		t.texture = nullptr;
	}
	m_TextureLibrary.clear();
}
