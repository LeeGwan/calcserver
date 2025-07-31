#include "OutputComponent.h"
#include "../../Object/Object/ObjectComponent/Object.h"
#include "../../Object/Object/CombatStats/CombatStats.h"
#include "../../Object/Object/Transform/Transform.h"
#include "../../Object/Object/objectState/objectState.h"
#include "../../Object/Object/objectStats/objectStats.h"
#include "../../Object/Object/CharacterMesh/CharacterMesh.h"
#include "../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
#include "../../../RoutineServer/UDP/UdpManager.h"
#include "../../Ability/SkillAbility/SkillAbility.h"
#include "../../Ability/Ability.h"
#include "../MapSchedule.h"
#include "../../Object/Object/Spells/Spellslot/Spellslot.h"
#include"../../Object/Object/Spells/SpellDataResource/SpellDataResource.h"
#include "../../../RoutineServer/Message/OptimizedPacketHandler/OptimizedPacketHandler.h"
#include "../../../RoutineServer/UDP/Send_To_RoutineServer.h"
#include "../../Object/Object/Spells/SpellData/SpellData.h"
#include "../../Object/Object/Spells/SpellBook/SpellBook.h"
#include "../../Object/Object/VMT/VMT.h"
void OutputComponent::SendInitData(int map_id, Object* object, PacketType packet_type)
{
    // �ʱ� ���� ������ ��Ŷ ���� �� ����
    std::string name = object->Get_Name();
    std::string hash = object->Get_Hash();
    Struct_Transform transform = object->Get_Transform_Component();
    Struct_CharacterMesh charactermesh = object->Get_CharacterMesh_Component();
    Struct_Sync_data init_data;
    init_data.attack_range = object->Get_ObjectStats_Component().Get_total_Attack_range();
    if (object->Get_VMT_Component()->is_hero())
    {
        init_data.CoolDownTime = object->Get_P_SpellBook()->get_slotDB_cooldown_time();
    }
    init_data.CurrentOrderType = object->Get_objectState_Component().Get_CurrentOrderType();
    init_data.health = object->Get_ObjectStats_Component().Get_health();
    init_data.maxhealth = object->Get_ObjectStats_Component().Get_maxhelth();
    init_data.Level = object->Get_objectState_Component().Get_Level();
    init_data.mana = object->Get_ObjectStats_Component().Get_mana();
    init_data.maxmana = object->Get_ObjectStats_Component().Get_maxmana();
    init_data.ObjectState = object->Get_objectState_Component().Get_ObjectState();
    init_data.total_AD = object->Get_CombatStats_Component().Get_total_AD();
    init_data.total_AP= object->Get_CombatStats_Component().Get_total_AP();
    init_data.total_armor = object->Get_CombatStats_Component().Get_total_armor();
    init_data.total_magic_armor = object->Get_CombatStats_Component().Get_total_magic_RES();
    unsigned long objecttype = object->Get_objectState_Component().Get_ObjectTypeFlag();
    auto packet = OptimizedPacketHandler::CreateCompactCreateMapDataPacket(map_id, name, hash, transform, charactermesh, objecttype, init_data);
    G_MapSchedule->Get_UDP()->Get_Send_To_Routine().PushSendTaskToQueue(packet);

}



void OutputComponent::ProcessObjectUpdates(int map_id, Object* object)
{
    // �� ������Ʈ�� ��ȭ ���� �� ó��
    ProcessTransformUpdates(map_id, object);
    ProcessCombatStatsUpdates(map_id, object);
    ProcessObjectStatsUpdates(map_id, object);
    ProcessObjectStateUpdates(map_id, object);
}
void OutputComponent::ProcessSpellInit(int map_id, SkillAbility* skillobject)
{

    Vec3 startPos = skillobject->Get_StartPosion();
    Vec3 endPos = skillobject->Get_EndPosion();
    auto spell_data = skillobject->Get_P_Spell_Slot()->Get_SpellData()->Get_Spell_Data_Resource();
    float radius = spell_data->Get_Spell_Base_CastRadius();
    SpellType Spell_Type = spell_data->Get_SpellType();
    auto packet = OptimizedPacketHandler::CreateCompactInitCastSpellPacket(map_id, skillobject->Get_skillid(), startPos, endPos, radius, Spell_Type);
    G_MapSchedule->Get_UDP()->Get_Send_To_Routine().PushSendTaskToQueue(packet);

}

void OutputComponent::ProcessSpellUpdates(int map_id, unsigned int skillid,const Vec3& startPos)
{
    auto packet = OptimizedPacketHandler::CreateCompactUpdateCastSpellPacket(map_id, skillid, startPos);
    G_MapSchedule->Get_UDP()->Get_Send_To_Routine().PushSendTaskToQueue(packet);
}
void OutputComponent::DeleteSpell(int map_id, unsigned int Skillid)
{
    auto packet = OptimizedPacketHandler::CreateCompactDeleteCastSpellPacket(map_id, Skillid);
    G_MapSchedule->Get_UDP()->Get_Send_To_Routine().PushSendTaskToQueue(packet);

}

template<typename PacketT, typename ValueT>
void OutputComponent::SendPacket(int map_id, const std::string& object_hash, PacketType packet_type, const ValueT& value)
{
    std::vector<uint8_t> packet_data;

    // ValueT�� ���� ���� Ÿ�� (int, float, uint8_t, Vec3)�̾�� ��
    if constexpr (std::is_same_v<ValueT, int> || std::is_same_v<ValueT, unsigned int>) {
        packet_data = OptimizedPacketHandler::CreateCompactIntPacket(packet_type, map_id,object_hash, value);
    }
    else if constexpr (std::is_same_v<ValueT, float>) {
        packet_data = OptimizedPacketHandler::CreateCompactFloatPacket(packet_type, map_id, object_hash, value);
    }
    else if constexpr (std::is_same_v<ValueT, uint8_t>) {
        packet_data = OptimizedPacketHandler::CreateCompactUInt8Packet(packet_type, map_id, object_hash, value);
    }
    else if constexpr (std::is_same_v<ValueT, Vec3>) {
        packet_data = OptimizedPacketHandler::CreateCompactVec3Packet(packet_type, map_id, object_hash ,value);
    }
    else {
        static_assert(!std::is_same_v<ValueT, ValueT>, "SendPacket���� �������� �ʴ� �� Ÿ���Դϴ�");
    }
    G_MapSchedule->Get_UDP()->Get_Send_To_Routine().PushSendTaskToQueue(packet_data);
}

template<typename ComponentT, typename FlagT, typename PacketT, typename ValueT>
void OutputComponent::CheckAndSendUpdate(ComponentT& component, FlagT flag, int map_id,
    const std::string& object_hash, PacketType packet_type,
    const ValueT& value)
{
    // �÷��� üũ �� ��Ŷ �����ϴ� ���� ����
    if (component.HasFlag(flag)) {
        component.ClearFlag(flag);
        SendPacket<PacketT>(map_id, object_hash, packet_type, value);
    }
}

void OutputComponent::ProcessTransformUpdates(int map_id, Object* object)
{
    auto& transform = object->Get_Transform_Component();
    const std::string& object_hash = object->Get_Hash();

    // ��ġ ��ȭ üũ �� ����
    CheckAndSendUpdate<decltype(transform), TransformChangedFlags, Vec3_packet>(
        transform, TransformChangedFlags::PositionChanged, map_id, object_hash,
        PacketType::TransformPosition, transform.Get_Position());

    // ȸ�� ��ȭ üũ �� ����
    CheckAndSendUpdate<decltype(transform), TransformChangedFlags, Vec3_packet>(
        transform, TransformChangedFlags::RotationChanged, map_id, object_hash,
        PacketType::TransformRotation, transform.Get_Rotation());
}

void OutputComponent::ProcessCombatStatsUpdates(int map_id, Object* object)
{
    auto& combat_stats = object->Get_CombatStats_Component();
    const std::string& object_hash = object->Get_Hash();

    // ���� ��ȭ üũ
    CheckAndSendUpdate<decltype(combat_stats), CombatStatsChangedFlags, float_packet>(
        combat_stats, CombatStatsChangedFlags::ArmorChanged, map_id, object_hash,
        PacketType::CombatStatsTotalArmor, combat_stats.Get_total_armor());

    // ���� ���׷� ��ȭ üũ
    CheckAndSendUpdate<decltype(combat_stats), CombatStatsChangedFlags, float_packet>(
        combat_stats, CombatStatsChangedFlags::magic_RESChanged, map_id, object_hash,
        PacketType::CombatStatsTotalMagicRes, combat_stats.Get_total_magic_RES());

    // ���� ���ݷ� ��ȭ üũ
    CheckAndSendUpdate<decltype(combat_stats), CombatStatsChangedFlags, float_packet>(
        combat_stats, CombatStatsChangedFlags::ADChanged, map_id, object_hash,
        PacketType::CombatStatsTotalAD, combat_stats.Get_total_AD());

    // ���� ���ݷ� ��ȭ üũ
    CheckAndSendUpdate<decltype(combat_stats), CombatStatsChangedFlags, float_packet>(
        combat_stats, CombatStatsChangedFlags::APChanged, map_id, object_hash,
        PacketType::CombatStatsTotalAP, combat_stats.Get_total_AP());
}

void OutputComponent::ProcessObjectStatsUpdates(int map_id, Object* object)
{
    auto& object_stats = object->Get_ObjectStats_Component();
    const std::string& object_hash = object->Get_Hash();

    // ���� ���� ��ȭ üũ
    CheckAndSendUpdate<decltype(object_stats), ObjectStatsChangedFlags, float_packet>(
        object_stats, ObjectStatsChangedFlags::attack_range_Changed, map_id, object_hash,
        PacketType::ObjectStatsAttackRange, object_stats.Get_total_Attack_range());

    // ���� ü�� ��ȭ üũ
    CheckAndSendUpdate<decltype(object_stats), ObjectStatsChangedFlags, float_packet>(
        object_stats, ObjectStatsChangedFlags::healthChanged, map_id, object_hash,
        PacketType::ObjectStatsHealth, object_stats.Get_health());

    // �ִ� ü�� ��ȭ üũ
    CheckAndSendUpdate<decltype(object_stats), ObjectStatsChangedFlags, float_packet>(
        object_stats, ObjectStatsChangedFlags::maxhealthChanged, map_id, object_hash,
        PacketType::ObjectStatsMaxHealth, object_stats.Get_maxhelth());

    // ���� ���� ��ȭ üũ
    CheckAndSendUpdate<decltype(object_stats), ObjectStatsChangedFlags, float_packet>(
        object_stats, ObjectStatsChangedFlags::manaChanged, map_id, object_hash,
        PacketType::ObjectStatsMana, object_stats.Get_mana());

    // �ִ� ���� ��ȭ üũ
    CheckAndSendUpdate<decltype(object_stats), ObjectStatsChangedFlags, float_packet>(
        object_stats, ObjectStatsChangedFlags::maxmanaChanged, map_id, object_hash,
        PacketType::ObjectStatsMaxMana, object_stats.Get_maxmana());
}

void OutputComponent::ProcessObjectStateUpdates(int map_id, Object* object)
{
    auto& object_state = object->Get_objectState_Component();
    const std::string& object_hash = object->Get_Hash();

    // ���� ��ȭ üũ (Ư�� ó�� �ʿ�)
    if (object_state.HasFlag(objectStateChangedFlags::Level_Changed)) {
        object_state.ClearFlag(objectStateChangedFlags::Level_Changed);
        object_state.Set_Anti_Check_Level(true); // ���� üũ ���� �÷��� ����
        SendPacket<int_packet>(map_id, object_hash, PacketType::ObjectStateLevel,
            object_state.Get_Level());
    }

    // ����ġ ��ȭ üũ
    CheckAndSendUpdate<decltype(object_state), objectStateChangedFlags, int_packet>(
        object_state, objectStateChangedFlags::XPChanged, map_id, object_hash,
        PacketType::ObjectStateXP, object_state.Get_XP());

    // �ִ� ����ġ ��ȭ üũ
    CheckAndSendUpdate<decltype(object_state), objectStateChangedFlags, int_packet>(
        object_state, objectStateChangedFlags::MaxXPChanged, map_id, object_hash,
        PacketType::ObjectStateMaxXP, object_state.Get_MaxXP());

    // ��� �ð� ��ȭ üũ
    CheckAndSendUpdate<decltype(object_state), objectStateChangedFlags, float_packet>(
        object_state, objectStateChangedFlags::DeadTimeChanged, map_id, object_hash,
        PacketType::ObjectStateDeadTime, object_state.Get_DeadTime());

    // ��ü ���� ��ȭ üũ
    CheckAndSendUpdate<decltype(object_state), objectStateChangedFlags, uint8_t_packet>(
        object_state, objectStateChangedFlags::ObjectStateChanged, map_id, object_hash,
        PacketType::ObjectStateObjectState, object_state.Get_ObjectState());

    // ���� ��� Ÿ�� ��ȭ üũ
    CheckAndSendUpdate<decltype(object_state), objectStateChangedFlags, uint8_t_packet>(
        object_state, objectStateChangedFlags::CurrentOrderTypeChanged, map_id, object_hash,
        PacketType::ObjectStateCurrentOrderType, object_state.Get_CurrentOrderType());
}

void OutputComponent::ProcessSpellBookStateUpdates(int map_id, Object* object)
{
    const std::string& object_hash = object->Get_Hash();
    PacketType type;

    for (int i = 0; i < 4; ++i)
    {
        Spellslot* slot = object->Get_P_SpellBook()->get_slot(i);
        if (i == 0)type = PacketType::CooldownTimeQ;
        else if(i == 1)type = PacketType::CooldownTimeW;
        else if (i == 2)type = PacketType::CooldownTimeE;
        else if (i == 3)type = PacketType::CooldownTimeR;
        // ��ٿ� �ð� ��ȭ üũ �� ����
        CheckAndSendUpdate<Spellslot, spellslotChangedFlags, float_packet>(
            *slot, spellslotChangedFlags::CoolDownTimeChanged, map_id, object_hash,
            type, slot->Get_CoolDownTime());
    }
}
