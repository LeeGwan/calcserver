#pragma once
#include <string>
#include <functional>
#include <unordered_map>
enum class PacketType : uint8_t;
class Vec3;
class Object;
class SkillAbility;
class OutputComponent
{
public:
    // 객체 초기화 데이터 전송
    void SendInitData(int map_id, Object* object, PacketType packet_type);
    void ProcessSpellInit(int map_id, SkillAbility* skillobject);
    // 객체 상태 변화 감지 및 전송
    void ProcessObjectUpdates(int map_id, Object* object);
    void ProcessSpellUpdates(int map_id, unsigned int skillid, const Vec3& startPos);
    void DeleteSpell(int map_id, unsigned int Skillid);
private:
    // 템플릿을 사용한 통합 패킷 전송 함수
    template<typename PacketT, typename ValueT>
    void SendPacket(int map_id, const std::string& object_hash, PacketType packet_type, const ValueT& value);

    // 플래그 체크 및 패킷 전송을 위한 헬퍼 템플릿
    template<typename ComponentT, typename FlagT, typename PacketT, typename ValueT>
    void CheckAndSendUpdate(ComponentT& component, FlagT flag, int map_id,
        const std::string& object_hash, PacketType packet_type,
        const ValueT& value);

    // 각 컴포넌트별 업데이트 처리 함수들
    void ProcessTransformUpdates(int map_id, Object* object);
    void ProcessCombatStatsUpdates(int map_id, Object* object);
    void ProcessObjectStatsUpdates(int map_id, Object* object);
    void ProcessObjectStateUpdates(int map_id, Object* object);
    void ProcessSpellBookStateUpdates(int map_id, Object* object);
};