#pragma once
#include <memory>
#include <shared_mutex>
#include <mutex>
#include <vector>

class SkillAbility;
class AttackAbility;
/**
 * @brief �ɷ� ��ü���� ȿ�������� �����ϴ� ��ü Ǯ Ŭ����
 *
 * �� Ŭ������ �޸� �Ҵ�/���� ����� ���̱� ���� �ɷ� ��ü���� �̸� �����ϰ� �����մϴ�.
 * ��Ƽ������ ȯ�濡�� �����ϰ� �����ϵ��� ����Ǿ����ϴ�.
 *
 * ��ü Ǯ ������ ����:
 * - �޸� �Ҵ�/���� ��� ���� (new/delete ȣ�� �ּ�ȭ)
 * - �޸� ����ȭ ����
 * - ������ ���� ���� (���� ������ �޸� ��뷮)
 * - ���� ��Ÿ�� �� ���� ���� ���� (������ �÷��� ����)
 */
class AbilityPool
{
public:
    /**
     * @brief ������ - ������ ũ�⸸ŭ �ɷ� ��ü���� �̸� ����
     * @param size �ʱ� ������ �� �ɷ� Ÿ�Ժ� ��ü ����
     *
     * ���� ���� ���� �� ����Ǵ� ���� ��ų ��뷮�� ����Ͽ�
     * ������ ũ��� �����ؾ� �մϴ�.
     */
    AbilityPool(int size);

    /**
     * @brief �Ҹ��� - �⺻ �Ҹ��� ���
     * unique_ptr �����̳ʰ� �ڵ����� �޸𸮸� �����մϴ�.
     */
    ~AbilityPool();

    /**
     * @brief �ɷ� ��ü ȹ��
     * @tparam T ȹ���� �ɷ� Ÿ�� (SkillAbility �Ǵ� AttackAbility)
     * @param OutType ȹ��� ��ü�� ������ �������� �ּ�
     * @return ���� �� true, ���� �� false
     *
     * ��� ����:
     * SkillAbility* skill = nullptr;
     * if (pool.Acquire_object(&skill)) {
     *     // skill ���
     * }
     */
    template<typename T>
    bool Acquire_object(T** OutType);

    /**
     * @brief �ɷ� ��ü ��ȯ
     * @tparam T ��ȯ�� �ɷ� Ÿ�� (SkillAbility �Ǵ� AttackAbility)
     * @param TempOBJ ��ȯ�� ��ü ������
     *
     * ��� �Ϸ�� ��ü�� Ǯ�� ��ȯ�Ͽ� ���� �����ϰ� ����ϴ�.
     */
    template<typename T>
    void Release_object(T* TempOBJ);

    /**
     * @brief ��� Ǯ ����
     * Ǯ�� �ִ� ��� ��ü���� �����մϴ�.
     */
    void Release();

private:
    // ==========================================================================
    // ��� ������
    // ==========================================================================
   // �����Ұ� ���� vector���� �ٸ��ɷ� ����
    std::vector<std::unique_ptr<SkillAbility>> Spell_pool;    ///< ��ų �ɷ� ��ü Ǯ
    std::vector<std::unique_ptr<AttackAbility>> Attack_pool;  ///< ���� �ɷ� ��ü Ǯ
    mutable std::shared_mutex mtx;                           ///< ������ �������� ���� ���ؽ�

    /**
     * @brief Ÿ�Ժ� Ǯ ������ ���� ���ø� ����ü (friend ����)
     * @tparam T �ɷ� Ÿ��
     */
    template<typename T>
    friend struct AbilityPoolSelector;
};

// ==========================================================================
// Ÿ�Ժ� Ǯ ������ ���� Ư��ȭ ����ü��
// ==========================================================================

/**
 * @brief Ÿ�Ժ��� ������ Ǯ�� �����ϴ� ���ø� ����ü
 * @tparam T �ɷ� Ÿ��
 */
template<typename T>
struct AbilityPoolSelector;

/**
 * @brief SkillAbility�� ���� Ư��ȭ
 * ��ų �ɷ� ��ü�� ��� Spell_pool�� ��ȯ�մϴ�.
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
 * @brief AttackAbility�� ���� Ư��ȭ
 * ���� �ɷ� ��ü�� ��� Attack_pool�� ��ȯ�մϴ�.
 */
template<>
struct AbilityPoolSelector<AttackAbility>
{
    static std::vector<std::unique_ptr<AttackAbility>>& GetPool(AbilityPool& pool)
    {
        return pool.Attack_pool;
    }
};