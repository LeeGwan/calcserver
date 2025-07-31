#include "Nexus.h"
#include "../../../Flags.h"
#include "../Struct_ObjectDB.h"


Nexus::Nexus() = default;


Nexus::~Nexus() = default;


void Nexus::load(std::vector<std::unique_ptr<Struct_ObjectDB>>& ObjectDB)
{
	std::unique_ptr<Struct_ObjectDB> DB = std::make_unique<Struct_ObjectDB>();
	DB->heroname = "nexus";

	
	DB->O_CombatStats
		.Set_base_armor(20.0f)
		.Set_base_magic_RES(20.0f);
	DB->O_objectStats
		.Set_health(5000.0f)
		.Set_maxhelth(5000.0f)
		.Set_mana(1.0f)
		.Set_maxmana(1.0f);
	DB->O_objectState
		.Set_ObjectState(static_cast<unsigned long>(Enums::ObjectState::Alive))

		.Set_CurrentOrderType(Enums::CurrentOrderType::Idle)
		.Set_BeforeOrderType(Enums::CurrentOrderType::None)
		.Set_CanchangeTime(0.f);;;
	DB->O_CharacterMesh.Set_CapsuleSIze(Vec3(230.0f, 230.0f, 0.0f)).Set_CapsuleLocation(Vec3(0.0f,0.0f,40.0f)).Set_Mesh_Location(Vec3(-270.0f,260.0f,0.0f)).Set_Mesh_Scale(Vec3(0.11f, 0.11f, 0.11f));
	ObjectDB.push_back(std::move(DB));
}
