#pragma once
#include <cstdint>
#include <string>
#include<vector>
#include "../../../Core/utils/Vector.h"
#include "../../../Core/Object/Object/Flags.h"
#include "../../../Core/Object/Object/Spells/SpellFlags/SpellFlags.h"

enum class PacketType : uint8_t
{
	ErrorType = 0,

	// ===== Ŭ���̾�Ʈ-����� ���� ��Ŷ (1-20) =====
	

	// ===== �ΰ���-�����-���Ӱ�꼭�� ��Ŷ (21-50) =====
	CreateMap = 21,
	JoinMap = 22,
	FinishMap = 23,
	Input = 24,
	CreateCastSpell = 25,
	UpdateCastSpell = 26,
	DeleteCastSpell = 27,
	SyncPacket = 28,

	// ��ȯ ����
	TransformPosition = 29,
	TransformRotation = 30,

	// ������Ʈ ����
	ObjectStatsAttackRange = 31,
	ObjectStatsHealth = 32,
	ObjectStatsMaxHealth = 33,
	ObjectStatsMana = 34,
	ObjectStatsMaxMana = 35,

	// ������Ʈ ����
	ObjectStateLevel = 36,
	ObjectStateXP = 37,
	ObjectStateMaxXP = 38,
	ObjectStateDeadTime = 39,
	ObjectStateObjectState = 40,
	ObjectStateCurrentOrderType = 41,

	// ���� ����
	CombatStatsTotalArmor = 42,
	CombatStatsTotalMagicRes = 43,
	CombatStatsTotalAD = 44,
	CombatStatsTotalAP = 45,

	// ��ٿ�
	CooldownTimeQ = 46,
	CooldownTimeW = 47,
	CooldownTimeE = 48,
	CooldownTimeR = 49
};
struct result_data
{
	std::string result = "";//�ߺ��̳� �α��� ȸ������ ����
	std::string hash = "";

};
struct user_infor_data
{
	PacketType Type;
	std::string id = "";
	std::string pw = "";
	result_data result;


};

struct Struct_Transform
{
	Vec3 Position;
	Vec3 Rotation;
	Vec3 Scale;

};

struct Struct_CharacterMesh
{
	Vec3 CapsuleSIze;
	Vec3 CapsuleLocation;
	Vec3 Mesh_Location;
	Vec3 Mesh_Rotation;
	Vec3 Mesh_Scale;

};
struct Struct_Sync_data
{
	
	float attack_range;
	float health;
	float maxhealth;
	float mana;
	float maxmana;
	float xp;
	float maxXp;
	float DeadTime;
	float total_armor;
	float total_magic_armor;
	float total_AD;
	float total_AP;
	int Level;
	std::vector<float> CoolDownTime;
	uint8_t CurrentOrderType;
	uint8_t ObjectState;
	Struct_Sync_data()
	{
		this->attack_range = 0.0f;
		this->health = 0.0f;
		this->maxhealth = 0.0f;
		this->mana = 0.0f;
		this->maxmana = 0.0f;
		this->xp = 0.0f;
		this->maxXp = 0.0f;
		this->DeadTime = 0.0f;
		this->total_armor = 0.0f;
		this->total_magic_armor = 0.0f;
		this->total_AD = 0.0f;
		this->total_AP=0.0f;
		this->Level = 0;

		this->CurrentOrderType = 0;
		this->ObjectState=0;
	}
};

struct Create_Map_data//����->���� �ʱⰪ ����
{
	int mapid;
	PacketType Type;
	std::string Name;
	std::string Hash;
	Struct_Transform S_Tranforms;
	Struct_CharacterMesh S_CharacterMesh;
	unsigned long ObjectTypeFlag;
	Struct_Sync_data sync_data;
};
struct Map_Packet
{
	int Createmapid;
	PacketType Type;
	std::vector<std::string> PlayerHeroName;
	std::vector<std::string> PlayerHash;
};
struct MoveOrder
{
	std::string ObjectHash;
	Enums::CurrentOrderType CurrentOrderType;
	Vec3 EndPos;
	std::string TargetObjectHash;
};

struct CastSpellINFOR
{
	
	std::string ObjectHash;
	Enums::CurrentOrderType CurrentOrderType;
	Vec3 EndPos;
	std::string TargetObjectHash;

};
struct InitCastSpell
{
	int mapid;
	PacketType Type;
	unsigned int Skillid;
	Vec3 startPos;
	Vec3 endPos;
	float radius;
	SpellType Spell_Type;

};
struct UpdateCasteSPell
{
	int mapid;
	PacketType Type;

	unsigned int Skillid;
	Vec3 startPos;
};
struct DeleteCasteSPell
{
	int mapid;
	PacketType Type;
	unsigned int Skillid;

};
struct ObjectMoveOrderType
{
	int mapid;
	PacketType Type;
	MoveOrder Move_INFOR;
};
struct ObjectCastOrderType
{
	int mapid;
	PacketType Type;
	CastSpellINFOR CastSpell_INFOR;
};
struct int_packet
{
	int mapid;
	PacketType Type;
	std::string ObjectHash;
	int value;
};
struct float_packet
{
	int mapid;
	PacketType Type;
	std::string ObjectHash;
	float value;
};
struct uint8_t_packet
{
	int mapid;
	PacketType Type;
	std::string ObjectHash;
	
	uint8_t value;
};
struct Vec3_packet
{
	int mapid;
	PacketType Type;
	std::string ObjectHash;
	Vec3 value;
};
