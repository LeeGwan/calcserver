
#include "SpellinputComponent.h"
#include "../../Error/ErrorClass.h"
#include"../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
#include "../../Object/Object/Flags.h"
#include "../../Navmesh/Flags/Flags.h"
#include "../../Object/Object/ObjectComponent/Object.h"
#include "../../Algorithm/SkillDetected/SkillDetected.h"

#include "../../Ability/AbilityPool/AbilityPool.h"
#include "../../Ability/SkillAbility/SkillAbility.h"

#include "../../Object/Object/Spells/SpellBook/SpellBook.h"
#include "../../Object/Object/VMT/VMT.h"

#include "../../Object/Object/Spells/SpellFlags/SpellFlags.h"
#include "../../Object/Object/Spells/Spellslot/Spellslot.h"

#include "../../utils/Find_OBJ/Find_OBJ.h"

#include "../../Navmesh/NavMesh.h"
#include "../../Error/is_valid.h"

#include "../Output/OutputComponent.h"
#include "../../utils/VectorUtils.h"


#include "../../Object/Object/Spells/SpellTargetClient/SpellTargetClient.h"

SpellinputComponent::SpellinputComponent()
{
}

SpellinputComponent::~SpellinputComponent() = default;

ErrorCode::Spell_ErrorType SpellinputComponent::InitializeSpellCasting(
    NavMesh* Nav_Mesh,
    SkillDetected*& m_SkillDetected,
    AbilityPool* in_Ability_Pool,
    SkillAbility*& Out_Skill_Ability,
    const CastSpellINFOR& task,
    const std::unordered_map<std::string, Object*>& OBJS,
    const float& Deltatime
) // const ����
{
    // �ؽ� ����
    if (task.ObjectHash.empty())
    {
        return ErrorCode::Spell_ErrorType::Spell_Error_Dont_have_hash;
    }
  
    // ĳ���� ������Ʈ ã��
    Object* caster_object = nullptr;
    if (!Find_OBJ::Get_OBJ_To_Use_Hash(OBJS, task.ObjectHash, caster_object))
    {
        return ErrorCode::Spell_ErrorType::Spell_Error_Dont_have_OBJ;
    }

    // ĳ���� ���� ����
    VMT_For_OBJ* vmt = caster_object->Get_VMT_Component();
    if (vmt->is_dead())
    {
        return ErrorCode::Spell_ErrorType::Spell_Error_Is_dead;
    }

    // Ÿ�� ������Ʈ ã�� (�����ص� ��� ����)
    Object* target_object = nullptr;
    Find_OBJ::Get_OBJ_To_Use_Hash(OBJS, "AITEST", target_object);
   
    // Find_OBJ::Get_OBJ_To_Use_Hash(OBJS,  task.TargetObjectHash, target_object);
    // ���� ���� �غ�
    Spellslot* spell_slot = nullptr;
    Slot slot;
    ErrorCode::Spell_ErrorType result = caster_object->Get_P_SpellBook()->try_prepare_spell_slot(
        caster_object, target_object, task, spell_slot, slot, Deltatime
    );

    if (result != ErrorCode::Spell_ErrorType::Spell_Successed)
    {
        return result;
    }

    // ��ų �����Ƽ ȹ��
    if (!in_Ability_Pool->Acquire_object(&Out_Skill_Ability))
    {
        return ErrorCode::Spell_ErrorType::Spell_Error_Cant_Acquire_object;
    }

    // ���� ������Ʈ ����
    result = caster_object->Get_P_SpellBook()->create_spell_object(
        Nav_Mesh, m_SkillDetected, Out_Skill_Ability, caster_object, target_object, task, spell_slot, slot, Deltatime
    );

    // ���� �� ������Ʈ ���� ������Ʈ
    if (result == ErrorCode::Spell_Successed)
    {
        // ��ų���� �Ϸ�
        auto& object_state = caster_object->Get_objectState_Component();
        object_state.Set_BeforeOrderType(object_state.Get_CurrentOrderType());
        object_state.Set_CurrentOrderType(FromSlot(slot));
        object_state.Set_IsStateRestored(false);
    }

    return result;
}

ErrorCode::Spell_ErrorType SpellinputComponent::UpdateSpellCasting(
    const std::unordered_map<std::string, Object*>& objects,
    OutputComponent* m_outputComponent,
    int m_mapID,
    NavMesh* Nav_Mesh,
    SkillDetected*& m_SkillDetected,
    SkillAbility*& Skill,
    const float& Deltatime,
    const float& Maptime
) // const ����
{
    // ĳ���� ������Ʈ ��ȿ�� ����
    Object* caster_object = Skill->Get_P_Spell_Slot()->Get_Target_Client()->Get_CasterObject();
    if (!MemoryCheck::is_valid<Object>(caster_object))
    {
        return ErrorCode::Spell_ErrorType::Spell_Error_Dont_have_OBJ;
    }

    // �ð� ���� ���� ������Ʈ
    unsigned int skillid = Skill->Get_skillid();
    float cast_delay_time = Skill->Get_Cast_Delay() - Deltatime;
    float cooldown_time = Skill->Get_P_Spell_Slot()->Get_CoolDownTime() - Deltatime;

    auto& object_state = caster_object->Get_objectState_Component();

    // ĳ��Ʈ ������ �Ϸ� �� ó��
    if (cast_delay_time < 0.0f)
    {
        if (Skill->Get_Spell_Type() == SpellType::Blink)
        {
            Skill->Release();
            return ErrorCode::Spell_ErrorType::Spell_Successed;
        }

        // ���� ���� ó��
        if (!object_state.Get_IsStateRestored() &&
            object_state.Get_BeforeOrderType() != object_state.Get_CurrentOrderType())
        {
            object_state.Set_CurrentOrderType(object_state.Get_BeforeOrderType());
            object_state.Set_IsStateRestored(true);
        }

        caster_object->Get_P_SpellBook()->process_spell_cast(objects, m_outputComponent, m_mapID, Nav_Mesh, m_SkillDetected, Skill, Deltatime, Maptime);
    }
    else
    {
 
        // ĳ���� ȸ�� ó��
        Vec3 start_pos = caster_object->Get_Transform_Component().Get_Position();
        Vec3 end_pos = Skill->Get_EndPosion();
        float angle=caster_object->Get_m_VectorUtils()->GetLerpForAngle(start_pos, end_pos, Deltatime);
        caster_object->Get_Transform_Component().Set_Rotation(Vec3(0.0f, 0.0f, angle));
    }

    // Ÿ�̹� ������Ʈ
    Skill->Set_Cast_Delay(cast_delay_time);

    return ErrorCode::Spell_ErrorType::Spell_Successed;
}

void SpellinputComponent::UpdateCooldown(Object* object, const float& Deltatime) // const ����
{
    object->Get_P_SpellBook()->Update_cooldowntime(Deltatime);
    return;
}