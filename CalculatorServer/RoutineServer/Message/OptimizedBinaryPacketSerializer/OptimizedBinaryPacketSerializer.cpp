#include "OptimizedBinaryPacketSerializer.h"
#include "../Hash32/XXHash32.h"
#include "../../../Core/utils/Vector.h"
#include "../CompactBinaryReader/CompactBinaryReader.h"
#include "../PacketStructure/PacketStructure.h"

OptimizedBinaryPacketSerializer::OptimizedBinaryPacketSerializer() = default;
OptimizedBinaryPacketSerializer::~OptimizedBinaryPacketSerializer() = default;

// ============ 기본 직렬화 함수들 ============
void OptimizedBinaryPacketSerializer::SerializeCompactVec3(std::vector<uint8_t>& buffer, const Vec3& vec) {
    int32_t x = static_cast<int32_t>(vec.x * 100.0f);
    int32_t y = static_cast<int32_t>(vec.y * 100.0f);
    int32_t z = static_cast<int32_t>(vec.z * 100.0f);

    SerializeInt32(buffer, x);
    SerializeInt32(buffer, y);
    SerializeInt32(buffer, z);
}

void OptimizedBinaryPacketSerializer::SerializeDirectString(std::vector<uint8_t>& buffer, const std::string& str) {
    SerializeUInt16(buffer, static_cast<uint16_t>(str.length()));
    for (char c : str) {
        buffer.push_back(static_cast<uint8_t>(c));
    }
}

void OptimizedBinaryPacketSerializer::SerializeUInt8(std::vector<uint8_t>& buffer, uint8_t value) {
    buffer.push_back(value);
}

void OptimizedBinaryPacketSerializer::SerializeInt16(std::vector<uint8_t>& buffer, int16_t value) {
    buffer.push_back(value & 0xFF);
    buffer.push_back((value >> 8) & 0xFF);
}

void OptimizedBinaryPacketSerializer::SerializeUInt16(std::vector<uint8_t>& buffer, uint16_t value) {
    buffer.push_back(value & 0xFF);
    buffer.push_back((value >> 8) & 0xFF);
}

void OptimizedBinaryPacketSerializer::SerializeUInt32(std::vector<uint8_t>& buffer, uint32_t value) {
    buffer.push_back(value & 0xFF);
    buffer.push_back((value >> 8) & 0xFF);
    buffer.push_back((value >> 16) & 0xFF);
    buffer.push_back((value >> 24) & 0xFF);
}

void OptimizedBinaryPacketSerializer::SerializeInt32(std::vector<uint8_t>& buffer, int32_t value) {
    SerializeUInt32(buffer, static_cast<uint32_t>(value));
}

void OptimizedBinaryPacketSerializer::SerializeCompactFloat(std::vector<uint8_t>& buffer, float value, float precision) {
    uint32_t compact_value = static_cast<uint32_t>(value / precision);
    SerializeInt32(buffer, compact_value);
}

void OptimizedBinaryPacketSerializer::SerializeBitFlags(std::vector<uint8_t>& buffer, uint8_t flags) {
    buffer.push_back(flags);
}

// ============ Struct_Sync_data 직렬화/역직렬화 ============
void OptimizedBinaryPacketSerializer::SerializeCompactSyncData(std::vector<uint8_t>& buffer, const Struct_Sync_data& sync_data) {
    SerializeCompactFloat(buffer, sync_data.attack_range, 0.01f);
    SerializeCompactFloat(buffer, sync_data.health, 0.01f);
    SerializeCompactFloat(buffer, sync_data.maxhealth, 0.01f);
    SerializeCompactFloat(buffer, sync_data.mana, 0.01f);
    SerializeCompactFloat(buffer, sync_data.maxmana, 0.01f);
    SerializeCompactFloat(buffer, sync_data.xp, 0.01f);
    SerializeCompactFloat(buffer, sync_data.maxXp, 0.01f);
    SerializeCompactFloat(buffer, sync_data.DeadTime, 0.01f);
    SerializeCompactFloat(buffer, sync_data.total_armor, 0.01f);
    SerializeCompactFloat(buffer, sync_data.total_magic_armor, 0.01f);
    SerializeCompactFloat(buffer, sync_data.total_AD, 0.01f);
    SerializeCompactFloat(buffer, sync_data.total_AP, 0.01f);

    SerializeInt32(buffer, sync_data.Level);

    SerializeUInt8(buffer, static_cast<uint8_t>(sync_data.CoolDownTime.size()));
    for (const auto& cooldown : sync_data.CoolDownTime) {
        SerializeCompactFloat(buffer, cooldown, 0.01f);
    }

    SerializeUInt8(buffer, sync_data.CurrentOrderType);
    SerializeUInt8(buffer, sync_data.ObjectState);
}

Struct_Sync_data OptimizedBinaryPacketSerializer::DeserializeCompactSyncData(CompactBinaryReader& reader) {
    Struct_Sync_data sync_data;

    sync_data.attack_range = reader.ReadCompactFloat(0.01f);
    sync_data.health = reader.ReadCompactFloat(0.01f);
    sync_data.maxhealth = reader.ReadCompactFloat(0.01f);
    sync_data.mana = reader.ReadCompactFloat(0.01f);
    sync_data.maxmana = reader.ReadCompactFloat(0.01f);
    sync_data.xp = reader.ReadCompactFloat(0.01f);
    sync_data.maxXp = reader.ReadCompactFloat(0.01f);
    sync_data.DeadTime = reader.ReadCompactFloat(0.01f);
    sync_data.total_armor = reader.ReadCompactFloat(0.01f);
    sync_data.total_magic_armor = reader.ReadCompactFloat(0.01f);
    sync_data.total_AD = reader.ReadCompactFloat(0.01f);
    sync_data.total_AP = reader.ReadCompactFloat(0.01f);

    sync_data.Level = reader.ReadInt32();

    uint8_t cooldown_count = reader.ReadUInt8();
    sync_data.CoolDownTime.reserve(cooldown_count);
    for (uint8_t i = 0; i < cooldown_count; ++i) {
        sync_data.CoolDownTime.push_back(reader.ReadCompactFloat(0.01f));
    }

    sync_data.CurrentOrderType = reader.ReadUInt8();
    sync_data.ObjectState = reader.ReadUInt8();

    return sync_data;
}

// ============ 패킷 래핑 함수들 ============
std::vector<uint8_t> OptimizedBinaryPacketSerializer::WrapPacketWithXXHash(uint8_t type, const std::vector<uint8_t>& data) {
    SecurePacketHeader header;
    header.magic = MAGIC_NUMBER;
    header.version = VERSION;
    header.type = type;
    header.data_size = static_cast<uint32_t>(data.size());
    header.xxhash = XXHash32::hash(data.data(), data.size());

    std::vector<uint8_t> result;
    result.resize(sizeof(SecurePacketHeader) + data.size());

    std::memcpy(result.data(), &header, sizeof(SecurePacketHeader));
    std::memcpy(result.data() + sizeof(SecurePacketHeader), data.data(), data.size());

    return result;
}

std::vector<uint8_t> OptimizedBinaryPacketSerializer::WrapPacket(uint8_t type, const std::vector<uint8_t>& data) {
    CompactPacketHeader header;
    header.magic = MAGIC_NUMBER;
    header.version = VERSION;
    header.type = type;
    header.data_size = static_cast<uint32_t>(data.size());

    std::vector<uint8_t> result;
    result.resize(sizeof(CompactPacketHeader) + data.size());

    std::memcpy(result.data(), &header, sizeof(CompactPacketHeader));
    std::memcpy(result.data() + sizeof(CompactPacketHeader), data.data(), data.size());

    return result;
}

// ============ 새로운 구조체 직렬화 구현 ============

// ObjectMoveOrderType 직렬화 (mapid가 첫 번째)
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactObjectMoveOrderType(const ObjectMoveOrderType& data) {
    std::vector<uint8_t> buffer;

    // mapid가 첫 번째 필드
    SerializeInt32(buffer, data.mapid);

    // MoveOrder 구조체 직렬화
    uint8_t flags = 0;
    if (!data.Move_INFOR.ObjectHash.empty()) flags |= 0x01;
    if (!data.Move_INFOR.TargetObjectHash.empty()) flags |= 0x02;
    SerializeBitFlags(buffer, flags);

    if (flags & 0x01) SerializeDirectString(buffer, data.Move_INFOR.ObjectHash);
    SerializeUInt8(buffer, static_cast<uint8_t>(data.Move_INFOR.CurrentOrderType));
    SerializeCompactVec3(buffer, data.Move_INFOR.EndPos);
    if (flags & 0x02) SerializeDirectString(buffer, data.Move_INFOR.TargetObjectHash);

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

// ObjectCastOrderType 직렬화 (mapid가 첫 번째)
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactObjectCastOrderType(const ObjectCastOrderType& data) {
    std::vector<uint8_t> buffer;

    // mapid가 첫 번째 필드
    SerializeInt32(buffer, data.mapid);

    // CastSpellINFOR 구조체 직렬화
    uint8_t flags = 0;
    if (!data.CastSpell_INFOR.ObjectHash.empty()) flags |= 0x01;
    if (!data.CastSpell_INFOR.TargetObjectHash.empty()) flags |= 0x02;
    SerializeBitFlags(buffer, flags);

    if (flags & 0x01) SerializeDirectString(buffer, data.CastSpell_INFOR.ObjectHash);
    SerializeUInt8(buffer, static_cast<uint8_t>(data.CastSpell_INFOR.CurrentOrderType));
    SerializeCompactVec3(buffer, data.CastSpell_INFOR.EndPos);
    if (flags & 0x02) SerializeDirectString(buffer, data.CastSpell_INFOR.TargetObjectHash);

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

// InitCastSpell 직렬화 (mapid가 첫 번째)
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactInitCastSpell(const InitCastSpell& data) {
    std::vector<uint8_t> buffer;

    SerializeInt32(buffer, data.mapid);  // mapid가 첫 번째
    SerializeUInt32(buffer, data.Skillid);
    SerializeCompactVec3(buffer, data.startPos);
    SerializeCompactVec3(buffer, data.endPos);
    SerializeCompactFloat(buffer, data.radius, 0.1f);
    SerializeUInt8(buffer, static_cast<uint8_t>(data.Spell_Type));

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

// UpdateCasteSPell 직렬화 (mapid가 첫 번째)
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactUpdateCastSpell(const UpdateCasteSPell& data) {
    std::vector<uint8_t> buffer;

    SerializeInt32(buffer, data.mapid);  // mapid가 첫 번째
    SerializeUInt32(buffer, data.Skillid);
    SerializeCompactVec3(buffer, data.startPos);

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

// DeleteCasteSPell 직렬화 (mapid가 첫 번째)
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactDeleteCastSpell(const DeleteCasteSPell& data) {
    std::vector<uint8_t> buffer;

    SerializeInt32(buffer, data.mapid);  // mapid가 첫 번째
    SerializeUInt32(buffer, data.Skillid);

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

// int_packet 직렬화 (mapid가 첫 번째)
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactIntPacket(const int_packet& data) {
    std::vector<uint8_t> buffer;

    SerializeInt32(buffer, data.mapid);  // mapid가 첫 번째
    SerializeDirectString(buffer, data.ObjectHash);
    SerializeInt32(buffer, data.value);

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

// float_packet 직렬화 (mapid가 첫 번째)
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactFloatPacket(const float_packet& data) {
    std::vector<uint8_t> buffer;

    SerializeInt32(buffer, data.mapid);  // mapid가 첫 번째
    SerializeDirectString(buffer, data.ObjectHash);
    SerializeCompactFloat(buffer, data.value, 0.01f);

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

// uint8_t_packet 직렬화 (mapid가 첫 번째)
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactUInt8Packet(const uint8_t_packet& data) {
    std::vector<uint8_t> buffer;

    SerializeInt32(buffer, data.mapid);  // mapid가 첫 번째
    SerializeDirectString(buffer, data.ObjectHash);
    SerializeUInt8(buffer, data.value);

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

// Vec3_packet 직렬화 (mapid가 첫 번째)
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactVec3Packet(const Vec3_packet& data) {
    std::vector<uint8_t> buffer;

    SerializeInt32(buffer, data.mapid);  // mapid가 첫 번째
    SerializeDirectString(buffer, data.ObjectHash);
    SerializeCompactVec3(buffer, data.value);

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

// ============ 기존 함수들 (Create_Map_data는 mapid 위치 변경 없음) ============

// Map_Packet 직렬화 (Createmapid가 첫 번째 필드)
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactMapPacket(const Map_Packet& data) {
    std::vector<uint8_t> buffer;

    // Createmapid가 첫 번째 필드 (int32_t로 직렬화)
    SerializeInt32(buffer, data.Createmapid);

    // PlayerHeroName vector 직렬화
    SerializeUInt8(buffer, static_cast<uint8_t>(data.PlayerHeroName.size()));
    for (const auto& name : data.PlayerHeroName) {
        SerializeDirectString(buffer, name);
    }

    // PlayerHash vector 직렬화
    SerializeUInt8(buffer, static_cast<uint8_t>(data.PlayerHash.size()));
    for (const auto& hash : data.PlayerHash) {
        SerializeDirectString(buffer, hash);
    }

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactCreateMapData(const Create_Map_data& data) {
    std::vector<uint8_t> buffer;

    SerializeInt32(buffer, data.mapid);
    SerializeDirectString(buffer, data.Name);
    SerializeDirectString(buffer, data.Hash);

    SerializeCompactVec3(buffer, data.S_Tranforms.Position);
    SerializeCompactVec3(buffer, data.S_Tranforms.Rotation);
    SerializeCompactVec3(buffer, data.S_Tranforms.Scale);

    SerializeCompactVec3(buffer, data.S_CharacterMesh.CapsuleSIze);
    SerializeCompactVec3(buffer, data.S_CharacterMesh.CapsuleLocation);
    SerializeCompactVec3(buffer, data.S_CharacterMesh.Mesh_Location);
    SerializeCompactVec3(buffer, data.S_CharacterMesh.Mesh_Rotation);
    SerializeCompactVec3(buffer, data.S_CharacterMesh.Mesh_Scale);

    SerializeUInt16(buffer, static_cast<uint16_t>(data.ObjectTypeFlag));
    SerializeCompactSyncData(buffer, data.sync_data);

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

// ============ 패킷 파싱 ============
bool OptimizedBinaryPacketSerializer::ParseSecurePacket(const std::vector<uint8_t>& data, PacketType& out_type, CompactBinaryReader*& out_reader) {
    out_reader = nullptr;
    out_type = PacketType::ErrorType;

    if (data.size() < sizeof(SecurePacketHeader)) {
        return false;
    }

    const SecurePacketHeader* header = reinterpret_cast<const SecurePacketHeader*>(data.data());

    if (header->magic != MAGIC_NUMBER || header->version != VERSION) {
        return false;
    }

    if (data.size() < sizeof(SecurePacketHeader) + header->data_size) {
        return false;
    }

    std::vector<uint8_t> payload(data.begin() + sizeof(SecurePacketHeader),
        data.begin() + sizeof(SecurePacketHeader) + header->data_size);
    uint32_t calculated_hash = XXHash32::hash(payload.data(), payload.size());

    if (calculated_hash != header->xxhash) {
        return false;
    }

    out_reader = new CompactBinaryReader(payload);
    out_type = static_cast<PacketType>(header->type);
    return true;
}

void OptimizedBinaryPacketSerializer::ReleaseReader(CompactBinaryReader* reader) {
    delete reader;
}

// ============ 새로운 구조체 역직렬화 구현 ============

// ObjectMoveOrderType 역직렬화 (mapid가 첫 번째)
ObjectMoveOrderType OptimizedBinaryPacketSerializer::DeserializeCompactObjectMoveOrderType(CompactBinaryReader& reader) {
    ObjectMoveOrderType data;

    data.mapid = reader.ReadInt32();  // mapid가 첫 번째

    uint8_t flags = reader.ReadBitFlags();

    if (flags & 0x01) {
        data.Move_INFOR.ObjectHash = reader.ReadDirectString();
    }

    data.Move_INFOR.CurrentOrderType = static_cast<Enums::CurrentOrderType>(reader.ReadUInt8());
    data.Move_INFOR.EndPos = reader.ReadCompactVec3();

    if (flags & 0x02) {
        data.Move_INFOR.TargetObjectHash = reader.ReadDirectString();
    }

    return data;
}

// ObjectCastOrderType 역직렬화 (mapid가 첫 번째)
ObjectCastOrderType OptimizedBinaryPacketSerializer::DeserializeCompactObjectCastOrderType(CompactBinaryReader& reader) {
    ObjectCastOrderType data;

    data.mapid = reader.ReadInt32();  // mapid가 첫 번째

    uint8_t flags = reader.ReadBitFlags();

    if (flags & 0x01) {
        data.CastSpell_INFOR.ObjectHash = reader.ReadDirectString();
    }

    data.CastSpell_INFOR.CurrentOrderType = static_cast<Enums::CurrentOrderType>(reader.ReadUInt8());
    data.CastSpell_INFOR.EndPos = reader.ReadCompactVec3();

    if (flags & 0x02) {
        data.CastSpell_INFOR.TargetObjectHash = reader.ReadDirectString();
    }

    return data;
}

// InitCastSpell 역직렬화 (mapid가 첫 번째)
InitCastSpell OptimizedBinaryPacketSerializer::DeserializeCompactInitCastSpell(CompactBinaryReader& reader) {
    InitCastSpell data;

    data.mapid = reader.ReadInt32();  // mapid가 첫 번째
    data.Skillid = reader.ReadUInt32();
    data.startPos = reader.ReadCompactVec3();
    data.endPos = reader.ReadCompactVec3();
    data.radius = reader.ReadCompactFloat(0.1f);
    data.Spell_Type = static_cast<SpellType>(reader.ReadUInt8());

    return data;
}

// UpdateCasteSPell 역직렬화 (mapid가 첫 번째)
UpdateCasteSPell OptimizedBinaryPacketSerializer::DeserializeCompactUpdateCastSpell(CompactBinaryReader& reader) {
    UpdateCasteSPell data;

    data.mapid = reader.ReadInt32();  // mapid가 첫 번째
    data.Skillid = reader.ReadUInt32();
    data.startPos = reader.ReadCompactVec3();

    return data;
}

// DeleteCasteSPell 역직렬화 (mapid가 첫 번째)
DeleteCasteSPell OptimizedBinaryPacketSerializer::DeserializeCompactDeleteCastSpell(CompactBinaryReader& reader) {
    DeleteCasteSPell data;

    data.mapid = reader.ReadInt32();  // mapid가 첫 번째
    data.Skillid = reader.ReadUInt32();

    return data;
}

// int_packet 역직렬화 (mapid가 첫 번째)
int_packet OptimizedBinaryPacketSerializer::DeserializeCompactIntPacket(CompactBinaryReader& reader) {
    int_packet data;

    data.mapid = reader.ReadInt32();  // mapid가 첫 번째
    data.ObjectHash = reader.ReadDirectString();
    data.value = reader.ReadInt32();

    return data;
}

// float_packet 역직렬화 (mapid가 첫 번째)
float_packet OptimizedBinaryPacketSerializer::DeserializeCompactFloatPacket(CompactBinaryReader& reader) {
    float_packet data;

    data.mapid = reader.ReadInt32();  // mapid가 첫 번째
    data.ObjectHash = reader.ReadDirectString();
    data.value = reader.ReadCompactFloat(0.01f);

    return data;
}

// uint8_t_packet 역직렬화 (mapid가 첫 번째)
uint8_t_packet OptimizedBinaryPacketSerializer::DeserializeCompactUInt8Packet(CompactBinaryReader& reader) {
    uint8_t_packet data;

    data.mapid = reader.ReadInt32();  // mapid가 첫 번째
    data.ObjectHash = reader.ReadDirectString();
    data.value = reader.ReadUInt8();

    return data;
}

// Vec3_packet 역직렬화 (mapid가 첫 번째)
Vec3_packet OptimizedBinaryPacketSerializer::DeserializeCompactVec3Packet(CompactBinaryReader& reader) {
    Vec3_packet data;

    data.mapid = reader.ReadInt32();  // mapid가 첫 번째
    data.ObjectHash = reader.ReadDirectString();
    data.value = reader.ReadCompactVec3();

    return data;
}

// ============ 기존 함수들 (변경 없음) ============
std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactMoveOrder(const MoveOrder& data) {
    std::vector<uint8_t> buffer;

    uint8_t flags = 0;
    if (!data.ObjectHash.empty()) flags |= 0x01;
    if (!data.TargetObjectHash.empty()) flags |= 0x02;
    SerializeBitFlags(buffer, flags);

    if (flags & 0x01) SerializeDirectString(buffer, data.ObjectHash);
    SerializeUInt8(buffer, static_cast<uint8_t>(data.CurrentOrderType));
    SerializeCompactVec3(buffer, data.EndPos);
    if (flags & 0x02) SerializeDirectString(buffer, data.TargetObjectHash);

    return buffer;
}

std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactCastSpellINFOR(const CastSpellINFOR& data) {
    std::vector<uint8_t> buffer;

    uint8_t flags = 0;
    if (!data.ObjectHash.empty()) flags |= 0x01;
    if (!data.TargetObjectHash.empty()) flags |= 0x02;
    SerializeBitFlags(buffer, flags);

    if (flags & 0x01) SerializeDirectString(buffer, data.ObjectHash);
    SerializeUInt8(buffer, static_cast<uint8_t>(data.CurrentOrderType));
    SerializeCompactVec3(buffer, data.EndPos);
    if (flags & 0x02) SerializeDirectString(buffer, data.TargetObjectHash);

    return buffer;
}

std::vector<uint8_t> OptimizedBinaryPacketSerializer::SerializeCompactUserInfo(const user_infor_data& data) {
    std::vector<uint8_t> buffer;

    uint8_t flags = 0;
    if (!data.id.empty()) flags |= 0x01;
    if (!data.pw.empty()) flags |= 0x02;
    if (!data.result.result.empty()) flags |= 0x04;
    if (!data.result.hash.empty()) flags |= 0x08;
    SerializeBitFlags(buffer, flags);

    if (flags & 0x01) SerializeDirectString(buffer, data.id);
    if (flags & 0x02) SerializeDirectString(buffer, data.pw);
    if (flags & 0x04) SerializeDirectString(buffer, data.result.result);
    if (flags & 0x08) SerializeDirectString(buffer, data.result.hash);

    return WrapPacketWithXXHash(static_cast<uint8_t>(data.Type), buffer);
}

user_infor_data OptimizedBinaryPacketSerializer::DeserializeCompactUserInfo(CompactBinaryReader& reader) {
    user_infor_data data;

    uint8_t flags = reader.ReadBitFlags();

    if (flags & 0x01) data.id = reader.ReadDirectString();
    if (flags & 0x02) data.pw = reader.ReadDirectString();
    if (flags & 0x04) data.result.result = reader.ReadDirectString();
    if (flags & 0x08) data.result.hash = reader.ReadDirectString();

    return data;
}

Create_Map_data OptimizedBinaryPacketSerializer::DeserializeCompactCreateMapData(CompactBinaryReader& reader) {
    Create_Map_data data;

    data.mapid = reader.ReadInt32();
    data.Name = reader.ReadDirectString();
    data.Hash = reader.ReadDirectString();

    data.S_Tranforms.Position = reader.ReadCompactVec3();
    data.S_Tranforms.Rotation = reader.ReadCompactVec3();
    data.S_Tranforms.Scale = reader.ReadCompactVec3();

    data.S_CharacterMesh.CapsuleSIze = reader.ReadCompactVec3();
    data.S_CharacterMesh.CapsuleLocation = reader.ReadCompactVec3();
    data.S_CharacterMesh.Mesh_Location = reader.ReadCompactVec3();
    data.S_CharacterMesh.Mesh_Rotation = reader.ReadCompactVec3();
    data.S_CharacterMesh.Mesh_Scale = reader.ReadCompactVec3();

    data.ObjectTypeFlag = reader.ReadUInt16();
    data.sync_data = DeserializeCompactSyncData(reader);

    return data;
}

Map_Packet OptimizedBinaryPacketSerializer::DeserializeCompactMapPacket(CompactBinaryReader& reader) {
    Map_Packet data;

    // Createmapid가 첫 번째 필드 (int32_t로 읽기)
    data.Createmapid = reader.ReadInt32();

    // Type은 패킷 헤더에서 이미 확인했으므로 설정
    data.Type = PacketType::CreateMap;

    uint8_t hero_count = reader.ReadUInt8();
    data.PlayerHeroName.reserve(hero_count);
    for (uint8_t i = 0; i < hero_count; ++i) {
        data.PlayerHeroName.push_back(reader.ReadDirectString());
    }

    uint8_t hash_count = reader.ReadUInt8();
    data.PlayerHash.reserve(hash_count);
    for (uint8_t i = 0; i < hash_count; ++i) {
        data.PlayerHash.push_back(reader.ReadDirectString());
    }

    return data;
}