#include "CombatStats.h"

CombatStats::CombatStats():last_combat_time(0.0f)
{

}


void CombatStats::clear()
{
	std::unique_lock<std::shared_mutex> writelock(CombatStatsmutex);

	base_armor = 0.f;
	bonus_armor = 0.f;

	base_magic_RES = 0.f;
	bonus_magic_RES = 0.f;

	baseAD = 0.f;
	bonus_AD = 0.f;

	baseAP = 0.f;
	bonus_AP = 0.f;

	ability_haste = 0.f;
	lethality = 0.f;
	Armor_Penetration = 0.f;
	Crit = 0.f;
	DamageType = 0;
	ChangedFlags = 0;
	last_combat_time = 0.0f;
}
float CombatStats::Get_total_armor() const
{
	std::shared_lock<std::shared_mutex> readlock(CombatStatsmutex);
	return Get_base_armor() + Get_bonus_armor();
}
float CombatStats::Get_total_magic_RES() const
{
	std::shared_lock<std::shared_mutex> readlock(CombatStatsmutex);
	return Get_base_magic_RES()+ Get_bonus_magic_RES();
}
float CombatStats::Get_total_AD() const
{
	std::shared_lock<std::shared_mutex> readlock(CombatStatsmutex);
	return Get_baseAD()+ Get_bonus_AD();
}
float CombatStats::Get_total_AP() const
{
	std::shared_lock<std::shared_mutex> readlock(CombatStatsmutex);
	return Get_baseAP()+ Get_bonus_AP();
}

float CombatStats::Get_last_combat_time() const
{
	std::shared_lock<std::shared_mutex> readlock(CombatStatsmutex);
	return last_combat_time;
}

void CombatStats::Set_last_combat_time(float in_last_combat_time)
{
	std::unique_lock<std::shared_mutex> readlock(CombatStatsmutex);
	last_combat_time = in_last_combat_time;
}


