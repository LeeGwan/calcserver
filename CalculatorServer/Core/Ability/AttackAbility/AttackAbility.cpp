#include "AttackAbility.h"
#include "../../Object/Object/ObjectComponent/Object.h"

// ==========================================================================
// Ability �������̽� ����
// ==========================================================================

bool AttackAbility::can_use_Ability()
{
    // ���� �ɷ��� ��� ������ ���ؽ� ���� �ʿ����� ���� ������ �ܼ���
    // ������ �ϰ����� ���� ��� ������ ����
    return use_obj;
}

void AttackAbility::Set_use_Ability(bool in)
{
    // ���� ��� ���� ����
    // true: ��� ����, false: ��� ��
    use_obj = in;
}

void AttackAbility::Release()
{
    // ���� �Ϸ� �� ��� ���� ���·� ����
    // ��ü Ǯ�� ��ȯ�ϱ� ���� ���� �۾�
    use_obj = true;
}