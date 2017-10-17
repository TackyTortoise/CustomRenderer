#pragma once
#include "Math.h"

struct Color
{
	unsigned char b,g,r, a;
	Color(): r(0), g(0), b(0), a(0){}
	Color(char v): r(v), g(v), b(v), a(v){}
	Color(char rr, char gg, char bb, char aa = 5) : r(rr), g(gg), b(bb){}

	Color operator* (const float v) const { return{ (char)(r*v), (char)(g*v), (char)(b*v) }; }
	Color operator*= (const float v) { r *= v, g *= v, b *= v; return *this; }
	Color operator/ (const float v) const { return{ (char)(r / v), (char)(g / v), (char)(b / v) }; }

	Color operator+ (const float v) const { return{ (char)(r + v), (char)(g + v), (char)(b + v) }; }
	Color operator+ (const Color& c) const { return{ r + c.r, g + c.g, b + c.b }; }
	Color operator+= (const Color& c) { r += c.r, g += c.g, b += c.b; return *this; }
	Color operator* (const Color v) const { return{ r*v.r, g*v.g, b*v.b }; }
	Color& ClampAdd(Color c)
	{
		r = Math::Clamp((int)r + c.r, 0, 255);
		g = Math::Clamp((int)g + c.g, 0, 255);
		b = Math::Clamp((int)b + c.b, 0, 255);
		return *this;
	}

	Color& MultiplyNormalized(const Color& c)
	{
		r = (float)r * (float)c.r / 255.f;
		g = (float)g * (float)c.g / 255.f;
		b = (float)b * (float)c.b / 255.f;
		return *this;
	}
};
