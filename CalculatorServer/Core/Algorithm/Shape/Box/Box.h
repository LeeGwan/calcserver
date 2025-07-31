#pragma once
#include "../../../utils/Vector.h"
#include <algorithm>
struct Box
{
    /**
       * @brief �⺻ ������
       */
    Box() = default;

    /**
     * @brief �Ű����� ������
     * @param start ���� ��ġ
     * @param end �� ��ġ
     */
    Box(const Vec3& start, const Vec3& end) : startPos(start), endPos(end) {}

    Vec3 startPos;  ///< �ڽ��� ���� �𼭸� ��ġ
    Vec3 endPos;    ///< �ڽ��� �ݴ��� �𼭸� ��ġ

    /**
     * @brief ����ȭ�� AABB ��� (�ּ�/�ִ� ��ǥ ����)
     * @param minPos ���: �ּ� ��ǥ (left-bottom-back)
     * @param maxPos ���: �ִ� ��ǥ (right-top-front)
     *
     * startPos�� endPos�� �� �ະ �ּ�/�ִ밪�� ����Ͽ�
     * �ùٸ� AABB�� �����մϴ�. Y���� 0���� �����մϴ�.
     */
    void GetNormalizedAABB(Vec3& minPos, Vec3& maxPos) const
    {
        // X, Z���� �ּ�/�ִ밪 ���
        minPos.x = (std::min)(startPos.x, endPos.x);
        minPos.y = 0.0f;  // Y���� ���� �������� ����
        minPos.z = (std::min)(startPos.z, endPos.z);

        maxPos.x = (std::max)(startPos.x, endPos.x);
        maxPos.y = 0.0f;  // Y���� ���� �������� ����  
        maxPos.z = (std::max)(startPos.z, endPos.z);
    }

    /**
     * @brief �ڽ� Ȯ��
     * @param expansion �� �ະ Ȯ�差
     * @return Ȯ��� ���ο� �ڽ�
     */
    Box Expand(const Vec3& expansion) const
    {
        Vec3 minPos, maxPos;
        GetNormalizedAABB(minPos, maxPos);

        minPos -= expansion;
        maxPos += expansion;

        return Box(minPos, maxPos);
    }

    /**
     * @brief �ڽ��� �߽��� ���
     * @return �߽��� ��ǥ
     */
    Vec3 GetCenter() const
    {
        return (startPos + endPos) * 0.5f;
    }

    /**
     * @brief �ڽ��� ũ�� ���
     * @return �� �ະ ũ��
     */
    Vec3 GetSize() const
    {
        Vec3 minPos, maxPos;
        GetNormalizedAABB(minPos, maxPos);
        return maxPos - minPos;
    }

};