#include "Ezreal.h"
#include "../../../Flags.h"
#include "../Struct_ObjectDB.h"


Ezreal::Ezreal() = default;


Ezreal::~Ezreal() = default;


void Ezreal::load(std::vector<std::unique_ptr<Struct_ObjectDB>>& ObjectDB)
{
	std::unique_ptr<Struct_ObjectDB> DB = std::make_unique<Struct_ObjectDB>();
	DB->heroname = "ezreal";
	DB->O_CombatStats.Set_baseAD(60.0f).Set_LV_AD(2.5f).Set_base_armor(24.f).Set_base_magic_RES(30.f).Set_LV_armor(3.f).Set_LV_magic_RES(1.3f).Set_DamageType(ToUInt8(Enums::DamageType::AD));
	
	DB->O_objectStats.Set_movementspeed(325.f)//325.0f
		.Set_attackspeed(0.625f)
		.Set_LV_attackspeed(0.025f)
		.Set_base_attack_range(550.f)
		.Set_health(600.f)
		.Set_maxhelth(600.0f)
		.Set_LV_health(102.f)
		.Set_health_REGEN(0.55f)
		.Set_LV_health_REGEN(0.55f)
		.Set_mana(375.0f)
		.Set_maxmana(375.0f)
		.Set_LV_mana(45.f)
		.Set_mana_REGEN(8.5f)
		.Set_LV_mana_REGEN(0.65f).Set_missileSpeed(2000.f);
	DB->O_objectState.Set_Level(1).Set_ObjectState(static_cast<unsigned long>(Enums::ObjectState::Alive))
		.Set_GameObjectActionState(static_cast<unsigned long>(Enums::GameObjectActionState::CanAttack	| Enums::GameObjectActionState::CanSpell | Enums::GameObjectActionState::CanMove))
		.Set_CurrentOrderType(Enums::CurrentOrderType::Idle)
		.Set_BeforeOrderType(Enums::CurrentOrderType::None)
		.Set_CanchangeTime(0.f);
	
	DB->O_CharacterMesh.Set_CapsuleSIze(Vec3(60.5f, 60.5f, 90.f)).Set_Mesh_Location(Vec3(0.f, 0.f, 0.0f)).Set_Mesh_Rotation(Vec3(0.f, 0.f, -90.0f)).Set_Mesh_Scale(Vec3(0.01f, 0.01f, 0.01f));
	ObjectDB.push_back(std::move(DB));
}


