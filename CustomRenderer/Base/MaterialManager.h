#pragma once
#include <vector>
#include "Texture.h"

struct TextureInfo
{
	int nameHash = 0;
	Texture* texture = nullptr;
	unsigned refCount = 0;
};

class MaterialManager
{
public:
	MaterialManager();
	~MaterialManager();

	static Texture* LoadTexture(const char* filePath);
	static bool UnLoadTexture(const char* filePath);
	static bool UnLoadTexture(Texture* tex);

private:
	static std::vector<TextureInfo> m_TextureLibrary;
};

