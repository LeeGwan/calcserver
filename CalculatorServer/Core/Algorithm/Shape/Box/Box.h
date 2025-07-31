#pragma once
#include "../../../utils/Vector.h"
#include <algorithm>
struct Box
{
    /**
       * @brief 기본 생성자
       */
    Box() = default;

    /**
     * @brief 매개변수 생성자
     * @param start 시작 위치
     * @param end 끝 위치
     */
    Box(const Vec3& start, const Vec3& end) : startPos(start), endPos(end) {}

    Vec3 startPos;  ///< 박스의 한쪽 모서리 위치
    Vec3 endPos;    ///< 박스의 반대쪽 모서리 위치

    /**
     * @brief 정규화된 AABB 얻기 (최소/최대 좌표 보정)
     * @param minPos 출력: 최소 좌표 (left-bottom-back)
     * @param maxPos 출력: 최대 좌표 (right-top-front)
     *
     * startPos와 endPos의 각 축별 최소/최대값을 계산하여
     * 올바른 AABB를 생성합니다. Y축은 0으로 고정합니다.
     */
    void GetNormalizedAABB(Vec3& minPos, Vec3& maxPos) const
    {
        // X, Z축의 최소/최대값 계산
        minPos.x = (std::min)(startPos.x, endPos.x);
        minPos.y = 0.0f;  // Y축은 지면 기준으로 고정
        minPos.z = (std::min)(startPos.z, endPos.z);

        maxPos.x = (std::max)(startPos.x, endPos.x);
        maxPos.y = 0.0f;  // Y축은 지면 기준으로 고정  
        maxPos.z = (std::max)(startPos.z, endPos.z);
    }

    /**
     * @brief 박스 확장
     * @param expansion 각 축별 확장량
     * @return 확장된 새로운 박스
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
     * @brief 박스의 중심점 계산
     * @return 중심점 좌표
     */
    Vec3 GetCenter() const
    {
        return (startPos + endPos) * 0.5f;
    }

    /**
     * @brief 박스의 크기 계산
     * @return 각 축별 크기
     */
    Vec3 GetSize() const
    {
        Vec3 minPos, maxPos;
        GetNormalizedAABB(minPos, maxPos);
        return maxPos - minPos;
    }

};