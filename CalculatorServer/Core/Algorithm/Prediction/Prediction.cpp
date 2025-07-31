#include "Prediction.h"
#include "../../utils/Vector.h"
#include "PredictionResult.h"
#include "../../Object/Object/ObjectComponent/Object.h"
#include "../../Object/Object/objectStats/objectStats.h"
#include "../../Object/Object/Navgation/Navigation.h"
#include "../../Object/Object/Transform/Transform.h"
#include<cmath>
const PredictionResult Prediction::PredictMissileTarget(const Vec3& myPosition, Object* target ,float missileSpeed, float missileRange)
{
    PredictionResult result;
    Navigation* targetnavi = target->Get_Navigation_Component();
    Vec3 targetPosition = target->Get_Transform_Component().Get_Position();
    if (targetnavi->Get_NaviPath().empty())
    {
        result.predictedPosition = targetPosition;
        result.canHit = true;
        return result;
    }
    std::vector<Vec3> navipath = targetnavi->Get_NaviPath();

    Vec3 targetToPosition= navipath[targetnavi->Get_Current_NaviPath_Index()];
    float targetSpeed= target->Get_ObjectStats_Component().Get_total_MovementSpeed();
  
    float currentDistance = myPosition.distance(targetPosition);
    if (currentDistance > missileRange) {
        return result; // 사거리 밖
    }
    // 타겟의 이동 벡터 계산
    Vec3 targetDirection = (targetToPosition - targetPosition).normalized();
    Vec3 targetVelocity = targetDirection * targetSpeed;

    // 상대 위치 벡터
    Vec3 relativePos = targetPosition - myPosition;

    // 이차방정식 계수 계산
    // |targetPos + targetVel*t - myPos| = missileSpeed * t
    float a = targetVelocity.dot_product(targetVelocity) - (missileSpeed * missileSpeed);
    float b = 2.0f * relativePos.dot_product(targetVelocity);
    float c = relativePos.dot_product(relativePos);

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return result; // 교점 없음
    }

    // 두 해 중 양수인 가장 작은 값 선택
    float sqrtD = std::sqrt(discriminant);
    float t1 = (-b - sqrtD) / (2 * a);
    float t2 = (-b + sqrtD) / (2 * a);

    float t = -1;
    if (t1 > 0) {
        t = t1;
    }
    else if (t2 > 0) {
        t = t2;
    }

    if (t <= 0) {
        return result; // 유효한 시간 없음
    }

    // 예측 위치 계산
    result.predictedPosition = targetPosition + targetVelocity * t;

    // 최종 사거리 체크
    float finalDistance = myPosition.distance(result.predictedPosition);
    if (finalDistance > missileRange) {
        return result;
    }

    // 성공
    result.canHit = true;


    return result;

}
