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
	Color operator- (const Color& c) const { return{ r - c.r, g - c.g, b - c.b }; }
	Color operator+= (const Color& c) { r += c.r, g += c.g, b += c.b; return *this; }
	Color operator-= (const Color& c) { r -= c.r, g -= c.g, b -= c.b; return *this; }
	Color operator* (const Color v) const { return{ r*v.r, g*v.g, b*v.b }; }
	Color operator*= (const Color v) { r *= v.r, g *= v.g, b *= v.b; return *this; }

	bool operator== (const Color& c) const { return r == c.r && g == c.g && b == c.b; }

	Color& ClampAdd(const Color& c)
	{
		r = Math::Clamp((int)r + c.r, 0, 255);
		g = Math::Clamp((int)g + c.g, 0, 255);
		b = Math::Clamp((int)b + c.b, 0, 255);
		return *this;
	}

	Color& ClampMultiply(const Color& c)
	{
		r = Math::Clamp((int)r * c.r, 0, 255);
		g = Math::Clamp((int)g * c.g, 0, 255);
		b = Math::Clamp((int)b * c.b, 0, 255);
		return *this;
	}

	Color& ClampMultiply(const float f)
	{
		r = Math::Clamp((int)r * f, 0, 255);
		g = Math::Clamp((int)g * f, 0, 255);
		b = Math::Clamp((int)b * f, 0, 255);
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

struct FloatColor
{
	float b, g, r, a;
	FloatColor() : r(0), g(0), b(0), a(0) {}
	FloatColor(float v) : r(v), g(v), b(v), a(v) {}
	FloatColor(float rr, float gg, float bb, float aa = 5) : r(rr), g(gg), b(bb) {}

	FloatColor operator* (const float v) const { return{ (r*v), (g*v), (b*v) }; }
	FloatColor operator*= (const float v) { r *= v, g *= v, b *= v; return *this; }
	FloatColor operator/ (const float v) const { return{ (r / v), (g / v), (b / v) }; }

	FloatColor operator+ (const float v) const { return{ (r + v), (g + v), (b + v) }; }
	FloatColor operator+ (const FloatColor& c) const { return{ r + c.r, g + c.g, b + c.b }; }
	FloatColor operator- (const FloatColor& c) const { return{ r - c.r, g - c.g, b - c.b }; }
	FloatColor operator- (const Color& c) const { return{ r - c.r, g - c.g, b - c.b }; }
	FloatColor operator+= (const FloatColor& c) { r += c.r, g += c.g, b += c.b; return *this; }
	FloatColor operator+= (const Color& c) { r += c.r, g += c.g, b += c.b; return *this; }
	FloatColor operator-= (const FloatColor& c) { r -= c.r, g -= c.g, b -= c.b; return *this; }
	FloatColor operator-= (const Color& c) { r -= c.r, g -= c.g, b -= c.b; return *this; }
	FloatColor operator* (const FloatColor v) const { return{ r*v.r, g*v.g, b*v.b }; }
	FloatColor operator*= (const FloatColor v) { r *= v.r, g *= v.g, b *= v.b; return *this; }
	FloatColor operator*= (const Color v) { r *= v.r, g *= v.g, b *= v.b; return *this; }
	FloatColor operator/= (const Color v) { r /= v.r, g /= v.g, b /= v.b; return *this; }

	bool operator== (const Color& c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }

	Color ToCharColor() const
	{
		return Color(Math::Clamp(r, 0, 255), Math::Clamp(g, 0, 255), Math::Clamp(b, 0, 255));
	}
};
