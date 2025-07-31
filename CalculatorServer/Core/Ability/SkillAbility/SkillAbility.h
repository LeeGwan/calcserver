#pragma once
#include <unordered_set>
#include <shared_mutex>
#include <vector>
#include <string>
#include "../Ability.h"
class  SkillDetected;
enum PolyFlags;
// ���� ����
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
 * @brief ��ų �ɷ��� �����ϴ� Ŭ����
 *
 * �� Ŭ������ ���� �� ��� ��ų(����, �̻���, ��Ŭ, ��ũ ��)�� ������ �����մϴ�.
 * ��Ƽ������ ȯ�濡�� �����ϰ� �����ϵ��� ���ؽ��� ����մϴ�.
 */
class SkillAbility : public Ability
{
public:
    SkillAbility();
    ~SkillAbility() = default;
    // ==========================================================================
    // Ability �������̽� ����
    // ==========================================================================

    /**
     * @brief ��ų ��� ���� ���� Ȯ��
     * @return true�� ��� ����, false�� ��� ��
     */
    bool can_use_Ability() override;

    /**
     * @brief ��ų ��� ���� ����
     * @param in_use_obj ��� ���� (true: ��� ����, false: ��� ��)
     */
    void Set_use_Ability(bool in_use_obj) override;

    /**
     * @brief ��ų ���� �� �ʱ�ȭ
     * ��ų ��� �Ϸ� �� ��ü Ǯ�� ��ȯ�ϱ� ���� ���� �۾�
     */
    void Release() override;

    // ==========================================================================
    // ��ų �ʱ�ȭ �� ����
    // ==========================================================================

    /**
     * @brief ��ų �ʱ�ȭ
     * @param Nav_Mesh ������̼� �޽�
     * @param this_hero ��ų �����
     * @param Target_hero Ÿ�� ��� (nullptr ����)
     * @param task ��ų ��� ����
     * @param in_Spell_slot ��ų ���� ����
     * @param in_Slot_Id ���� ID
     * @param Deltatime ��Ÿ Ÿ��
     */
    void Init_Ability(NavMesh* Nav_Mesh, SkillDetected*& m_SkillDetected, Object* this_hero, Object* Target_hero,
        const CastSpellINFOR& task, Spellslot* in_Spell_slot,
        const Slot& in_Slot_Id, const float& Deltatime);

    // ==========================================================================
    // ��ų Ÿ�� Ȯ�� �Լ���
    // ==========================================================================

    bool Is_Spell();    ///< �Ϲ� ��ų ����
    bool Is_Attack();   ///< ���� ��ų ����
    bool Is_Line();     ///< ���� ��ų ����
    bool Is_Missile();  ///< �̻��� ��ų ����
    bool Is_Circle();   ///< ���� ��ų ����
    bool Is_Blink();    ///< �����̵� ��ų ����

    // ==========================================================================
    // Getter �Լ��� (������ ����)
    // ==========================================================================

    std::string Get_Hash() const;           ///< ����� �ؽð�
    std::string Get_TargetHash() const;     ///< Ÿ�� �ؽð�
    Vec3 Get_StartPosion() const;           ///< ���� ��ġ
    Vec3 Get_EndPosion() const;             ///< ���� ��ġ
    Slot Get_Slot_Id() const;               ///< ���� ID
    SpellType Get_Spell_Type() const;       ///< ��ų Ÿ��
    Spellslot* Get_P_Spell_Slot();          ///< ��ų ���� ������
    float Get_SpellDuration_Time() const;   ///< ��ų ���� �ð�
    float Get_Cast_Delay() const;           ///< ĳ���� ���� �ð�


    // ==========================================================================
    // Setter �Լ��� (������ ����)
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
    // ��� ������
    // ==========================================================================

    mutable std::shared_mutex SkillAbility_mtx;      ///< �ֿ� ������ ��ȣ�� ���ؽ�
    bool use_obj = true;                             ///< ��� ���� ����
    std::string Hash;                                ///< ����� �ؽ�
    std::string TargetHash;                          ///< Ÿ�� �ؽ�
    float Cast_Delay;                                ///< ĳ���� ���� �ð�
    float SpellDuration;                             ///< ��ų ���� �ð�
    Vec3 StartPosion;                                ///< ���� ��ġ
    Vec3 EndPosion;                                  ///< ���� ��ġ
    Slot Slot_Id;                                    ///< ���� ID
    SpellType Spell_Type;                            ///< ��ų Ÿ��
    Spellslot* P_Spell_Slot;                         ///< ��ų ���� ������
    
    unsigned int skillid;                            ///< ��ų ID   
};