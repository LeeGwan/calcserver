#pragma once
#include "../../utils/Vector.h"
struct PredictionResult {
    bool canHit;              // ���� ���� ����
    Vec3 predictedPosition;   // ������ Ÿ�� ��ġ

    PredictionResult() : canHit(false) {}
};
