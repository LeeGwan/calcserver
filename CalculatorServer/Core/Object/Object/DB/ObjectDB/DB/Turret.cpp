#include "Turret.h"
#include "../../../Flags.h"
#include "../Struct_ObjectDB.h"


Turret::Turret() = default;


Turret::~Turret() = default;


void Turret::load(std::vector<std::unique_ptr<Struct_ObjectDB>>& ObjectDB)
{
	std::unique_ptr<Struct_ObjectDB> DB = std::make_unique<Struct_ObjectDB>();
	DB->heroname = "turret";
	DB->O_CombatStats
		.Set_base_armor(60.0f)
		.Set_base_magic_RES(60.0f)
		.Set_baseAD(170.f).Set_DamageType(ToUInt8(Enums::DamageType::Raw));
	DB->O_objectStats
		.Set_health(3500.0f)
		.Set_maxhelth(3500.0f)
		.Set_mana(1.0f)
		.Set_maxmana(1.0f)
		.Set_attackspeed(0.83f)
		.Set_base_attack_range(810.f);
	DB->O_objectState.Set_ObjectState(static_cast<unsigned long>(Enums::ObjectState::Alive))
		.Set_GameObjectActionState(static_cast<unsigned long>(Enums::GameObjectActionState::CanAttack))
	//	.Set_ObjectTypeFlag(static_cast<unsigned long>(Enums::ObjectTypeFlag::Turret))
		.Set_CurrentOrderType(Enums::CurrentOrderType::Idle)
		.Set_BeforeOrderType(Enums::CurrentOrderType::None)
		.Set_CanchangeTime(0.f);;

	DB->O_CharacterMesh.Set_CapsuleSIze(Vec3(300.0f, 550.0f, 0.f)).Set_CapsuleLocation(Vec3(0.0f,0.0f,550.0f));
	
	ObjectDB.push_back(std::move(DB));
}
