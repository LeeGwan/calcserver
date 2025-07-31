#include "VectorUtils.h"
#include <cmath>


float VectorUtils::GetLerpForAngle(const Vec3& from, const Vec3& to, const float& DeltaTime)
{
	float outangle;
	float LerpSpeed = 10.0f;
	if (LastTo.distance(to) > 0.001f)//to �ٲ�
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
    // ������ 0~360 ������ ����ȭ
    a = std::fmodf(a, 360.0f);
    b = std::fmodf(b, 360.0f);
    if (a < 0) a += 360.0f;
    if (b < 0) b += 360.0f;

    // �ִ� ��� ���
    float delta = b - a;

    // 180������ ũ�� �ݴ� ������ �� ª��
    if (delta > 180.0f) {
        delta -= 360.0f;
    }
    else if (delta < -180.0f) {
        delta += 360.0f;
    }

    float result = a + delta * t;

    // ����� 0~360 ������ ����ȭ
    result = std::fmodf(result, 360.0f);
    if (result < 0) result += 360.0f;

    return result;
}

