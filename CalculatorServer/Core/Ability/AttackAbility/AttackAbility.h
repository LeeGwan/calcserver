#pragma once
#include <shared_mutex>
#include "../Ability.h"

// ���� ����
enum class Slot : unsigned int;

/**
 * @brief ���� �ɷ��� �����ϴ� Ŭ����
 *
 * �� Ŭ������ �⺻ ����(Auto Attack)�� �����մϴ�.
 * ��ų�� �޸� ��������� �ܼ��� ������ ������,
 * �ַ� ��� ���� ���θ� �����մϴ�.
 */
class AttackAbility : public Ability
{
public:
    // ==========================================================================
    // Ability �������̽� ����
    // ==========================================================================

    /**
     * @brief ���� ��� ���� ���� Ȯ��
     * @return true�� ��� ����, false�� ��� ��
     */
    bool can_use_Ability() override;

    /**
     * @brief ���� ��� ���� ����
     * @param in ��� ���� (true: ��� ����, false: ��� ��)
     */
    void Set_use_Ability(bool in) override;

    /**
     * @brief ���� ���� �� �ʱ�ȭ
     * ���� ��� �Ϸ� �� ��ü Ǯ�� ��ȯ�ϱ� ���� ���� �۾�
     */
    void Release() override;

    // ==========================================================================
    // ���� Ȯ���� ���� �ʱ�ȭ �Լ� (���� �ּ� ó��)
    // ==========================================================================

    // void Init_Ability(Object* this_hero, Object* Target_hero, const CastSpellINFOR& task, 
    //                   Spellslot* in_Spell_slot, const Slot& in_Slot_Id, const float& Map_Time);

private:
    // ==========================================================================
    // ��� ������
    // ==========================================================================

    mutable std::shared_mutex AttackAbility_mtx;  ///< ������ ��ȣ�� ���ؽ�
    bool use_obj = true;                          ///< ��� ���� ���� (true: ��� ����)
};