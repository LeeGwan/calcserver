#pragma once
#include "../../../utils/Vector.h"
struct Circle
{
    Vec3 center;    ///< ���� �߽���
    float radius;   ///< ���� ������

    /**
     * @brief �⺻ ������
     */
    Circle() : center(Vec3(0, 0, 0)), radius(0.0f) {}

    /**
     * @brief �Ű����� ������
     * @param centerPos �߽���
     * @param r ������
     */
    Circle(const Vec3& centerPos, float r) : center(centerPos), radius(r) {}

    /**
     * @brief ���� �� ���ο� �ִ��� Ȯ��
     * @param point Ȯ���� ��
     * @return true�� ����, false�� �ܺ�
     */
    bool Contains(const Vec3& point) const
    {
        return center.distance(point) <= radius;
    }

    /**
     * @brief ���� ���� ���
     * @return ���� ����
     */
    float GetArea() const
    {
        return 3.14159f * radius * radius;
    }
};