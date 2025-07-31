#pragma once

#include <vector>
#include <memory>
#include <array>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
// Forward declarations
namespace ErrorCode { enum Input_ErrorType : uint8_t; enum Spell_ErrorType : uint8_t; enum Level_Input_ErrorType : uint8_t; };
namespace Enums { enum CurrentOrderType : uint8_t; };
enum class SpellType : unsigned int;
enum class Slot : unsigned int;
enum class SpellState : int;
class Spellslot;
class  SkillDetected;
class NavMesh;
struct CastSpellINFOR;
struct Struct_SpellDB;
class Object;
class SkillAbility;
class OutputComponent;
struct SkillStatusInfo;
class Vec3;
/**
 * SpellBook - ���� ���� �ý���
 * ���� ���� ����, ���� ĳ����, ��ٿ� ó�� ���� ���
 */
class SpellBook final
{
public:
    static constexpr size_t MAX_SPELL_SLOTS = 6;    // �ִ� ���� ���� ��
    static constexpr size_t BASIC_SPELL_SLOTS = 4;  // �⺻ ���� ���� �� (Q,W,E,R)

    SpellBook();
    ~SpellBook();

    // ����/�̵� ������ �� ���� ������ ����
    SpellBook(const SpellBook&) = delete;
    SpellBook& operator=(const SpellBook&) = delete;


    // ���� ���� ���� �޼���
    Spellslot* get_slot(uint8_t index);
    const Spellslot* get_slot(uint8_t index) const;
    std::vector<float>get_slotDB_cooldown_time() const;
    float get_slotDB_cooldown_time(uint8_t index) const;

    // �����ͺ��̽����� ���� ���� �ε�
    void import_from_database(const std::vector<Struct_SpellDB>& spell_db);
    //���� ��Ÿ��
    void Update_cooldowntime(const float& deltatime);
    // ���� ĳ���� ���� �޼���
    ErrorCode::Spell_ErrorType try_prepare_spell_slot(
        Object* caster,
        Object* target,
        const CastSpellINFOR& cast_info,
        Spellslot*& out_spell_slot,
        Slot& out_slot,
        float delta_time) const;

    ErrorCode::Spell_ErrorType create_spell_object(
        NavMesh* nav_mesh,
        SkillDetected*& m_SkillDetected,
        SkillAbility* cast_info,
        Object* caster,
        Object* target,
        const CastSpellINFOR& task,
        Spellslot* spell_slot,
        Slot slot,
        float delta_time) const;

    ErrorCode::Spell_ErrorType process_spell_cast(
        const std::unordered_map<std::string, Object*>& objects,
        OutputComponent* m_outputComponent,
        int m_mapID,
        NavMesh* nav_mesh,
        SkillDetected*& m_SkillDetected,
        SkillAbility* cast_info,
        float delta_time,
        const float& Maptime) const;

    ErrorCode::Spell_ErrorType delete_spell_object(
        NavMesh* nav_mesh,
        SkillDetected*& m_SkillDetected,
        SkillAbility*& cast_info) const;

    // ���ҽ� ����
    void clear();
    SkillStatusInfo analyze_skill_status(NavMesh* navmesh, Object* ai_object, float search_radius);
    float Get_spell_range(uint8_t index);
    float Get_spell_missilespeed(uint8_t index);
private:
    std::array<std::unique_ptr<Spellslot>, BASIC_SPELL_SLOTS> spell_slots_;
    mutable std::shared_mutex m_SkillDetectedMutex;

    // ���� �޼����
  
    SpellState get_spell_state(Object* caster, uint8_t index) const;
    ErrorCode::Spell_ErrorType convert_spell_state_to_error(SpellState spell_state) const;
    uint8_t get_slot_index_from_order_type(Enums::CurrentOrderType order_type, Slot& out_slot) const;
    bool is_valid_slot_index(uint8_t index) const;
    void analyze_individual_skill_details(NavMesh* navmesh, Object* caster, Slot skill_slot, float search_radius, 
        float& cooldown_ratio, float& mana_cost, bool& has_enough_mana, unsigned int& skill_level, Vec3& blinkpos, SpellType& spelltype);
    Vec3 findgoodpos_for_blink(NavMesh* navmesh, Object* object, float search_radius, uint8_t slot_index);
};