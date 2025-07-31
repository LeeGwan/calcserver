#include "VectorUtils.h"
#include <cmath>


float VectorUtils::GetLerpForAngle(const Vec3& from, const Vec3& to, const float& DeltaTime)
{
	float outangle;
	float LerpSpeed = 10.0f;
	if (LastTo.distance(to) > 0.001f)//to 바뀜
	{
		LastTo = to;
		LastAngle = from.GetSignedAngleBetween(to);
	}
	outangle = LerpAngle(CurrentAngle, LastAngle, DeltaTime * LerpSpeed);
	CurrentAngle = outangle;
	return outangle;
}

float VectorUtils::LerpAngle(float a, float b, float t)
{
    // 각도를 0~360 범위로 정규화
    a = std::fmodf(a, 360.0f);
    b = std::fmodf(b, 360.0f);
    if (a < 0) a += 360.0f;
    if (b < 0) b += 360.0f;

    // 최단 경로 계산
    float delta = b - a;

    // 180도보다 크면 반대 방향이 더 짧음
    if (delta > 180.0f) {
        delta -= 360.0f;
    }
    else if (delta < -180.0f) {
        delta += 360.0f;
    }

    float result = a + delta * t;

    // 결과를 0~360 범위로 정규화
    result = std::fmodf(result, 360.0f);
    if (result < 0) result += 360.0f;

    return result;
}

