#include "AbilityPool.h"
#include "../Ability.h"
#include "../SkillAbility/SkillAbility.h"
#include "../AttackAbility/AttackAbility.h"
// ==========================================================================
// 생성자 - 초기 객체 풀 생성
// ==========================================================================

AbilityPool::AbilityPool(int size)
{
    // 지정된 크기만큼 각 능력 타입별로 객체를 미리 생성
    // 이렇게 미리 생성함으로써 런타임 중 메모리 할당으로 인한 지연을 방지
    for (int i = 0; i < size; ++i)
    {
        // 스킬 능력 객체 생성 및 풀에 추가
        // make_unique는 예외 안전성과 메모리 안전성을 보장
        Spell_pool.push_back(std::make_unique<SkillAbility>());
        Spell_pool[i]->Set_Skillid(i);
        // 공격 능력 객체 생성 및 풀에 추가
        Attack_pool.push_back(std::make_unique<AttackAbility>());
    }
}

AbilityPool::~AbilityPool() = default;


// ==========================================================================
// 템플릿 함수 구현 - 객체 획득
// ==========================================================================

/**
 * @brief 능력 객체 획득 (템플릿 구현)
 *
 * 동작 과정:
 * 1. 뮤텍스 락 획득 (스레드 안전성 보장)
 * 2. 해당 타입의 풀에서 사용 가능한 객체 검색
 * 3. 사용 가능한 객체가 있으면 사용 중으로 표시하고 반환
 * 4. 없으면 새로운 객체를 동적으로 생성하여 풀에 추가 후 반환
 *
 * 주의사항:
 * - 반환된 객체는 반드시 Release_object()로 반환해야 함
 * - OutType은 null이 아닌 유효한 포인터의 주소여야 함
 */
template<typename T>
bool AbilityPool::Acquire_object(T** OutType)
{
    // 스레드 안전성을 위한 유니크 락 (쓰기 작업)
    // 여러 스레드가 동시에 객체를 요청해도 안전하게 처리
    std::unique_lock lock(mtx);

    // 타입에 맞는 풀 선택 (컴파일 타임에 결정됨)
    auto& typedPool = AbilityPoolSelector<T>::GetPool(*this);

    // 기존 객체 중 사용 가능한 것이 있는지 순차 검색
    for (auto& item : typedPool)
    {
        if (item->can_use_Ability())
        {
            // 사용 중으로 표시 (false = 사용 중, true = 사용 가능)
            item->Set_use_Ability(false);

            // 포인터 반환 (raw 포인터로 반환하되 소유권은 풀이 유지)
            *OutType = item.get();
            return true;
        }
    }

    // 사용 가능한 객체가 없으면 새로 생성 (동적 확장)
    // 이는 초기 풀 크기가 부족할 때의 안전장치 역할
    typedPool.push_back(std::make_unique<T>());

    // 새로 생성된 객체를 즉시 사용 중으로 표시
    typedPool.back()->Set_use_Ability(false);

    // 새 객체의 포인터 반환
    *OutType = typedPool.back().get();
    return true;
}

// ==========================================================================
// 템플릿 함수 구현 - 객체 반환
// ==========================================================================

/**
 * @brief 능력 객체 반환 (템플릿 구현)
 *
 * 동작 과정:
 * 1. 뮤텍스 락 획득 (스레드 안전성 보장)
 * 2. 해당 타입의 풀에서 반환할 객체 검색 (포인터 비교)
 * 3. 객체를 찾으면 Release() 호출하여 사용 가능 상태로 변경
 *
 * 주의사항:
 * - TempOBJ는 반드시 이 풀에서 획득한 객체여야 함
 * - 이미 반환된 객체를 다시 반환하면 안 됨 (중복 반환 방지 필요)
 */
template<typename T>
void AbilityPool::Release_object(T* TempOBJ)
{
    // 스레드 안전성을 위한 유니크 락 (쓰기 작업)
    std::unique_lock lock(mtx);

    // 타입에 맞는 풀 선택
    auto& typedPool = AbilityPoolSelector<T>::GetPool(*this);

    // 풀에서 해당 객체 찾기 (포인터 주소 비교)
    for (auto& item : typedPool)
    {
        if (item.get() == TempOBJ)
        {
            // 객체 해제 및 초기화 (사용 가능 상태로 변경)
            // Release()는 객체 내부 상태를 정리하고 use_obj를 true로 설정
            item->Release();
            return;
        }
    }

    // 주의: 여기에 도달하면 잘못된 포인터가 전달된 것임
    // 디버그 모드에서는 assert 또는 로그를 추가하는 것이 좋음
}

// ==========================================================================
// 명시적 템플릿 인스턴스화
// ==========================================================================

/**
 * @brief 템플릿 함수의 명시적 인스턴스화
 *
 * 컴파일러가 SkillAbility와 AttackAbility 타입에 대해
 * 템플릿 함수들을 미리 생성하도록 지시합니다.
 * 이는 링킹 오류를 방지하고 성능을 향상시킵니다.
 */

 // SkillAbility 타입에 대한 명시적 인스턴스화
template bool AbilityPool::Acquire_object(SkillAbility**);
template void AbilityPool::Release_object(SkillAbility*);

// AttackAbility 타입에 대한 명시적 인스턴스화
template bool AbilityPool::Acquire_object(AttackAbility**);
template void AbilityPool::Release_object(AttackAbility*);

// ==========================================================================
// 풀 전체 해제
// ==========================================================================

void AbilityPool::Release()
{
    // 모든 풀 비우기
    // unique_ptr 컨테이너이므로 clear() 호출 시 자동으로 모든 객체의 소멸자가 호출됨
    // 메모리 누수 걱정 없이 안전하게 정리됨
    Spell_pool.clear();
    Attack_pool.clear();

    // 주의: 이 함수 호출 후에는 이전에 획득한 모든 포인터가 무효화됨
    // 따라서 게임 서버 종료 시에만 호출해야 함
}