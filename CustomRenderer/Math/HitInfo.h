#pragma once
class Object;

struct HitInfo
{
	Vec3 position;
	Vec3 normal;
	float distance;
	Vec2 uvCoordinate;
	Object* hitObject;
};