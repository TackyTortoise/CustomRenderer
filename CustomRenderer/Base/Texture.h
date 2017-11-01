#pragma once
#include <SDL.h>
class Color;

class Texture
{
public:
	Texture();
	~Texture();

	bool LoadFromFile(const char* path);
	Color GetPixelColor(float u, float v) const;

private:
	Uint32 GetPixel(int x, int y) const;

	SDL_Surface* m_Surface = nullptr;
};

