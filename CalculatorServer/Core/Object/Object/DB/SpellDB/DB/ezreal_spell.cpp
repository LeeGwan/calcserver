#include "ezreal_spell.h"
#include "../../../Spells/SpellFlags/SpellFlags.h"
#include "../Struct_SpellDB.h"
void ezreal_spell::load_q_data(std::vector<std::unique_ptr<Struct_SpellDB>>& SpellDB)
{
	std::unique_ptr<Struct_SpellDB> q_data = std::make_unique<Struct_SpellDB>();
//	std::vector<float> q_CoolDownTime = { 5.5f, 5.25f, 5.0f, 4.75f, 4.5f };
	std::vector<float> q_CoolDownTime = { 3.0f, 3.0f, 3.0f, 4.75f, 4.5f };
	std::vector<float> q_Damage = { 20.0f, 45.0f, 70.0f, 95.0f, 120.0f }; //+130 % AD
	std::vector<float> q_mana_cost = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	//std::vector<float> q_mana_cost = { 2.0f, 31.0f, 34.0f, 37.0f, 40.0f };
	q_data->Set_ID(Slot::Q)
		.Set_HeroName("ezreal")
		.Set_SpellName("ezreal_q")
		.Set_Spell_Type(SpellType::Missile)
		.Set_Spell_Base_CastRange(1200.f)//1200.0f
		.Set_Spell_Base_CastRadius(55.f)
		.Set_Spell_Base_missile_speed(2000.0f)//2000.0f
		.Set_Danger_Level(DangerLevel::Medium)
		.Set_Collision_Object(CollisionObject::YasuoWall | CollisionObject::Heroes | CollisionObject::Minions)
		.Set_CastDelay(0.25f)
		.Set_Spell_Base_CoolDownTime(q_CoolDownTime)
		.Set_Spell_Base_Damage(q_Damage)
		.Set_Spell_Base_Mana(q_mana_cost);
		
	SpellDB.push_back(std::move(q_data));
}

void ezreal_spell::load_w_data(std::vector<std::unique_ptr<Struct_SpellDB>>& SpellDB)
{
	std::unique_ptr<Struct_SpellDB> w_data = std::make_unique<Struct_SpellDB>();
	//std::vector<float> w_CoolDownTime = { 12.0f, 11.0f, 10.0f, 9.0f, 8.0f };
	std::vector<float> w_CoolDownTime = { 3.0f, 3.0f, 3.0f, 3.0f, 3.0f };
	std::vector<float> w_Damage = { 70.0f, 115.0f, 160.0f, 205.0f, 250.0f };//+80% AP)
	std::vector<float> w_mana_cost = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	//std::vector<float> w_mana_cost = { 50.0f, 60.0f, 70.0f, 80.0f, 90.0f };
	w_data->Set_ID(Slot::W)
		.Set_HeroName("ezreal")
		.Set_SpellName("ezreal_w")
		.Set_Spell_Type(SpellType::Missile)
		.Set_Spell_Base_CastRange(1200.f)
		.Set_Spell_Base_CastRadius(60.f)
		.Set_Spell_Base_missile_speed(1700.f)
		.Set_Danger_Level(DangerLevel::Low)
		.Set_Collision_Object(CollisionObject::YasuoWall | CollisionObject::Heroes)
		.Set_CastDelay(0.25f)
		.Set_Spell_Base_CoolDownTime(w_CoolDownTime)
		.Set_Spell_Base_Damage(w_Damage)
		.Set_Spell_Base_Mana(w_mana_cost);
	SpellDB.push_back(std::move(w_data));
}

void ezreal_spell::load_e_data(std::vector<std::unique_ptr<Struct_SpellDB>>& SpellDB)
{
	std::unique_ptr<Struct_SpellDB> e_data = std::make_unique<Struct_SpellDB>();
	std::vector<float> e_CoolDownTime = { 10.0f, 10.0f, 10.0f, 10.0f, 10.0f };
	//std::vector<float> e_CoolDownTime = { 28.0f, 25.0f, 22.0f, 19.0f, 16.0f };
	std::vector<float> e_Damage = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };//+80% AP)
	std::vector<float> e_mana_cost = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	//std::vector<float> e_mana_cost = { 90.0f, 90.0f, 90.0f, 90.0f, 90.0f };
	e_data->Set_ID(Slot::E)
		.Set_HeroName("ezreal")
		.Set_SpellName("ezreal_e")
		.Set_Spell_Type(SpellType::Blink)
		.Set_Spell_Base_CastRange(475.f)
		.Set_Spell_Base_CastRadius(0.0f)
		.Set_Spell_Base_missile_speed(0.0f)
		.Set_Danger_Level(DangerLevel::Never)
		.Set_Collision_Object(CollisionObject::GameWall)
		.Set_CastDelay(0.0f)
		.Set_Spell_Base_CoolDownTime(e_CoolDownTime)
		.Set_Spell_Base_Damage(e_Damage)
		.Set_Spell_Base_Mana(e_mana_cost);
	SpellDB.push_back(std::move(e_data));
}

void ezreal_spell::load_r_data(std::vector<std::unique_ptr<Struct_SpellDB>>& SpellDB)
{
	std::unique_ptr<Struct_SpellDB> r_data = std::make_unique<Struct_SpellDB>();
	std::vector<float> r_CoolDownTime = { 120.0f, 105.0f, 90.0f, 90.0f, 90.0f };
	std::vector<float> r_Damage = { 350.0f, 500.0f, 650.0f, 650.0f, 650.0f };//	+100% AD	+90% AP
	std::vector<float> r_mana_cost = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }; //= { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f };
	r_data->Set_ID(Slot::R)
		.Set_HeroName("ezreal")
		.Set_SpellName("ezreal_r")
		.Set_Spell_Type(SpellType::Missile)
		.Set_Spell_Base_CastRange(30000.f)
		.Set_Spell_Base_CastRadius(100.f)
		.Set_Spell_Base_missile_speed(2000.f)
		.Set_Danger_Level(DangerLevel::Medium)
		.Set_Collision_Object(CollisionObject::YasuoWall)
		.Set_CastDelay(1.0f)
		.Set_Spell_Base_CoolDownTime(r_CoolDownTime)
		.Set_Spell_Base_Damage(r_Damage)
		.Set_Spell_Base_Mana(r_mana_cost);
	SpellDB.push_back(std::move(r_data));
}
ezreal_spell::ezreal_spell() = default;

ezreal_spell::~ezreal_spell() = default;

void ezreal_spell::load(std::vector<std::unique_ptr<Struct_SpellDB>>& SpellDB)
{
	load_q_data(SpellDB);
	load_w_data(SpellDB);
	load_e_data(SpellDB);
	load_r_data(SpellDB);
}