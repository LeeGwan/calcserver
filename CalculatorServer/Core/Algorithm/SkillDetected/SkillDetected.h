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
    std::unordered_map<unsigned int, SkillEffect> activeSkills;  // skillId를 키로 사용
    mutable std::shared_mutex m_PolyFlagInfoMutex;

    // Getter 함수들
    dtPolyRef GetPolyRef() const;
    unsigned short GetOriginalFlags() const;
    size_t GetActiveSkillsCount() const;
    bool HasActiveSkills() const;

    // Setter 함수들
    void SetPolyRef(dtPolyRef ref);
    void SetOriginalFlags(unsigned short flags);

    // 스킬 추가/수정 (O(1))
    void AddOrUpdateSkill(const SkillEffect& skill);

    // 특정 스킬 제거 (O(1))
    bool RemoveSkill(unsigned int skillId);

    // 모든 스킬 제거
    void ClearSkills();

    // 시간 업데이트 및 만료된 스킬 제거
    bool UpdateSkillTimes(float deltaTime);

    // 모든 위험도 OR 연산으로 합치기
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
    std::unordered_set<dtPolyRef> currentPolys;  // 빠른 삽입/삭제를 위해 set 사용
    mutable std::shared_mutex m_MovingSkillInfoMutex;
    unsigned long collisionobject;
    // Getter 함수들
    unsigned int GetSkillId() const;
    void GetStartPos(float pos[3]) const;
    void GetEndPos(float pos[3]) const;
    float GetRadius() const;
    unsigned short GetDangerousLevel() const;
    float GetDuration() const;
    std::vector<dtPolyRef> GetCurrentPolys() const;
    size_t GetCurrentPolysCount() const;
    unsigned long GetcollisionObj()const;
    // Setter 함수들
    void SetSkillId(unsigned int id);
    void SetStartPos(const float pos[3]);
    void SetEndPos(const float pos[3]);
    void SetRadius(float r);
    void SetDangerousLevel(unsigned short level);
    void SetDuration(float dur);
    void SetcollisionObj(const unsigned long& in_collisionobject);
    // 폴리곤 추가 (O(1))
    bool AddCurrentPoly(dtPolyRef polyRef);

    // 폴리곤 제거 (O(1))
    bool RemoveCurrentPoly(dtPolyRef polyRef);

    // 모든 폴리곤 제거
    void ClearCurrentPolys();

    // 전체 데이터 한 번에 설정
    void SetAllData(unsigned int id, const float start[3], const float end[3],
        float r, unsigned short level, float dur, unsigned long incollisionobject);
};

class SkillDetected
{
private:
    std::unordered_map<dtPolyRef, std::unique_ptr<PolyFlagInfo>> m_modifiedPolys;
    std::unordered_map<unsigned int, std::unique_ptr<MovingSkillInfo>> m_movingSkills;
    std::unordered_map<unsigned int, dtPolyRef> m_missilePositionPolys; // 미사일 위치 폴리곤 추적
    mutable std::shared_mutex m_modifiedPolysmutex;
    mutable std::shared_mutex m_movingSkillsmutex;
    mutable std::shared_mutex m_missilePositionPolysmutex;
public:
    SkillDetected();
    ~SkillDetected() = default;
    // 스킬 범위 내 폴리곤들에 위험 플래그 적용
    unsigned int ApplySkillDangerousArea(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        const float* startPos, const float* endPos, float radius,
        unsigned short dangerousLevel, float duration);

    // 이동하는 스킬 생성 (미사일 등)
    bool CreateMovingSkill(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        const float* startPos, const float* endPos, float radius,
        unsigned short dangerousLevel, float duration, unsigned int in_skillid, unsigned long incollisionobject);

    void FindPolysInCircularRange_NoFilter(dtNavMeshQuery* navQuery, const float* center, float radius, std::vector<dtPolyRef>& outPolys);

    float CheckMissileCollisionWithObject(dtNavMesh* navMesh, dtNavMeshQuery* navQuery, const float* objectPos, float radius);

    bool CheckMultipleObjectsMissileCollision(dtNavMesh* navMesh, dtNavMeshQuery* navQuery, const std::unordered_map<std::string, Object*>& objects, Object* caster, Object*& target, unsigned long collisionObj);

    // 이동하는 스킬의 위치 업데이트
    bool UpdateMovingSkillPosition(Object* caster, Object*& target,const std::unordered_map<std::string, Object*>& objects,dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        unsigned int skillId, const float* newStartPos, float radius);

    // 오브젝트가 스킬 범위와 충돌하는지 체크
    bool CheckObjectCollision(const float* startPos, const float* endPos, float skillradius,
        const float* objectPos, float objectRadius);

    // 여러 오브젝트들과의 충돌 체크
    std::vector<int> CheckMultipleObjectsCollision(const float* startPos, const float* endPos, float skillradius,
        const std::vector<std::pair<float[3], float>>& objects);

    // 특정 스킬 ID의 효과를 즉시 제거
    void RemoveSkillEffect(dtNavMesh* navMesh, unsigned int skillId);

    // 모든 스킬 효과를 즉시 제거
    void RemoveAllSkillEffects(dtNavMesh* navMesh);

    // 매 프레임마다 호출하여 만료된 플래그들을 원래대로 복원
    void Update(dtNavMesh* navMesh, dtNavMeshQuery* navQuery, float deltaTime);

    // 미사일 현재 위치에 Missile_pos 플래그 적용
    void UpdateMissilePositionFlag(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        unsigned int skillId, const float* currentPos, float radius, PolyFlags flag);




private:
    // === 사각형 미사일 스킬 범위 검사 함수들 ===

    // 사각형 미사일 범위 내 폴리곤들을 찾는 메인 함수
    void FindPolysInSkillRange(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos, float radius,
        std::vector<dtPolyRef>& outPolys);

    // 사각형의 4개 꼭짓점 계산
    void CalculateRectangleCorners(const float* startPos, const float* endPos, float radius, Vec3* corners);

    // 사각형의 바운딩 박스 계산
    void CalculateRectangleBounds(const Vec3* corners, float* bounds);

    // NavMesh 쿼리로 후보 폴리곤들 수집
    void QueryCandidatePolygons(dtNavMeshQuery* navQuery, const float* bounds, std::vector<dtPolyRef>& candidates);

    // 폴리곤이 사각형 안에 있는지 체크
    bool IsPolygonInRectangle(const dtNavMesh* navMesh, dtPolyRef polyRef, const Vec3* rectCorners);

    // 점이 사각형 안에 있는지 확인 (2D 기준)
    bool IsPointInRectangle(const Vec3& point, const Vec3* rectCorners);



    // === 원형 범위 검사 함수들 (미사일 위치용) ===

    // 특정 위치 주변의 폴리곤 찾기 (원형 범위)
    void FindPolysInCircularRange(dtNavMeshQuery* navQuery, const float* center, float radius,
        std::vector<dtPolyRef>& outPolys);


    // 가장 가까운 폴리곤 찾기
    dtPolyRef FindClosestPolyToPoint(const dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        const float* point, const std::vector<dtPolyRef>& candidates);

    // === 기존 유틸 함수들 ===

    // 원(오브젝트)과 캡슐(스킬 범위)의 충돌 체크
    bool IsCircleIntersectCapsule(const float* circleCenter, float circleRadius,
        const float* capsuleStart, const float* capsuleEnd, float capsuleradius);

    // 특정 폴리곤에서 특정 스킬만 제거 (이동 스킬용)
    void RemoveSkillFromPoly(dtNavMesh* navMesh, dtPolyRef polyRef, unsigned int skillId);

    // 폴리곤에 위험 플래그 적용
    bool ApplyDangerousFlagToPoly(dtNavMesh* navMesh, dtPolyRef polyRef, unsigned int skillId,
        unsigned short dangerousLevel, float duration);

    // 폴리곤의 플래그를 현재 활성 스킬들에 따라 업데이트
    void UpdatePolyFlags(dtNavMesh* navMesh, dtPolyRef polyRef);

    // 원래 플래그로 복원
    void RestoreOriginalFlag(dtNavMesh* navMesh, dtPolyRef polyRef);

    bool IsPolyInCircularRange(const dtNavMesh* navMesh, dtPolyRef polyRef, const float* center, float radius);

    Vec3 CalculatePolygonCenter(const dtPoly* poly, const float* verts);

    void CheckAllMissileFlags(dtNavMesh* navMesh);

    // 폴리곤에 Missile_pos 플래그 적용/제거
    bool ApplyMissilePositionFlag(dtNavMesh* navMesh, dtPolyRef polyRef, bool apply, PolyFlags flag);


    
};