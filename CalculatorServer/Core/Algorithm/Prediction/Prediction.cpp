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
        return result; // ��Ÿ� ��
    }
    // Ÿ���� �̵� ���� ���
    Vec3 targetDirection = (targetToPosition - targetPosition).normalized();
    Vec3 targetVelocity = targetDirection * targetSpeed;

    // ��� ��ġ ����
    Vec3 relativePos = targetPosition - myPosition;

    // ���������� ��� ���
    // |targetPos + targetVel*t - myPos| = missileSpeed * t
    float a = targetVelocity.dot_product(targetVelocity) - (missileSpeed * missileSpeed);
    float b = 2.0f * relativePos.dot_product(targetVelocity);
    float c = relativePos.dot_product(relativePos);

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return result; // ���� ����
    }

    // �� �� �� ����� ���� ���� �� ����
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
        return result; // ��ȿ�� �ð� ����
    }

    // ���� ��ġ ���
    result.predictedPosition = targetPosition + targetVelocity * t;

    // ���� ��Ÿ� üũ
    float finalDistance = myPosition.distance(result.predictedPosition);
    if (finalDistance > missileRange) {
        return result;
    }

    // ����
    result.canHit = true;


    return result;

}
