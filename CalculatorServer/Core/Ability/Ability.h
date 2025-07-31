#pragma once
#include "../utils/Vector.h"

// ���� ����
class Object;
class Spellslot;
struct CastSpellINFOR;

/**
 * @brief ��� �ɷ�(��ų, ����)�� �⺻ �������̽� Ŭ����
 *
 * �� Ŭ������ ���� �� ��� �ɷ� �ý����� �⺻ ������ �����մϴ�.
 * ��ų�� ���� ��� �� �������̽��� �����Ͽ� �ϰ��� ������ �����մϴ�.
 */
class Ability
{
public:
    /**
     * @brief ���� �Ҹ���
     * �Ļ� Ŭ������ �ùٸ� �Ҹ��� �����մϴ�.
     */
    virtual ~Ability() = default;

    /**
     * @brief �ɷ� ��� ���� ���� Ȯ��
     * @return true�� ��� ����, false�� ��� �Ұ�
     */
    virtual bool can_use_Ability() = 0;

    /**
     * @brief �ɷ� ��� ���� ����
     * @param in ��� ���� (true: ��� ����, false: ��� ��)
     */
    virtual void Set_use_Ability(bool in) = 0;

    /**
     * @brief �ɷ� ���� �� �ʱ�ȭ
     * �ɷ� ��� �Ϸ� �� ��ü Ǯ�� ��ȯ�ϱ� ���� ���� �۾�
     */
    virtual void Release() = 0;

    // ���� Ȯ���� ���� �ּ� ó���� �ʱ�ȭ �Լ�
    // virtual void init_Ability(Object* this_hero, Object* Target_hero, 
    //                          const CastSpellINFOR& task, const float& Map_Time) = 0;
};

