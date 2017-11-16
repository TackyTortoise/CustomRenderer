#pragma once
class Object;
#include "Math.h"

struct HitInfo
{
	Vec3 position;
	Vec3 normal;
	float distance = 0.f;
	Vec2 uvCoordinate;
	Object* hitObject = nullptr;
};