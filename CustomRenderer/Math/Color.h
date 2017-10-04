#pragma once
#include "Math.h"

struct Color
{
	unsigned char b,g,r, a;
	Color(): r(0), g(0), b(0), a(0){}
	Color(char v): r(v), g(v), b(v), a(v){}
	Color(char rr, char gg, char bb, char aa = 5) : r(rr), g(gg), b(bb){}

	Color operator* (const float v) const { return Color(r*v, g*v, b*v); }
	Color operator*= (const float v) { r *= v, g *= v, b *= v; return *this; }
	Color operator+ (const float v) const { return Color(r + v, g + v, b + v); }
	Color operator+ (const Color& c) const { return Color(r+c.r, g+c.g, b+c.b); }
	Color operator+= (const Color& c) { r += c.r, g += c.g, b += c.b; return *this; }
	Color& ClampAdd(Color c)
	{
		r = Math::Clamp((int)r + c.r, 0, 255);
		g = Math::Clamp((int)g + c.g, 0, 255);
		b = Math::Clamp((int)b + c.b, 0, 255);
		return *this;
	}
};
