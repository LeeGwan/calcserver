#pragma once
class Vec3;
class Object;
struct PredictionResult;

class Prediction
{

  
public:

   static const PredictionResult PredictMissileTarget(
        const Vec3& myPosition,       // 나의 위치
        Object* target, // 상대방
        float missileSpeed,           // 미사일 속도
        float missileRange            // 미사일 사거리
    );
};

