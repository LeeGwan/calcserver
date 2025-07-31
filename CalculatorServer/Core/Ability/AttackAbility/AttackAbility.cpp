#include "AttackAbility.h"
#include "../../Object/Object/ObjectComponent/Object.h"

// ==========================================================================
// Ability 인터페이스 구현
// ==========================================================================

bool AttackAbility::can_use_Ability()
{
    // 공격 능력의 경우 별도의 뮤텍스 락이 필요하지 않을 정도로 단순함
    // 하지만 일관성을 위해 멤버 변수에 접근
    return use_obj;
}

void AttackAbility::Set_use_Ability(bool in)
{
    // 공격 사용 상태 설정
    // true: 사용 가능, false: 사용 중
    use_obj = in;
}

void AttackAbility::Release()
{
    // 공격 완료 후 사용 가능 상태로 복원
    // 객체 풀로 반환하기 위한 정리 작업
    use_obj = true;
}