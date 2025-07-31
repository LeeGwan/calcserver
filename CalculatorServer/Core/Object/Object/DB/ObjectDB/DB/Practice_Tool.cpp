#include "Practice_Tool.h"
#include "../../../Flags.h"
#include "../Struct_ObjectDB.h"


Practice_Tool::Practice_Tool() = default;


Practice_Tool::~Practice_Tool() = default;

void Practice_Tool::load(std::vector<std::unique_ptr<Struct_ObjectDB>>& ObjectDB)
{
	std::unique_ptr<Struct_ObjectDB> DB = std::make_unique<Struct_ObjectDB>();
	DB->heroname = "practice_tool";
	DB->O_CombatStats.Set_DamageType(ToUInt8(Enums::DamageType::Raw));
	
	DB->O_objectStats.Set_movementspeed(325.f)
		.Set_attackspeed(0.625f)
		.Set_LV_attackspeed(0.025f)
		.Set_base_attack_range(550.f)
		.Set_health(1000.f)
		.Set_LV_health(102.f)
		.Set_health_REGEN(100.55f)
		.Set_LV_health_REGEN(100.55f)
		.Set_mana(375.f)
		.Set_LV_mana(45.f)
		.Set_mana_REGEN(8.5f)
		.Set_LV_mana_REGEN(0.65f);
	DB->O_objectState.Set_Level(1).Set_ObjectState(static_cast<unsigned long>(Enums::ObjectState::Alive))
		.Set_GameObjectActionState(static_cast<unsigned long>(Enums::GameObjectActionState::CanAttack | Enums::GameObjectActionState::CanSpell | Enums::GameObjectActionState::CanMove))
		.Set_ObjectTypeFlag(Enums::ObjectTypeFlag::Hero| Enums::ObjectTypeFlag::AI)
		.Set_CurrentOrderType(Enums::CurrentOrderType::Idle)
		.Set_BeforeOrderType(Enums::CurrentOrderType::None)
		.Set_CanchangeTime(0.f);
	
	DB->O_CharacterMesh.Set_CapsuleSIze(Vec3(65.f, 65.f, 188.f)).Set_Mesh_Location(Vec3(0.f, 0.f, -88.298531f)).Set_Mesh_Rotation(Vec3(0.f, 0.f, -90.0f)).Set_Mesh_Scale(Vec3(0.01f, 0.01f, 0.01f));
	ObjectDB.push_back(std::move(DB));
}
