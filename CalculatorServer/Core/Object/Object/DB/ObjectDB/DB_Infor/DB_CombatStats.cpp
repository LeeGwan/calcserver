#include "DB_CombatStats.h"
#include "../../../CombatStats/CombatStats.h"
DB_CombatStats::DB_CombatStats()
{
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

}

void DB_CombatStats::CopyTo(CombatStats& out) const
{
	out.Set_base_armor(Get_base_armor());
	out.Set_LV_armor(Get_LV_armor());
	out.Set_bonus_armor(Get_bonus_armor());
	out.Set_base_magic_RES(Get_base_magic_RES());
	out.Set_LV_magic_RES(Get_LV_magic_RES());
	out.Set_bonus_magic_RES(Get_bonus_magic_RES());
	out.Set_baseAD(Get_baseAD());
	out.Set_bonus_AD(Get_bonus_AD());
	out.Set_LV_AD(Get_LV_AD());
	out.Set_baseAP(Get_baseAP());
	out.Set_LV_AP(Get_LV_AP());
	out.Set_bonus_AP(Get_bonus_AP());
	out.Set_ability_haste(Get_ability_haste());
	out.Set_lethality(Get_lethality());
	out.Set_Armor_Penetration(Get_Armor_Penetration());
	out.Set_Crit(Get_Crit());
	out.Set_DamageType(Get_DamageType());

}
