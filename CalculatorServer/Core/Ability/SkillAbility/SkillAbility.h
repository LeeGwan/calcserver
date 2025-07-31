#pragma once
#include <unordered_set>
#include <shared_mutex>
#include <vector>
#include <string>
#include "../Ability.h"
class  SkillDetected;
enum PolyFlags;
// 전방 선언
class Object;
struct dtMeshTile;
class NavMesh;
class Spellslot;
enum class Slot : unsigned int;
enum class SpellType : unsigned int;
enum SkillAbilityChangedFlags : uint16_t
{
    use_obj = 1 << 0,  // bit 0
    startPosChanged = 1 << 1,
    EndPosChanged = 1 << 2,
    radiusChanged = 1 << 3,
    SpellSlotChanged = 1 << 4,
};
/**
 * @brief 스킬 능력을 관리하는 클래스
 *
 * 이 클래스는 게임 내 모든 스킬(라인, 미사일, 서클, 블링크 등)의 동작을 관리합니다.
 * 멀티스레드 환경에서 안전하게 동작하도록 뮤텍스를 사용합니다.
 */
class SkillAbility : public Ability
{
public:
    SkillAbility();
    ~SkillAbility() = default;
    // ==========================================================================
    // Ability 인터페이스 구현
    // ==========================================================================

    /**
     * @brief 스킬 사용 가능 여부 확인
     * @return true면 사용 가능, false면 사용 중
     */
    bool can_use_Ability() override;

    /**
     * @brief 스킬 사용 상태 설정
     * @param in_use_obj 사용 상태 (true: 사용 가능, false: 사용 중)
     */
    void Set_use_Ability(bool in_use_obj) override;

    /**
     * @brief 스킬 해제 및 초기화
     * 스킬 사용 완료 후 객체 풀로 반환하기 위한 정리 작업
     */
    void Release() override;

    // ==========================================================================
    // 스킬 초기화 및 설정
    // ==========================================================================

    /**
     * @brief 스킬 초기화
     * @param Nav_Mesh 내비게이션 메쉬
     * @param this_hero 스킬 사용자
     * @param Target_hero 타겟 대상 (nullptr 가능)
     * @param task 스킬 사용 정보
     * @param in_Spell_slot 스킬 슬롯 정보
     * @param in_Slot_Id 슬롯 ID
     * @param Deltatime 델타 타임
     */
    void Init_Ability(NavMesh* Nav_Mesh, SkillDetected*& m_SkillDetected, Object* this_hero, Object* Target_hero,
        const CastSpellINFOR& task, Spellslot* in_Spell_slot,
        const Slot& in_Slot_Id, const float& Deltatime);

    // ==========================================================================
    // 스킬 타입 확인 함수들
    // ==========================================================================

    bool Is_Spell();    ///< 일반 스킬 여부
    bool Is_Attack();   ///< 공격 스킬 여부
    bool Is_Line();     ///< 라인 스킬 여부
    bool Is_Missile();  ///< 미사일 스킬 여부
    bool Is_Circle();   ///< 원형 스킬 여부
    bool Is_Blink();    ///< 순간이동 스킬 여부

    // ==========================================================================
    // Getter 함수들 (스레드 안전)
    // ==========================================================================

    std::string Get_Hash() const;           ///< 사용자 해시값
    std::string Get_TargetHash() const;     ///< 타겟 해시값
    Vec3 Get_StartPosion() const;           ///< 시작 위치
    Vec3 Get_EndPosion() const;             ///< 종료 위치
    Slot Get_Slot_Id() const;               ///< 슬롯 ID
    SpellType Get_Spell_Type() const;       ///< 스킬 타입
    Spellslot* Get_P_Spell_Slot();          ///< 스킬 슬롯 포인터
    float Get_SpellDuration_Time() const;   ///< 스킬 지속 시간
    float Get_Cast_Delay() const;           ///< 캐스팅 지연 시간


    // ==========================================================================
    // Setter 함수들 (스레드 안전)
    // ==========================================================================

    void Set_SpellDuration_Time(const float& in_SpellDuration);
    void Set_Cast_Delay(const float& in_Cast_Delay);
    void Set_Hash(const std::string& in_hash);
    void Set_TargetHash(const std::string& in_TargetHash);
    void Set_StartPosion(const Vec3& in_StartPosion);
    void Set_EndPosion(const Vec3& in_EndPosion);
    void Set_Slot_Id(const Slot& in_Slot_Id);
    void Set_Spell_Type(const SpellType& in_Spell_Type);
    void Set_P_Spell_Slot(Spellslot* in_P_Spell_Slot);
 
    unsigned int Get_skillid()const;
    void Set_Skillid(const unsigned int& inskillid);
private:
    // ==========================================================================
    // 멤버 변수들
    // ==========================================================================

    mutable std::shared_mutex SkillAbility_mtx;      ///< 주요 데이터 보호용 뮤텍스
    bool use_obj = true;                             ///< 사용 가능 여부
    std::string Hash;                                ///< 사용자 해시
    std::string TargetHash;                          ///< 타겟 해시
    float Cast_Delay;                                ///< 캐스팅 지연 시간
    float SpellDuration;                             ///< 스킬 지속 시간
    Vec3 StartPosion;                                ///< 시작 위치
    Vec3 EndPosion;                                  ///< 종료 위치
    Slot Slot_Id;                                    ///< 슬롯 ID
    SpellType Spell_Type;                            ///< 스킬 타입
    Spellslot* P_Spell_Slot;                         ///< 스킬 슬롯 포인터
    
    unsigned int skillid;                            ///< 스킬 ID   
};