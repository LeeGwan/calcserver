#pragma once
#include <memory>
#include <shared_mutex>
#include <mutex>
#include <vector>

class SkillAbility;
class AttackAbility;
/**
 * @brief 능력 객체들을 효율적으로 관리하는 객체 풀 클래스
 *
 * 이 클래스는 메모리 할당/해제 비용을 줄이기 위해 능력 객체들을 미리 생성하고 재사용합니다.
 * 멀티스레드 환경에서 안전하게 동작하도록 설계되었습니다.
 *
 * 객체 풀 패턴의 장점:
 * - 메모리 할당/해제 비용 절약 (new/delete 호출 최소화)
 * - 메모리 파편화 방지
 * - 일정한 성능 보장 (예측 가능한 메모리 사용량)
 * - 게임 런타임 중 끊김 없는 실행 (가비지 컬렉션 없음)
 */
class AbilityPool
{
public:
    /**
     * @brief 생성자 - 지정된 크기만큼 능력 객체들을 미리 생성
     * @param size 초기 생성할 각 능력 타입별 객체 개수
     *
     * 게임 서버 시작 시 예상되는 동시 스킬 사용량을 고려하여
     * 적절한 크기로 설정해야 합니다.
     */
    AbilityPool(int size);

    /**
     * @brief 소멸자 - 기본 소멸자 사용
     * unique_ptr 컨테이너가 자동으로 메모리를 정리합니다.
     */
    ~AbilityPool();

    /**
     * @brief 능력 객체 획득
     * @tparam T 획득할 능력 타입 (SkillAbility 또는 AttackAbility)
     * @param OutType 획득된 객체를 저장할 포인터의 주소
     * @return 성공 시 true, 실패 시 false
     *
     * 사용 예시:
     * SkillAbility* skill = nullptr;
     * if (pool.Acquire_object(&skill)) {
     *     // skill 사용
     * }
     */
    template<typename T>
    bool Acquire_object(T** OutType);

    /**
     * @brief 능력 객체 반환
     * @tparam T 반환할 능력 타입 (SkillAbility 또는 AttackAbility)
     * @param TempOBJ 반환할 객체 포인터
     *
     * 사용 완료된 객체를 풀로 반환하여 재사용 가능하게 만듭니다.
     */
    template<typename T>
    void Release_object(T* TempOBJ);

    /**
     * @brief 모든 풀 해제
     * 풀에 있는 모든 객체들을 제거합니다.
     */
    void Release();

private:
    // ==========================================================================
    // 멤버 변수들
    // ==========================================================================
   // 내일할거 여기 vector말고 다른걸로 하자
    std::vector<std::unique_ptr<SkillAbility>> Spell_pool;    ///< 스킬 능력 객체 풀
    std::vector<std::unique_ptr<AttackAbility>> Attack_pool;  ///< 공격 능력 객체 풀
    mutable std::shared_mutex mtx;                           ///< 스레드 안전성을 위한 뮤텍스

    /**
     * @brief 타입별 풀 선택을 위한 템플릿 구조체 (friend 선언)
     * @tparam T 능력 타입
     */
    template<typename T>
    friend struct AbilityPoolSelector;
};

// ==========================================================================
// 타입별 풀 선택을 위한 특성화 구조체들
// ==========================================================================

/**
 * @brief 타입별로 적절한 풀을 선택하는 템플릿 구조체
 * @tparam T 능력 타입
 */
template<typename T>
struct AbilityPoolSelector;

/**
 * @brief SkillAbility에 대한 특성화
 * 스킬 능력 객체의 경우 Spell_pool을 반환합니다.
 */
template<>
struct AbilityPoolSelector<SkillAbility>
{
    static std::vector<std::unique_ptr<SkillAbility>>& GetPool(AbilityPool& pool)
    {
        return pool.Spell_pool;
    }
};

/**
 * @brief AttackAbility에 대한 특성화
 * 공격 능력 객체의 경우 Attack_pool을 반환합니다.
 */
template<>
struct AbilityPoolSelector<AttackAbility>
{
    static std::vector<std::unique_ptr<AttackAbility>>& GetPool(AbilityPool& pool)
    {
        return pool.Attack_pool;
    }
};