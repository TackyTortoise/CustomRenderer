#include "Texture.h"
#include <SDL_image.h>
#include <iostream>
#include "../Math/Color.h"

Texture::Texture(){}

Texture::~Texture()
{
	if (m_Surface)
	{
		SDL_FreeSurface(m_Surface);
		m_Surface = nullptr;
	}
}

bool Texture::LoadFromFile(const char* path)
{
	m_Surface = IMG_Load(path);
	if (!m_Surface)
		std::cout << "Failed to load texture " << path << std::endl;

	return m_Surface != nullptr;
}

Color Texture::GetPixelColor(float u, float v) const
{
	if (!m_Surface)
	{
		std::cout << "Trying to acces pixels from uninitialized texture" << std::endl;
		return Color(255, 0, 255);
	}

	//wrap around uv's to [0,1]
	u = (u < 0 ? 1 : 0) + std::fmod(u, 1.f);
	v = (v < 0 ? 1 : 0) + std::fmod(v, 1.f);


	//map from [0,1] to texture size
	u *= m_Surface->w;
	v *= m_Surface->h;

	//get pixel color
	SDL_LockSurface(m_Surface);
	Uint32 pixelColor = GetPixel(u, v);
	Uint8 red, green, blue, alpha;
	SDL_GetRGB(pixelColor, m_Surface->format, &red, &green, &blue);
	SDL_UnlockSurface(m_Surface);

	return Color(red, green, blue, 255);
}

Uint32 Texture::GetPixel(int x, int y) const
{
	int bpp = m_Surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)m_Surface->pixels + y * m_Surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		return 0;
	}
}
