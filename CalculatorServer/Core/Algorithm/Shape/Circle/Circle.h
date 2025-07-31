#pragma once
#include "../../../utils/Vector.h"
struct Circle
{
    Vec3 center;    ///< 원의 중심점
    float radius;   ///< 원의 반지름

    /**
     * @brief 기본 생성자
     */
    Circle() : center(Vec3(0, 0, 0)), radius(0.0f) {}

    /**
     * @brief 매개변수 생성자
     * @param centerPos 중심점
     * @param r 반지름
     */
    Circle(const Vec3& centerPos, float r) : center(centerPos), radius(r) {}

    /**
     * @brief 점이 원 내부에 있는지 확인
     * @param point 확인할 점
     * @return true면 내부, false면 외부
     */
    bool Contains(const Vec3& point) const
    {
        return center.distance(point) <= radius;
    }

    /**
     * @brief 원의 면적 계산
     * @return 원의 면적
     */
    float GetArea() const
    {
        return 3.14159f * radius * radius;
    }
};