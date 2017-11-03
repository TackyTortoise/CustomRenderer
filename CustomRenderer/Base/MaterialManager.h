#pragma once
#include <vector>
#include "Texture.h"
class Material;

struct TextureInfo
{
	int nameHash = 0;
	Texture* texture = nullptr;
	unsigned refCount = 0;
};

struct MaterialInfo
{
	int nameHash = 0;
	Material* material = nullptr;
	unsigned refCount = 0;
};

class MaterialManager
{
public:
	MaterialManager();
	~MaterialManager();

	static void Destroy();

	static Texture* LoadTexture(const char* filePath);
	static Material* LoadMaterial(const std::string& materialName);
	static bool UnLoadTexture(const char* filePath);
	static bool UnLoadTexture(Texture* tex);
	static bool UnLoadMaterial(Material* mat);

private:
	static void DestroyRemainingTextures();
	static void DestroyRemainingMaterials();

	static std::string m_MaterialLibraryText;
	static std::vector<TextureInfo> m_TextureLibrary;
	static std::vector<MaterialInfo> m_MaterialLibrary;
};

