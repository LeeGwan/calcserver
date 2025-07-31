#pragma once

#include <vector>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include<shared_mutex>

// 전방선언들
enum class PacketType : uint8_t;
enum class SpellType : unsigned int;
namespace Enums { enum CurrentOrderType : uint8_t; }
struct result_data;
struct user_infor_data;
struct Struct_Transform;
struct Struct_CharacterMesh;
struct Struct_Sync_data;
struct Create_Map_data;
struct Map_Packet;
struct MoveOrder;
struct CastSpellINFOR;
struct InitCastSpell;
struct UpdateCasteSPell;
struct DeleteCasteSPell;
struct ObjectMoveOrderType;
struct ObjectCastOrderType;
struct int_packet;
struct float_packet;
struct uint8_t_packet;
struct Vec3_packet;
class Vec3;
class CompactBinaryReader;

class OptimizedBinaryPacketSerializer {
public:
    static const uint16_t MAGIC_NUMBER = 0x2340;
    static const uint8_t VERSION = 1;

    struct CompactPacketHeader {
        uint16_t magic;
        uint8_t version;
        uint8_t type;
        uint32_t data_size;
    };

    struct SecurePacketHeader {
        uint16_t magic;
        uint8_t version;
        uint8_t type;
        uint32_t data_size;
        uint32_t xxhash;
    };

    static std::vector<uint8_t> WrapPacketWithXXHash(uint8_t type, const std::vector<uint8_t>& data);
    static std::vector<uint8_t> WrapPacket(uint8_t type, const std::vector<uint8_t>& data);

public:
    OptimizedBinaryPacketSerializer();
    ~OptimizedBinaryPacketSerializer();

    OptimizedBinaryPacketSerializer(const OptimizedBinaryPacketSerializer&) = delete;
    OptimizedBinaryPacketSerializer& operator=(const OptimizedBinaryPacketSerializer&) = delete;

    // 기본 직렬화 함수들
    static void SerializeCompactVec3(std::vector<uint8_t>& buffer, const Vec3& vec);
    static void SerializeDirectString(std::vector<uint8_t>& buffer, const std::string& str);
    static void SerializeUInt8(std::vector<uint8_t>& buffer, uint8_t value);
    static void SerializeInt16(std::vector<uint8_t>& buffer, int16_t value);
    static void SerializeUInt16(std::vector<uint8_t>& buffer, uint16_t value);
    static void SerializeUInt32(std::vector<uint8_t>& buffer, uint32_t value);
    static void SerializeInt32(std::vector<uint8_t>& buffer, int32_t value);
    static void SerializeCompactFloat(std::vector<uint8_t>& buffer, float value, float precision = 0.01f);
    static void SerializeBitFlags(std::vector<uint8_t>& buffer, uint8_t flags);

    // Struct_Sync_data 직렬화/역직렬화 함수
    static void SerializeCompactSyncData(std::vector<uint8_t>& buffer, const Struct_Sync_data& sync_data);
    static Struct_Sync_data DeserializeCompactSyncData(CompactBinaryReader& reader);

    // ============ 새로운 구조체 직렬화 함수들 ============
    static std::vector<uint8_t> SerializeCompactMoveOrder(const MoveOrder& data);
    static std::vector<uint8_t> SerializeCompactCastSpellINFOR(const CastSpellINFOR& data);
    static std::vector<uint8_t> SerializeCompactObjectMoveOrderType(const ObjectMoveOrderType& data);
    static std::vector<uint8_t> SerializeCompactObjectCastOrderType(const ObjectCastOrderType& data);
    static std::vector<uint8_t> SerializeCompactInitCastSpell(const InitCastSpell& data);
    static std::vector<uint8_t> SerializeCompactUpdateCastSpell(const UpdateCasteSPell& data);
    static std::vector<uint8_t> SerializeCompactDeleteCastSpell(const DeleteCasteSPell& data);
    static std::vector<uint8_t> SerializeCompactUserInfo(const user_infor_data& data);
    static std::vector<uint8_t> SerializeCompactCreateMapData(const Create_Map_data& data);
    static std::vector<uint8_t> SerializeCompactMapPacket(const Map_Packet& data);
    static std::vector<uint8_t> SerializeCompactIntPacket(const int_packet& data);
    static std::vector<uint8_t> SerializeCompactFloatPacket(const float_packet& data);
    static std::vector<uint8_t> SerializeCompactUInt8Packet(const uint8_t_packet& data);
    static std::vector<uint8_t> SerializeCompactVec3Packet(const Vec3_packet& data);

    // 패킷 파싱
    static bool ParseSecurePacket(const std::vector<uint8_t>& data, PacketType& out_type, CompactBinaryReader*& out_reader);
    static void ReleaseReader(CompactBinaryReader* reader);

    // ============ 새로운 구조체 역직렬화 함수들 ============
    static ObjectMoveOrderType DeserializeCompactObjectMoveOrderType(CompactBinaryReader& reader);
    static ObjectCastOrderType DeserializeCompactObjectCastOrderType(CompactBinaryReader& reader);
    static InitCastSpell DeserializeCompactInitCastSpell(CompactBinaryReader& reader);
    static UpdateCasteSPell DeserializeCompactUpdateCastSpell(CompactBinaryReader& reader);
    static DeleteCasteSPell DeserializeCompactDeleteCastSpell(CompactBinaryReader& reader);
    static user_infor_data DeserializeCompactUserInfo(CompactBinaryReader& reader);
    static Create_Map_data DeserializeCompactCreateMapData(CompactBinaryReader& reader);
    static Map_Packet DeserializeCompactMapPacket(CompactBinaryReader& reader);
    static int_packet DeserializeCompactIntPacket(CompactBinaryReader& reader);
    static float_packet DeserializeCompactFloatPacket(CompactBinaryReader& reader);
    static uint8_t_packet DeserializeCompactUInt8Packet(CompactBinaryReader& reader);
    static Vec3_packet DeserializeCompactVec3Packet(CompactBinaryReader& reader);
};