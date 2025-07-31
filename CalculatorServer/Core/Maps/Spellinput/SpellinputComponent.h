#pragma once

#include <memory>
#include <unordered_map>
#include <string>
// 전방 선언 - 불필요한 include 방지

namespace ErrorCode { enum Spell_ErrorType : uint8_t; };
class SkillDetected;
class Object;
class NavMesh;
class VMT_For_OBJ;
struct CastSpellINFOR;
class AbilityPool;
class SkillAbility;
class OutputComponent;
class VectorUtils;

/**
 * @brief 스펠 입력 및 처리를 담당하는 컴포넌트
 * 스펠 캐스팅 초기화, 업데이트, 상태 관리를 수행
 */
class SpellinputComponent final
{
public:
    SpellinputComponent();
    ~SpellinputComponent();

    // 복사/이동 방지 (필요시 제거)
    SpellinputComponent(const SpellinputComponent&) = delete;
    SpellinputComponent& operator=(const SpellinputComponent&) = delete;
    SpellinputComponent(SpellinputComponent&&) = delete;
    SpellinputComponent& operator=(SpellinputComponent&&) = delete;

    /**
     * @brief 스펠 캐스팅 초기화
     * @param Nav_Mesh 내비게이션 메쉬
     * @param m_SkillDetected 스킬 감지 객체 참조
     * @param in_Ability_Pool 어빌리티 풀
     * @param Out_Skill_Ability 출력될 스킬 어빌리티 참조
     * @param task 캐스팅 정보
     * @param OBJS 오브젝트 맵
     * @param Deltatime 델타타임
     * @return 에러 코드
     */
    ErrorCode::Spell_ErrorType InitializeSpellCasting(
        NavMesh* Nav_Mesh,
        SkillDetected*& m_SkillDetected,
        AbilityPool* in_Ability_Pool,
        SkillAbility*& Out_Skill_Ability,
        const CastSpellINFOR& task,
        const std::unordered_map<std::string, Object*>& OBJS,
        const float& Deltatime
    ); // const 제거

    /**
     * @brief 스펠 업데이트 처리
     * @param objects 오브젝트 맵
     * @param m_outputComponent 출력 컴포넌트
     * @param m_mapID 맵 ID
     * @param Nav_Mesh 내비게이션 메쉬
     * @param m_SkillDetected 스킬 감지 객체 참조
     * @param Skill 스킬 어빌리티 참조
     * @param Deltatime 델타타임
     * @return 에러 코드
     */
    ErrorCode::Spell_ErrorType UpdateSpellCasting(
        const std::unordered_map<std::string, Object*>& objects,
        OutputComponent* m_outputComponent,
        int m_mapID,
        NavMesh* Nav_Mesh,
        SkillDetected*& m_SkillDetected,
        SkillAbility*& Skill,
        const float& Deltatime,
        const float&Maptime
    ); // const 제거

    /**
     * @brief 쿨다운 업데이트
     * @param object 오브젝트
     * @param Deltatime 델타타임
     */
    void UpdateCooldown(Object* object, const float& Deltatime); // const 제거


};