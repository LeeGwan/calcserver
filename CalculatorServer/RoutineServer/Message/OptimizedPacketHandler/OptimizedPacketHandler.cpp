#include "OptimizedPacketHandler.h"
#include "..//PacketStructure/PacketStructure.h"
#include "../../../Core/Object/Object/Flags.h"
#include "../../../Core/Maps/MapSchedule.h"
#include "../../../Core/Maps/Map.h"
#include "../OptimizedBinaryPacketSerializer/OptimizedBinaryPacketSerializer.h"
#include "..//CompactBinaryReader/CompactBinaryReader.h"

// ============ 패킷 생성 함수들 구현 (mapid 첫 번째 필드로 수정) ============

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactMovePacket(int mapid, const std::string& object_hash,
    const std::string& target_hash, const Vec3& endPos, uint8_t order_type) {
    ObjectMoveOrderType packet;
    packet.mapid = mapid;  // mapid가 첫 번째 필드
    packet.Type = PacketType::Input;
    packet.Move_INFOR.ObjectHash = object_hash;
    packet.Move_INFOR.CurrentOrderType = static_cast<Enums::CurrentOrderType>(order_type);
    packet.Move_INFOR.EndPos = endPos;
    packet.Move_INFOR.TargetObjectHash = target_hash;

    return OptimizedBinaryPacketSerializer::SerializeCompactObjectMoveOrderType(packet);
}

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactCastSpellPacket(int mapid, const std::string& object_hash,
    const std::string& target_hash, const Vec3& endPos, uint8_t order_type) {
    ObjectCastOrderType packet;
    packet.mapid = mapid;  // mapid가 첫 번째 필드
    packet.Type = PacketType::CreateCastSpell;
    packet.CastSpell_INFOR.ObjectHash = object_hash;
    packet.CastSpell_INFOR.CurrentOrderType = static_cast<Enums::CurrentOrderType>(order_type);
    packet.CastSpell_INFOR.EndPos = endPos;
    packet.CastSpell_INFOR.TargetObjectHash = target_hash;

    return OptimizedBinaryPacketSerializer::SerializeCompactObjectCastOrderType(packet);
}

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactInitCastSpellPacket(int mapid, uint32_t skillid,
    const Vec3& start, const Vec3& end, float radius, SpellType spell_type) {
    InitCastSpell packet;
    packet.mapid = mapid;  // mapid가 첫 번째 필드
    packet.Type = PacketType::CreateCastSpell;
    packet.Skillid = skillid;
    packet.startPos = start;
    packet.endPos = end;
    packet.radius = radius;
    packet.Spell_Type = spell_type;

    return OptimizedBinaryPacketSerializer::SerializeCompactInitCastSpell(packet);
}

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactUpdateCastSpellPacket(int mapid, uint32_t skillid, const Vec3& start) {
    UpdateCasteSPell packet;
    packet.mapid = mapid;  // mapid가 첫 번째 필드
    packet.Type = PacketType::UpdateCastSpell;
    packet.Skillid = skillid;
    packet.startPos = start;

    return OptimizedBinaryPacketSerializer::SerializeCompactUpdateCastSpell(packet);
}

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactDeleteCastSpellPacket(int mapid, uint32_t skillid) {
    DeleteCasteSPell packet;
    packet.mapid = mapid;  // mapid가 첫 번째 필드
    packet.Type = PacketType::DeleteCastSpell;
    packet.Skillid = skillid;

    return OptimizedBinaryPacketSerializer::SerializeCompactDeleteCastSpell(packet);
}

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactCreateMapDataPacket(int mapid, const std::string& name,
    const std::string& hash, const Struct_Transform& transform,
    const Struct_CharacterMesh& mesh, unsigned long object_flag, const Struct_Sync_data& sync_data) {
    Create_Map_data packet;
    packet.Type = PacketType::CreateMap;
    packet.mapid = mapid;
    packet.Name = name;
    packet.Hash = hash;
    packet.S_Tranforms = transform;
    packet.S_CharacterMesh = mesh;
    packet.ObjectTypeFlag = object_flag;
    packet.sync_data = sync_data;

    return OptimizedBinaryPacketSerializer::SerializeCompactCreateMapData(packet);
}

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactMapPacket(int createmapid,
    const std::vector<std::string>& hero_names, const std::vector<std::string>& player_hashes) {
    Map_Packet packet;
    packet.Type = PacketType::CreateMap;
    packet.Createmapid = createmapid;
    packet.PlayerHeroName = hero_names;
    packet.PlayerHash = player_hashes;

    return OptimizedBinaryPacketSerializer::SerializeCompactMapPacket(packet);
}

// ============ 새로운 패킷 생성 함수들 (mapid 첫 번째 필드) ============

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactIntPacket(PacketType type, int mapid,
    const std::string& object_hash, int value) {
    int_packet packet;
    packet.mapid = mapid;  // mapid가 첫 번째 필드
    packet.Type = type;
    packet.ObjectHash = object_hash;
    packet.value = value;

    return OptimizedBinaryPacketSerializer::SerializeCompactIntPacket(packet);
}

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactFloatPacket(PacketType type, int mapid,
    const std::string& object_hash, float value) {
    float_packet packet;
    packet.mapid = mapid;  // mapid가 첫 번째 필드
    packet.Type = type;
    packet.ObjectHash = object_hash;
    packet.value = value;

    return OptimizedBinaryPacketSerializer::SerializeCompactFloatPacket(packet);
}

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactUInt8Packet(PacketType type, int mapid,
    const std::string& object_hash, uint8_t value) {
    uint8_t_packet packet;
    packet.mapid = mapid;  // mapid가 첫 번째 필드
    packet.Type = type;
    packet.ObjectHash = object_hash;
    packet.value = value;

    return OptimizedBinaryPacketSerializer::SerializeCompactUInt8Packet(packet);
}

std::vector<uint8_t> OptimizedPacketHandler::CreateCompactVec3Packet(PacketType type, int mapid,
    const std::string& object_hash, const Vec3& value) {
    Vec3_packet packet;
    packet.mapid = mapid;  // mapid가 첫 번째 필드
    packet.Type = type;
    packet.ObjectHash = object_hash;
    packet.value = value;

    return OptimizedBinaryPacketSerializer::SerializeCompactVec3Packet(packet);
}

// ============ 패킷 처리 함수 구현 ============

bool OptimizedPacketHandler::ProcessSecurePacket(const std::vector<uint8_t>& data) {
    PacketType packet_type;
    CompactBinaryReader* reader = nullptr;

    if (!OptimizedBinaryPacketSerializer::ParseSecurePacket(data, packet_type, reader)) {
        return false;
    }

    std::unique_ptr<CompactBinaryReader, void(*)(CompactBinaryReader*)> reader_guard(
        reader, [](CompactBinaryReader* r) {
            OptimizedBinaryPacketSerializer::ReleaseReader(r);
        });

    switch (packet_type) {
    case PacketType::Input: {
        auto move_packet = OptimizedBinaryPacketSerializer::DeserializeCompactObjectMoveOrderType(*reader);
        ProcessMovePacket(move_packet);
        break;
    }

    case PacketType::CreateCastSpell: {
        auto cast_packet = OptimizedBinaryPacketSerializer::DeserializeCompactObjectCastOrderType(*reader);
        ProcessCastSpellPacket(cast_packet);
        break;
    }

    case PacketType::CreateMap: {
        auto create_map_packet = OptimizedBinaryPacketSerializer::DeserializeCompactMapPacket(*reader);
        ProcessMakeMapPacket(create_map_packet);
        break;
    }

    case PacketType::UpdateCastSpell: {
        auto update_spell = OptimizedBinaryPacketSerializer::DeserializeCompactUpdateCastSpell(*reader);
        // 필요시 처리 구현
        break;
    }

    case PacketType::DeleteCastSpell: {
        auto delete_spell = OptimizedBinaryPacketSerializer::DeserializeCompactDeleteCastSpell(*reader);
        // 필요시 처리 구현
        break;
    }

    default:
        break;
    }

    return true;
}

// ============ 패킷별 처리 함수들 구현 ============

void OptimizedPacketHandler::ProcessMovePacket(const ObjectMoveOrderType& packet) {
    // mapid를 직접 사용 가능 (첫 번째 필드)
    auto ThisMap = G_MapSchedule->Get_MapList(packet.mapid);
    ThisMap->PushTask<ObjectMoveOrderType>(packet);
}

void OptimizedPacketHandler::ProcessCastSpellPacket(const ObjectCastOrderType& packet) {
    // mapid를 직접 사용 가능 (첫 번째 필드)
    auto ThisMap = G_MapSchedule->Get_MapList(packet.mapid);
    ThisMap->PushTask<ObjectCastOrderType>(packet);
}

void OptimizedPacketHandler::ProcessMakeMapPacket(const Map_Packet& packet) {
    G_MapSchedule->Make(packet.Createmapid, packet.PlayerHeroName, packet.PlayerHash);
}