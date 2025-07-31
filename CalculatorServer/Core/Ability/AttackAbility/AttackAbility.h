#pragma once
#include <shared_mutex>
#include "../Ability.h"

// 전방 선언
enum class Slot : unsigned int;

/**
 * @brief 공격 능력을 관리하는 클래스
 *
 * 이 클래스는 기본 공격(Auto Attack)을 관리합니다.
 * 스킬과 달리 상대적으로 단순한 구조를 가지며,
 * 주로 사용 가능 여부만 관리합니다.
 */
class AttackAbility : public Ability
{
public:
    // ==========================================================================
    // Ability 인터페이스 구현
    // ==========================================================================

    /**
     * @brief 공격 사용 가능 여부 확인
     * @return true면 사용 가능, false면 사용 중
     */
    bool can_use_Ability() override;

    /**
     * @brief 공격 사용 상태 설정
     * @param in 사용 상태 (true: 사용 가능, false: 사용 중)
     */
    void Set_use_Ability(bool in) override;

    /**
     * @brief 공격 해제 및 초기화
     * 공격 사용 완료 후 객체 풀로 반환하기 위한 정리 작업
     */
    void Release() override;

    // ==========================================================================
    // 향후 확장을 위한 초기화 함수 (현재 주석 처리)
    // ==========================================================================

    // void Init_Ability(Object* this_hero, Object* Target_hero, const CastSpellINFOR& task, 
    //                   Spellslot* in_Spell_slot, const Slot& in_Slot_Id, const float& Map_Time);

private:
    // ==========================================================================
    // 멤버 변수들
    // ==========================================================================

    mutable std::shared_mutex AttackAbility_mtx;  ///< 데이터 보호용 뮤텍스
    bool use_obj = true;                          ///< 사용 가능 여부 (true: 사용 가능)
};