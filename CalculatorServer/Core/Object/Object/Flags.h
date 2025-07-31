#pragma once
#include <type_traits>
#include "../../utils/variableUtils.h"
#include "Spells/SpellFlags/SpellFlags.h"
namespace Enums
{

	enum class RecallState : uint8_t
	{
		Unknown = 0,
		Recall = 6,
		EmpoweredRecall = 11,
		Teleport = 16,
	};

	enum class ObjectTypeFlag : unsigned long
	{
		Object = (1 << 0),			//0x1
		AI=(1 << 1),				//0x2
		Hero = (1 << 2),			//0x4
		Minion= (1<<3),				//0x8
		Melee_Minion=(1<<4),		//0x10
		Ranged_Minion = (1 << 5),	//0x20
		AttackableUnit = (1 << 6),	//0x40
		Turret = (1 << 7),			//0x80
		Nexus = (1 << 8),			//0x160
		Spell_Missile = (1 << 9),	//0x180
		Spell_Circle = (1 << 10),	//0x200
		Spell_Custom = (1 << 11),	//0x220
		Attack = (1 << 12),
	};


	
	enum CurrentOrderType : uint8_t
	{
		None = 0x0,
		Idle = 0x1,
		Stop = 0x2,
		MoveTo = 0x3,
		AttackTo = 0x4,//사용안함
		AttackMove = 0x5,
		Attack=0x6,
		Spell_Q = 0x7,
		Spell_W = 0x8,
		Spell_E = 0x9,
		Spell_R = 0xA,
		Spell_D = 0xB,
		Spell_F = 0xC,
		ItemCastspell = 0xD,
		Dead

	};

	enum GameObjectActionState
	{
		CanAttack = 1 << 0,
	//	CanCrit = 1 << 1,
		CanSpell = 1 << 2,
		CanMove = 1 << 3,
		Immovable = 1 << 4,
		Candash = 1 << 5,
		Feared = 1 << 8,
		Fleeing = 1 << 9,
		Supressed = 1 << 10,
		Sleep = 1 << 11,
		Ghosted = 1 << 13,
		Charmed = 1 << 17,
		Slowed = 1 << 24,
	};
	enum class ObjectState
	{
		Alive = 1 << 0,
		Dead = 1 << 1,
	
	};
	enum class DamageType : uint8_t
	{
		InitDMG,
		Raw,
		AD,
		AP,
		Mixed
	};
	template<typename T,typename Y>
	inline bool operator&(T lhs, Y rhs)
	{

		return (static_cast<T>(lhs) & static_cast<T>(rhs)) != 0;
	}
	inline unsigned long operator|(ObjectTypeFlag lhs, ObjectTypeFlag rhs)
	{
		return static_cast<unsigned long>(
			static_cast<unsigned long>(lhs) |
			static_cast<unsigned long>(rhs)
			);
	}
	

	
}
static unsigned long default_ActionState = Enums::GameObjectActionState::CanAttack
| Enums::GameObjectActionState::CanSpell
| Enums::GameObjectActionState::CanMove
| Enums::GameObjectActionState::Candash;
inline uint8_t ToUInt8(const Enums::DamageType& In_DamageType)
{

	return static_cast<uint8_t>(In_DamageType);
}
inline Enums::DamageType ToDamageType(const uint8_t& In_DamageType)
{

	return static_cast<Enums::DamageType>(In_DamageType);
}
inline Enums::CurrentOrderType To_CurrentOrderType(uint8_t in)
{
	return static_cast<Enums::CurrentOrderType>(in);
}
inline Enums::CurrentOrderType FromSlot(Slot in_Slot)
{
	switch (in_Slot)
	{
	case Slot::Q:return Enums::CurrentOrderType::Spell_Q;
	case Slot::W:return Enums::CurrentOrderType::Spell_W;
	case Slot::E:return Enums::CurrentOrderType::Spell_E;
	case Slot::R:return Enums::CurrentOrderType::Spell_R;
	default:return  Enums::CurrentOrderType::None;
	}
}