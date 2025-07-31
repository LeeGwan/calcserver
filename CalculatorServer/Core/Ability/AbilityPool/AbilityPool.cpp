#include "AbilityPool.h"
#include "../Ability.h"
#include "../SkillAbility/SkillAbility.h"
#include "../AttackAbility/AttackAbility.h"
// ==========================================================================
// ������ - �ʱ� ��ü Ǯ ����
// ==========================================================================

AbilityPool::AbilityPool(int size)
{
    // ������ ũ�⸸ŭ �� �ɷ� Ÿ�Ժ��� ��ü�� �̸� ����
    // �̷��� �̸� ���������ν� ��Ÿ�� �� �޸� �Ҵ����� ���� ������ ����
    for (int i = 0; i < size; ++i)
    {
        // ��ų �ɷ� ��ü ���� �� Ǯ�� �߰�
        // make_unique�� ���� �������� �޸� �������� ����
        Spell_pool.push_back(std::make_unique<SkillAbility>());
        Spell_pool[i]->Set_Skillid(i);
        // ���� �ɷ� ��ü ���� �� Ǯ�� �߰�
        Attack_pool.push_back(std::make_unique<AttackAbility>());
    }
}

AbilityPool::~AbilityPool() = default;


// ==========================================================================
// ���ø� �Լ� ���� - ��ü ȹ��
// ==========================================================================

/**
 * @brief �ɷ� ��ü ȹ�� (���ø� ����)
 *
 * ���� ����:
 * 1. ���ؽ� �� ȹ�� (������ ������ ����)
 * 2. �ش� Ÿ���� Ǯ���� ��� ������ ��ü �˻�
 * 3. ��� ������ ��ü�� ������ ��� ������ ǥ���ϰ� ��ȯ
 * 4. ������ ���ο� ��ü�� �������� �����Ͽ� Ǯ�� �߰� �� ��ȯ
 *
 * ���ǻ���:
 * - ��ȯ�� ��ü�� �ݵ�� Release_object()�� ��ȯ�ؾ� ��
 * - OutType�� null�� �ƴ� ��ȿ�� �������� �ּҿ��� ��
 */
template<typename T>
bool AbilityPool::Acquire_object(T** OutType)
{
    // ������ �������� ���� ����ũ �� (���� �۾�)
    // ���� �����尡 ���ÿ� ��ü�� ��û�ص� �����ϰ� ó��
    std::unique_lock lock(mtx);

    // Ÿ�Կ� �´� Ǯ ���� (������ Ÿ�ӿ� ������)
    auto& typedPool = AbilityPoolSelector<T>::GetPool(*this);

    // ���� ��ü �� ��� ������ ���� �ִ��� ���� �˻�
    for (auto& item : typedPool)
    {
        if (item->can_use_Ability())
        {
            // ��� ������ ǥ�� (false = ��� ��, true = ��� ����)
            item->Set_use_Ability(false);

            // ������ ��ȯ (raw �����ͷ� ��ȯ�ϵ� �������� Ǯ�� ����)
            *OutType = item.get();
            return true;
        }
    }

    // ��� ������ ��ü�� ������ ���� ���� (���� Ȯ��)
    // �̴� �ʱ� Ǯ ũ�Ⱑ ������ ���� ������ġ ����
    typedPool.push_back(std::make_unique<T>());

    // ���� ������ ��ü�� ��� ��� ������ ǥ��
    typedPool.back()->Set_use_Ability(false);

    // �� ��ü�� ������ ��ȯ
    *OutType = typedPool.back().get();
    return true;
}

// ==========================================================================
// ���ø� �Լ� ���� - ��ü ��ȯ
// ==========================================================================

/**
 * @brief �ɷ� ��ü ��ȯ (���ø� ����)
 *
 * ���� ����:
 * 1. ���ؽ� �� ȹ�� (������ ������ ����)
 * 2. �ش� Ÿ���� Ǯ���� ��ȯ�� ��ü �˻� (������ ��)
 * 3. ��ü�� ã���� Release() ȣ���Ͽ� ��� ���� ���·� ����
 *
 * ���ǻ���:
 * - TempOBJ�� �ݵ�� �� Ǯ���� ȹ���� ��ü���� ��
 * - �̹� ��ȯ�� ��ü�� �ٽ� ��ȯ�ϸ� �� �� (�ߺ� ��ȯ ���� �ʿ�)
 */
template<typename T>
void AbilityPool::Release_object(T* TempOBJ)
{
    // ������ �������� ���� ����ũ �� (���� �۾�)
    std::unique_lock lock(mtx);

    // Ÿ�Կ� �´� Ǯ ����
    auto& typedPool = AbilityPoolSelector<T>::GetPool(*this);

    // Ǯ���� �ش� ��ü ã�� (������ �ּ� ��)
    for (auto& item : typedPool)
    {
        if (item.get() == TempOBJ)
        {
            // ��ü ���� �� �ʱ�ȭ (��� ���� ���·� ����)
            // Release()�� ��ü ���� ���¸� �����ϰ� use_obj�� true�� ����
            item->Release();
            return;
        }
    }

    // ����: ���⿡ �����ϸ� �߸��� �����Ͱ� ���޵� ����
    // ����� ��忡���� assert �Ǵ� �α׸� �߰��ϴ� ���� ����
}

// ==========================================================================
// ����� ���ø� �ν��Ͻ�ȭ
// ==========================================================================

/**
 * @brief ���ø� �Լ��� ����� �ν��Ͻ�ȭ
 *
 * �����Ϸ��� SkillAbility�� AttackAbility Ÿ�Կ� ����
 * ���ø� �Լ����� �̸� �����ϵ��� �����մϴ�.
 * �̴� ��ŷ ������ �����ϰ� ������ ����ŵ�ϴ�.
 */

 // SkillAbility Ÿ�Կ� ���� ����� �ν��Ͻ�ȭ
template bool AbilityPool::Acquire_object(SkillAbility**);
template void AbilityPool::Release_object(SkillAbility*);

// AttackAbility Ÿ�Կ� ���� ����� �ν��Ͻ�ȭ
template bool AbilityPool::Acquire_object(AttackAbility**);
template void AbilityPool::Release_object(AttackAbility*);

// ==========================================================================
// Ǯ ��ü ����
// ==========================================================================

void AbilityPool::Release()
{
    // ��� Ǯ ����
    // unique_ptr �����̳��̹Ƿ� clear() ȣ�� �� �ڵ����� ��� ��ü�� �Ҹ��ڰ� ȣ���
    // �޸� ���� ���� ���� �����ϰ� ������
    Spell_pool.clear();
    Attack_pool.clear();

    // ����: �� �Լ� ȣ�� �Ŀ��� ������ ȹ���� ��� �����Ͱ� ��ȿȭ��
    // ���� ���� ���� ���� �ÿ��� ȣ���ؾ� ��
}