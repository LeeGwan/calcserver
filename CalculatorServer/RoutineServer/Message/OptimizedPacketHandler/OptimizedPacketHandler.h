#pragma once
#include <vector>
#include <string>
#include <stdint.h>

// 전방선언들
enum class SpellType : unsigned int;
enum class PacketType : uint8_t;
struct Struct_Transform;
struct Struct_CharacterMesh;
struct Struct_Sync_data;
struct ObjectMoveOrderType;
struct ObjectCastOrderType;
struct InitCastSpell;
struct UpdateCasteSPell;
struct DeleteCasteSPell;
struct Create_Map_data;
struct Map_Packet;
struct int_packet;
struct float_packet;
struct uint8_t_packet;
struct Vec3_packet;
struct user_infor_data;
class Vec3;

class OptimizedPacketHandler {
public:
    // ============ 패킷 생성 함수들 ============

    // 움직임 패킷 생성 (mapid가 첫 번째 필드)
    static std::vector<uint8_t> CreateCompactMovePacket(int mapid, const std::string& object_hash,
        const std::string& target_hash, const Vec3& endPos, uint8_t order_type);

    // 스킬 시전 패킷 생성 (mapid가 첫 번째 필드)
    static std::vector<uint8_t> CreateCompactCastSpellPacket(int mapid, const std::string& object_hash,
        const std::string& target_hash, const Vec3& endPos, uint8_t order_type);

    // 스킬 초기화 패킷 생성 (mapid가 첫 번째 필드)
    static std::vector<uint8_t> CreateCompactInitCastSpellPacket(int mapid, uint32_t skillid,
        const Vec3& start, const Vec3& end, float radius, SpellType spell_type);

    // 스킬 업데이트 패킷 생성 (mapid가 첫 번째 필드)
    static std::vector<uint8_t> CreateCompactUpdateCastSpellPacket(int mapid, uint32_t skillid, const Vec3& start);

    // 스킬 삭제 패킷 생성 (mapid가 첫 번째 필드)
    static std::vector<uint8_t> CreateCompactDeleteCastSpellPacket(int mapid, uint32_t skillid);

    // 맵 생성 데이터 패킷 생성
    static std::vector<uint8_t> CreateCompactCreateMapDataPacket(int mapid, const std::string& name,
        const std::string& hash, const Struct_Transform& transform,
        const Struct_CharacterMesh& mesh, unsigned long object_flag, const Struct_Sync_data& sync_data);

    // 맵 정보 패킷 생성
    static std::vector<uint8_t> CreateCompactMapPacket(int createmapid,
        const std::vector<std::string>& hero_names, const std::vector<std::string>& player_hashes);

    // 정수 값 패킷 생성 (mapid가 첫 번째 필드)
    static std::vector<uint8_t> CreateCompactIntPacket(PacketType type, int mapid,
        const std::string& object_hash, int value);

    // 실수 값 패킷 생성 (mapid가 첫 번째 필드)
    static std::vector<uint8_t> CreateCompactFloatPacket(PacketType type, int mapid,
        const std::string& object_hash, float value);

    // uint8_t 값 패킷 생성 (mapid가 첫 번째 필드)
    static std::vector<uint8_t> CreateCompactUInt8Packet(PacketType type, int mapid,
        const std::string& object_hash, uint8_t value);

    // Vec3 값 패킷 생성 (mapid가 첫 번째 필드)
    static std::vector<uint8_t> CreateCompactVec3Packet(PacketType type, int mapid,
        const std::string& object_hash, const Vec3& value);

    // ============ 패킷 처리 함수들 ============

    // 보안 패킷 처리 (xxHash 검증 포함)
    static bool ProcessSecurePacket(const std::vector<uint8_t>& data);

    // ============ 패킷별 처리 함수들 ============
    static void ProcessMovePacket(const ObjectMoveOrderType& packet);
    static void ProcessCastSpellPacket(const ObjectCastOrderType& packet);
    static void ProcessMakeMapPacket(const Map_Packet& packet);
};