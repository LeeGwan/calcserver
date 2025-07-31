#pragma once
#include <mutex>
#include <shared_mutex>
#include <string>
#include "../../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
#include "../../../utils/variableUtils.h"
enum CombatStatsChangedFlags : uint16_t
{
	ArmorChanged = 1 << 0,  // bit 0
	magic_RESChanged = 1 << 1,
	ADChanged = 1 << 2,
	APChanged = 1 << 3,
	NON=1<<4
};
#define ADD_COMBAT_STAT_FULL(TYPE, NAME, FLAG)                        \
private:                                                        \
    TYPE NAME = {};                                             \
public:                                                         \
    TYPE Get_##NAME() const { std::shared_lock lock(CombatStatsmutex); return NAME; } \
    CombatStats& Set_##NAME(const TYPE& value) { std::unique_lock lock(CombatStatsmutex);ChangedFlags |= FLAG;  NAME = value; return *this; } \
    CombatStats& Add_##NAME(const TYPE& value) { std::unique_lock lock(CombatStatsmutex);ChangedFlags |= FLAG; NAME += value; return *this; }


class CombatStats final
{
public:
	CombatStats();

	void clear();
	float Get_total_armor()const;//클라이언트
	float Get_total_magic_RES()const;//클라이언트
	float Get_total_AD()const;//클라이언트
	float Get_total_AP()const;//클라이언트
	float Get_last_combat_time()const;//서버전용
	void Set_last_combat_time(float in_last_combat_time);
	inline bool HasFlag(const uint16_t& flags)const { return (ChangedFlags & flags) != 0; };
	inline void ClearFlag(const uint16_t& flags) {  ChangedFlags &= ~flags; };

	

	ADD_COMBAT_STAT_FULL(float, base_armor, ArmorChanged)
		ADD_COMBAT_STAT_FULL(float, LV_armor, ArmorChanged)
		ADD_COMBAT_STAT_FULL(float, bonus_armor, ArmorChanged)

		ADD_COMBAT_STAT_FULL(float, base_magic_RES, magic_RESChanged)
		ADD_COMBAT_STAT_FULL(float, LV_magic_RES, magic_RESChanged)
		ADD_COMBAT_STAT_FULL(float, bonus_magic_RES, magic_RESChanged)

		ADD_COMBAT_STAT_FULL(float, baseAD, ADChanged)
		ADD_COMBAT_STAT_FULL(float, LV_AD, ADChanged)
		ADD_COMBAT_STAT_FULL(float, bonus_AD, ADChanged)
		ADD_COMBAT_STAT_FULL(float, baseAP, APChanged)
		ADD_COMBAT_STAT_FULL(float, LV_AP, APChanged)
		ADD_COMBAT_STAT_FULL(float, bonus_AP, APChanged)

		// 기타 전투 스탯
		ADD_COMBAT_STAT_FULL(float, ability_haste, NON)
		ADD_COMBAT_STAT_FULL(float, lethality, NON)//물리관통력
		ADD_COMBAT_STAT_FULL(float, Armor_Penetration, NON)//방어구관통력
		ADD_COMBAT_STAT_FULL(float, Crit, NON)
		ADD_COMBAT_STAT_FULL(uint8_t, DamageType, NON)

private:
	mutable std::shared_mutex CombatStatsmutex;
	uint16_t ChangedFlags = 0;
	float last_combat_time;
};

