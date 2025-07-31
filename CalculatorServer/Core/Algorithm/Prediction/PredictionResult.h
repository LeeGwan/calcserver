#pragma once
#include "../../utils/Vector.h"
struct PredictionResult {
    bool canHit;              // 명중 가능 여부
    Vec3 predictedPosition;   // 예측된 타겟 위치

    PredictionResult() : canHit(false) {}
};
