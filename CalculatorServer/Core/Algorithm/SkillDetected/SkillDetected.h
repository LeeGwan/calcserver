#pragma once
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <float.h>
#include <string>
#include "../../Navmesh/recastnavigation/Include/DetourNavMesh.h"
enum class CollisionObject : unsigned long;
enum PolyFlags;
class Vec3;
class Object;
class dtNavMeshQuery;
struct SkillEffect
{
    unsigned int skillId;
    float remainingTime;
    unsigned short dangerousLevel;
};

struct PolyFlagInfo
{
    PolyFlagInfo();
    ~PolyFlagInfo();

    dtPolyRef polyRef;
    unsigned short originalFlags;
    std::unordered_map<unsigned int, SkillEffect> activeSkills;  // skillId�� Ű�� ���
    mutable std::shared_mutex m_PolyFlagInfoMutex;

    // Getter �Լ���
    dtPolyRef GetPolyRef() const;
    unsigned short GetOriginalFlags() const;
    size_t GetActiveSkillsCount() const;
    bool HasActiveSkills() const;

    // Setter �Լ���
    void SetPolyRef(dtPolyRef ref);
    void SetOriginalFlags(unsigned short flags);

    // ��ų �߰�/���� (O(1))
    void AddOrUpdateSkill(const SkillEffect& skill);

    // Ư�� ��ų ���� (O(1))
    bool RemoveSkill(unsigned int skillId);

    // ��� ��ų ����
    void ClearSkills();

    // �ð� ������Ʈ �� ����� ��ų ����
    bool UpdateSkillTimes(float deltaTime);

    // ��� ���赵 OR �������� ��ġ��
    unsigned short GetCombinedDangerLevel() const;
};

struct MovingSkillInfo
{
    MovingSkillInfo();
    ~MovingSkillInfo();

    unsigned int skillId;
    float startPos[3];
    float endPos[3];
    float radius;
    unsigned short dangerousLevel;
    float duration;
    std::unordered_set<dtPolyRef> currentPolys;  // ���� ����/������ ���� set ���
    mutable std::shared_mutex m_MovingSkillInfoMutex;
    unsigned long collisionobject;
    // Getter �Լ���
    unsigned int GetSkillId() const;
    void GetStartPos(float pos[3]) const;
    void GetEndPos(float pos[3]) const;
    float GetRadius() const;
    unsigned short GetDangerousLevel() const;
    float GetDuration() const;
    std::vector<dtPolyRef> GetCurrentPolys() const;
    size_t GetCurrentPolysCount() const;
    unsigned long GetcollisionObj()const;
    // Setter �Լ���
    void SetSkillId(unsigned int id);
    void SetStartPos(const float pos[3]);
    void SetEndPos(const float pos[3]);
    void SetRadius(float r);
    void SetDangerousLevel(unsigned short level);
    void SetDuration(float dur);
    void SetcollisionObj(const unsigned long& in_collisionobject);
    // ������ �߰� (O(1))
    bool AddCurrentPoly(dtPolyRef polyRef);

    // ������ ���� (O(1))
    bool RemoveCurrentPoly(dtPolyRef polyRef);

    // ��� ������ ����
    void ClearCurrentPolys();

    // ��ü ������ �� ���� ����
    void SetAllData(unsigned int id, const float start[3], const float end[3],
        float r, unsigned short level, float dur, unsigned long incollisionobject);
};

class SkillDetected
{
private:
    std::unordered_map<dtPolyRef, std::unique_ptr<PolyFlagInfo>> m_modifiedPolys;
    std::unordered_map<unsigned int, std::unique_ptr<MovingSkillInfo>> m_movingSkills;
    std::unordered_map<unsigned int, dtPolyRef> m_missilePositionPolys; // �̻��� ��ġ ������ ����
    mutable std::shared_mutex m_modifiedPolysmutex;
    mutable std::shared_mutex m_movingSkillsmutex;
    mutable std::shared_mutex m_missilePositionPolysmutex;
public:
    SkillDetected();
    ~SkillDetected() = default;
    // ��ų ���� �� ������鿡 ���� �÷��� ����
    unsigned int ApplySkillDangerousArea(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        const float* startPos, const float* endPos, float radius,
        unsigned short dangerousLevel, float duration);

    // �̵��ϴ� ��ų ���� (�̻��� ��)
    bool CreateMovingSkill(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        const float* startPos, const float* endPos, float radius,
        unsigned short dangerousLevel, float duration, unsigned int in_skillid, unsigned long incollisionobject);

    void FindPolysInCircularRange_NoFilter(dtNavMeshQuery* navQuery, const float* center, float radius, std::vector<dtPolyRef>& outPolys);

    float CheckMissileCollisionWithObject(dtNavMesh* navMesh, dtNavMeshQuery* navQuery, const float* objectPos, float radius);

    bool CheckMultipleObjectsMissileCollision(dtNavMesh* navMesh, dtNavMeshQuery* navQuery, const std::unordered_map<std::string, Object*>& objects, Object* caster, Object*& target, unsigned long collisionObj);

    // �̵��ϴ� ��ų�� ��ġ ������Ʈ
    bool UpdateMovingSkillPosition(Object* caster, Object*& target,const std::unordered_map<std::string, Object*>& objects,dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        unsigned int skillId, const float* newStartPos, float radius);

    // ������Ʈ�� ��ų ������ �浹�ϴ��� üũ
    bool CheckObjectCollision(const float* startPos, const float* endPos, float skillradius,
        const float* objectPos, float objectRadius);

    // ���� ������Ʈ����� �浹 üũ
    std::vector<int> CheckMultipleObjectsCollision(const float* startPos, const float* endPos, float skillradius,
        const std::vector<std::pair<float[3], float>>& objects);

    // Ư�� ��ų ID�� ȿ���� ��� ����
    void RemoveSkillEffect(dtNavMesh* navMesh, unsigned int skillId);

    // ��� ��ų ȿ���� ��� ����
    void RemoveAllSkillEffects(dtNavMesh* navMesh);

    // �� �����Ӹ��� ȣ���Ͽ� ����� �÷��׵��� ������� ����
    void Update(dtNavMesh* navMesh, dtNavMeshQuery* navQuery, float deltaTime);

    // �̻��� ���� ��ġ�� Missile_pos �÷��� ����
    void UpdateMissilePositionFlag(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        unsigned int skillId, const float* currentPos, float radius, PolyFlags flag);




private:
    // === �簢�� �̻��� ��ų ���� �˻� �Լ��� ===

    // �簢�� �̻��� ���� �� ��������� ã�� ���� �Լ�
    void FindPolysInSkillRange(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos, float radius,
        std::vector<dtPolyRef>& outPolys);

    // �簢���� 4�� ������ ���
    void CalculateRectangleCorners(const float* startPos, const float* endPos, float radius, Vec3* corners);

    // �簢���� �ٿ�� �ڽ� ���
    void CalculateRectangleBounds(const Vec3* corners, float* bounds);

    // NavMesh ������ �ĺ� ������� ����
    void QueryCandidatePolygons(dtNavMeshQuery* navQuery, const float* bounds, std::vector<dtPolyRef>& candidates);

    // �������� �簢�� �ȿ� �ִ��� üũ
    bool IsPolygonInRectangle(const dtNavMesh* navMesh, dtPolyRef polyRef, const Vec3* rectCorners);

    // ���� �簢�� �ȿ� �ִ��� Ȯ�� (2D ����)
    bool IsPointInRectangle(const Vec3& point, const Vec3* rectCorners);



    // === ���� ���� �˻� �Լ��� (�̻��� ��ġ��) ===

    // Ư�� ��ġ �ֺ��� ������ ã�� (���� ����)
    void FindPolysInCircularRange(dtNavMeshQuery* navQuery, const float* center, float radius,
        std::vector<dtPolyRef>& outPolys);


    // ���� ����� ������ ã��
    dtPolyRef FindClosestPolyToPoint(const dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        const float* point, const std::vector<dtPolyRef>& candidates);

    // === ���� ��ƿ �Լ��� ===

    // ��(������Ʈ)�� ĸ��(��ų ����)�� �浹 üũ
    bool IsCircleIntersectCapsule(const float* circleCenter, float circleRadius,
        const float* capsuleStart, const float* capsuleEnd, float capsuleradius);

    // Ư�� �����￡�� Ư�� ��ų�� ���� (�̵� ��ų��)
    void RemoveSkillFromPoly(dtNavMesh* navMesh, dtPolyRef polyRef, unsigned int skillId);

    // �����￡ ���� �÷��� ����
    bool ApplyDangerousFlagToPoly(dtNavMesh* navMesh, dtPolyRef polyRef, unsigned int skillId,
        unsigned short dangerousLevel, float duration);

    // �������� �÷��׸� ���� Ȱ�� ��ų�鿡 ���� ������Ʈ
    void UpdatePolyFlags(dtNavMesh* navMesh, dtPolyRef polyRef);

    // ���� �÷��׷� ����
    void RestoreOriginalFlag(dtNavMesh* navMesh, dtPolyRef polyRef);

    bool IsPolyInCircularRange(const dtNavMesh* navMesh, dtPolyRef polyRef, const float* center, float radius);

    Vec3 CalculatePolygonCenter(const dtPoly* poly, const float* verts);

    void CheckAllMissileFlags(dtNavMesh* navMesh);

    // �����￡ Missile_pos �÷��� ����/����
    bool ApplyMissilePositionFlag(dtNavMesh* navMesh, dtPolyRef polyRef, bool apply, PolyFlags flag);


    
};