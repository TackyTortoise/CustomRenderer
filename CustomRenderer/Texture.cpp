#include "Texture.h"
#include "Math/Color.h"
#include <SDL_image.h>

Texture::Texture()
{
}

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

Color Texture::GetPixelColor(float u, float v)
{
	if (!m_Surface)
	{
		std::cout << "Trying to acces pixels from uninitialized texture" << std::endl;
		return Color(255, 0, 255);
	}

	while (u < 0.f)
		u += 1.f;
	while (u > 1.f)
		u -= 1.f;
	while (v < 0.f)
		v += 1.f;
	while (v > 1.f)
		v -= 1.f;

	u = u * m_Surface->w;
	v = v * m_Surface->h;
	SDL_LockSurface(m_Surface);
	int bbp = m_Surface->format->BytesPerPixel;
	int index = (int)u * bbp + (int)v * m_Surface->pitch;
	Uint8* p = (Uint8*)m_Surface->pixels + index;
	Uint32 pixelColor = *(Uint32*)p;
	Uint8 red, green, blue, alpha;
	SDL_GetRGBA(pixelColor, m_Surface->format, &red, &green, &blue, &alpha);
	SDL_UnlockSurface(m_Surface);

	return Color(red, green, blue, alpha);
}
