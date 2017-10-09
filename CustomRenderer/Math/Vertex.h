#pragma once
#include "Vec3.h"

struct PosNormVertex
{
	PosNormVertex(Vec3 pos, Vec3 norm) : position(pos), normal(norm){}
	Vec3 position;
	Vec3 normal;
};

struct PosNormColVertex
{
	PosNormColVertex(Vec3 pos, Vec3 norm, Color col = Color(255)) : position(pos), normal(norm), color(col) {}
	Vec3 position;
	Vec3 normal;
	Color color;
};