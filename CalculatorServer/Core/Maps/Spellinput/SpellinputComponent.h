#pragma once

#include <memory>
#include <unordered_map>
#include <string>
// ���� ���� - ���ʿ��� include ����

namespace ErrorCode { enum Spell_ErrorType : uint8_t; };
class SkillDetected;
class Object;
class NavMesh;
class VMT_For_OBJ;
struct CastSpellINFOR;
class AbilityPool;
class SkillAbility;
class OutputComponent;
class VectorUtils;

/**
 * @brief ���� �Է� �� ó���� ����ϴ� ������Ʈ
 * ���� ĳ���� �ʱ�ȭ, ������Ʈ, ���� ������ ����
 */
class SpellinputComponent final
{
public:
    SpellinputComponent();
    ~SpellinputComponent();

    // ����/�̵� ���� (�ʿ�� ����)
    SpellinputComponent(const SpellinputComponent&) = delete;
    SpellinputComponent& operator=(const SpellinputComponent&) = delete;
    SpellinputComponent(SpellinputComponent&&) = delete;
    SpellinputComponent& operator=(SpellinputComponent&&) = delete;

    /**
     * @brief ���� ĳ���� �ʱ�ȭ
     * @param Nav_Mesh ������̼� �޽�
     * @param m_SkillDetected ��ų ���� ��ü ����
     * @param in_Ability_Pool �����Ƽ Ǯ
     * @param Out_Skill_Ability ��µ� ��ų �����Ƽ ����
     * @param task ĳ���� ����
     * @param OBJS ������Ʈ ��
     * @param Deltatime ��ŸŸ��
     * @return ���� �ڵ�
     */
    ErrorCode::Spell_ErrorType InitializeSpellCasting(
        NavMesh* Nav_Mesh,
        SkillDetected*& m_SkillDetected,
        AbilityPool* in_Ability_Pool,
        SkillAbility*& Out_Skill_Ability,
        const CastSpellINFOR& task,
        const std::unordered_map<std::string, Object*>& OBJS,
        const float& Deltatime
    ); // const ����

    /**
     * @brief ���� ������Ʈ ó��
     * @param objects ������Ʈ ��
     * @param m_outputComponent ��� ������Ʈ
     * @param m_mapID �� ID
     * @param Nav_Mesh ������̼� �޽�
     * @param m_SkillDetected ��ų ���� ��ü ����
     * @param Skill ��ų �����Ƽ ����
     * @param Deltatime ��ŸŸ��
     * @return ���� �ڵ�
     */
    ErrorCode::Spell_ErrorType UpdateSpellCasting(
        const std::unordered_map<std::string, Object*>& objects,
        OutputComponent* m_outputComponent,
        int m_mapID,
        NavMesh* Nav_Mesh,
        SkillDetected*& m_SkillDetected,
        SkillAbility*& Skill,
        const float& Deltatime,
        const float&Maptime
    ); // const ����

    /**
     * @brief ��ٿ� ������Ʈ
     * @param object ������Ʈ
     * @param Deltatime ��ŸŸ��
     */
    void UpdateCooldown(Object* object, const float& Deltatime); // const ����


};