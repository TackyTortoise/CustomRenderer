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

struct PosNormUVVertex
{
	PosNormUVVertex(Vec3 pos, Vec3 norm, Vec2 uvCoord) : position(pos), normal(norm), uv(uvCoord) {}
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
};

struct PosNormUVTangentBitangentVertex
{
	PosNormUVTangentBitangentVertex(Vec3 pos, Vec3 norm, Vec2 uvCoord, Vec3 tangent, Vec3 biTangent) : position(pos), normal(norm), uv(uvCoord) {}
	Vec3 position;
	Vec3 normal;
	Vec3 tangent;
	Vec3 biTangent;
	Vec2 uv;

};