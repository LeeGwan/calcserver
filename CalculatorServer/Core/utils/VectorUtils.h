#pragma once
#include "Vector.h"
class Vec3;
class VectorUtils final
{
public:

	float GetLerpForAngle(const Vec3& from, const Vec3& to,const float& DeltaTime);
	float LerpAngle(float a, float b, float t);
private:
	Vec3 LastTo = {0.0f,0.0f, 0.0f};
	float CurrentAngle = 0.0f;
	float LastAngle=0.0f;
};