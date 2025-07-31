#pragma once

#include <string>
#include "../../../../../utils/variableUtils.h"
class CombatStats;


#define ADD_COMBAT_STAT_FULL(TYPE, NAME)                        \
private:                                                        \
    TYPE NAME = {};                                             \
public:                                                         \
    TYPE Get_##NAME() const { return NAME; } \
    DB_CombatStats& Set_##NAME(const TYPE& value) { NAME = value; return *this; } \
    DB_CombatStats& Add_##NAME(const TYPE& value) { NAME += value; return *this; }


class DB_CombatStats final
{
public:
	DB_CombatStats();

	void CopyTo(CombatStats& out) const;
	

	ADD_COMBAT_STAT_FULL(float, base_armor)
	ADD_COMBAT_STAT_FULL(float, LV_armor)
	ADD_COMBAT_STAT_FULL(float, bonus_armor)
	ADD_COMBAT_STAT_FULL(float, base_magic_RES)
	ADD_COMBAT_STAT_FULL(float, LV_magic_RES)
	ADD_COMBAT_STAT_FULL(float, bonus_magic_RES)
	ADD_COMBAT_STAT_FULL(float, baseAD)
	ADD_COMBAT_STAT_FULL(float, LV_AD)
	ADD_COMBAT_STAT_FULL(float, bonus_AD)
	ADD_COMBAT_STAT_FULL(float, baseAP)
	ADD_COMBAT_STAT_FULL(float, LV_AP)
	ADD_COMBAT_STAT_FULL(float, bonus_AP)

	// 기타 전투 스탯
	ADD_COMBAT_STAT_FULL(float, ability_haste)
	ADD_COMBAT_STAT_FULL(float, lethality)
	ADD_COMBAT_STAT_FULL(float, Armor_Penetration)
	ADD_COMBAT_STAT_FULL(float, Crit)
	ADD_COMBAT_STAT_FULL(uint8_t,DamageType)


};

