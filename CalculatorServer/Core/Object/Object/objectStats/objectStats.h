#pragma once
#include <mutex>
#include <shared_mutex>
#include "../../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
enum ObjectStatsChangedFlags : uint16_t
{
    attack_range_Changed = 1 << 0,  // bit 0
    healthChanged = 1 << 1,
    maxhealthChanged = 1 << 2,
    manaChanged = 1 << 3,
    maxmanaChanged = 1 << 4,
};
#define ADD_OBJECT_STAT(TYPE, NAME)                                      \
private:                                                                 \
    TYPE NAME = {};                                                      \
public:                                                                  \
    TYPE Get_##NAME() const { std::shared_lock lock(ObjectStatsmutex); return NAME; } \
    ObjectStats& Set_##NAME(const TYPE& value) { std::unique_lock lock(ObjectStatsmutex); NAME = value; return *this; }

#define CHECK_OBJECTSTATS_STATE(TYPE, NAME, FLAG) \
private: \
TYPE NAME = {}; \
public: \
TYPE Get_##NAME() const { std::shared_lock lock(ObjectStatsmutex); return NAME; } \
ObjectStats& Set_##NAME(const TYPE& value) { \
std::unique_lock lock(ObjectStatsmutex); \
NAME = value; \
ChangedFlags |= FLAG; \
return *this; \
}
class ObjectStats final
{
public:
	ObjectStats();
    void clear();
    float Get_total_MovementSpeed()const;
    float Get_total_Attackspeed()const;
    float Get_total_Attack_range()const;//클라이언트
    inline bool HasFlag(const uint16_t& flags)const { return (ChangedFlags & flags) != 0; };
    inline void ClearFlag(const uint16_t& flags) { ChangedFlags &= ~flags; };
    ADD_OBJECT_STAT(float, missileSpeed)
    ADD_OBJECT_STAT(float, movementspeed)
    ADD_OBJECT_STAT(float, bonus_movementspeed)
    ADD_OBJECT_STAT(float, attackspeed)
    ADD_OBJECT_STAT(float, bonus_attackspeed)
    ADD_OBJECT_STAT(float, LV_attackspeed)
    CHECK_OBJECTSTATS_STATE(float, base_attack_range, attack_range_Changed)
    CHECK_OBJECTSTATS_STATE(float, bonus_attack_range, attack_range_Changed)
    CHECK_OBJECTSTATS_STATE(float, health, healthChanged)//클라이언트
    ADD_OBJECT_STAT(float, LV_health)
    CHECK_OBJECTSTATS_STATE(float, maxhelth, maxhealthChanged)//클라이언트
    ADD_OBJECT_STAT(float, health_REGEN)
    ADD_OBJECT_STAT(float, LV_health_REGEN)
    CHECK_OBJECTSTATS_STATE(float, mana, manaChanged)//클라이언트
    ADD_OBJECT_STAT(float, LV_mana)
    CHECK_OBJECTSTATS_STATE(float, maxmana, maxmanaChanged)//클라이언트
    ADD_OBJECT_STAT(float, mana_REGEN)
    ADD_OBJECT_STAT(float, LV_mana_REGEN)



private:
	mutable std::shared_mutex ObjectStatsmutex;
  
    uint16_t ChangedFlags = 0;

};

