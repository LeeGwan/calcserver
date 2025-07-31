#pragma once
#include "../utils/Vector.h"

// 전방 선언
class Object;
class Spellslot;
struct CastSpellINFOR;

/**
 * @brief 모든 능력(스킬, 공격)의 기본 인터페이스 클래스
 *
 * 이 클래스는 게임 내 모든 능력 시스템의 기본 구조를 정의합니다.
 * 스킬과 공격 모두 이 인터페이스를 구현하여 일관된 동작을 보장합니다.
 */
class Ability
{
public:
    /**
     * @brief 가상 소멸자
     * 파생 클래스의 올바른 소멸을 보장합니다.
     */
    virtual ~Ability() = default;

    /**
     * @brief 능력 사용 가능 여부 확인
     * @return true면 사용 가능, false면 사용 불가
     */
    virtual bool can_use_Ability() = 0;

    /**
     * @brief 능력 사용 상태 설정
     * @param in 사용 상태 (true: 사용 가능, false: 사용 중)
     */
    virtual void Set_use_Ability(bool in) = 0;

    /**
     * @brief 능력 해제 및 초기화
     * 능력 사용 완료 후 객체 풀로 반환하기 위한 정리 작업
     */
    virtual void Release() = 0;

    // 향후 확장을 위한 주석 처리된 초기화 함수
    // virtual void init_Ability(Object* this_hero, Object* Target_hero, 
    //                          const CastSpellINFOR& task, const float& Map_Time) = 0;
};

