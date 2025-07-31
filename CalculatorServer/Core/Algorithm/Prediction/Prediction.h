#pragma once
class Vec3;
class Object;
struct PredictionResult;

class Prediction
{

  
public:

   static const PredictionResult PredictMissileTarget(
        const Vec3& myPosition,       // ���� ��ġ
        Object* target, // ����
        float missileSpeed,           // �̻��� �ӵ�
        float missileRange            // �̻��� ��Ÿ�
    );
};

