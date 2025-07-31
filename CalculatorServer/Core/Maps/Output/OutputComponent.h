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
    // ��ü �ʱ�ȭ ������ ����
    void SendInitData(int map_id, Object* object, PacketType packet_type);
    void ProcessSpellInit(int map_id, SkillAbility* skillobject);
    // ��ü ���� ��ȭ ���� �� ����
    void ProcessObjectUpdates(int map_id, Object* object);
    void ProcessSpellUpdates(int map_id, unsigned int skillid, const Vec3& startPos);
    void DeleteSpell(int map_id, unsigned int Skillid);
private:
    // ���ø��� ����� ���� ��Ŷ ���� �Լ�
    template<typename PacketT, typename ValueT>
    void SendPacket(int map_id, const std::string& object_hash, PacketType packet_type, const ValueT& value);

    // �÷��� üũ �� ��Ŷ ������ ���� ���� ���ø�
    template<typename ComponentT, typename FlagT, typename PacketT, typename ValueT>
    void CheckAndSendUpdate(ComponentT& component, FlagT flag, int map_id,
        const std::string& object_hash, PacketType packet_type,
        const ValueT& value);

    // �� ������Ʈ�� ������Ʈ ó�� �Լ���
    void ProcessTransformUpdates(int map_id, Object* object);
    void ProcessCombatStatsUpdates(int map_id, Object* object);
    void ProcessObjectStatsUpdates(int map_id, Object* object);
    void ProcessObjectStateUpdates(int map_id, Object* object);
    void ProcessSpellBookStateUpdates(int map_id, Object* object);
};