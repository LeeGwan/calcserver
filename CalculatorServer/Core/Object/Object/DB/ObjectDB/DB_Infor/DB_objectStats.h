#pragma once

class ObjectStats;
#define ADD_OBJECT_STAT(TYPE, NAME)                                      \
private:                                                                 \
    TYPE NAME = {};                                                      \
public:                                                                  \
    TYPE Get_##NAME() const {  return NAME; } \
    DB_ObjectStats& Set_##NAME(const TYPE& value) {  NAME = value; return *this; }

class DB_ObjectStats final
{
public:
	DB_ObjectStats();
    void CopyTo(ObjectStats& out) const;
    ADD_OBJECT_STAT(float, missileSpeed)
    ADD_OBJECT_STAT(float, movementspeed)
        ADD_OBJECT_STAT(float, bonus_movementspeed)
        ADD_OBJECT_STAT(float, attackspeed)
        ADD_OBJECT_STAT(float, bonus_attackspeed)
        ADD_OBJECT_STAT(float, LV_attackspeed)
        ADD_OBJECT_STAT(float, base_attack_range)
        ADD_OBJECT_STAT(float, bonus_attack_range)
        ADD_OBJECT_STAT(float, health)
        ADD_OBJECT_STAT(float, LV_health)
        ADD_OBJECT_STAT(float, maxhelth)
        ADD_OBJECT_STAT(float, health_REGEN)
        ADD_OBJECT_STAT(float, LV_health_REGEN)
        ADD_OBJECT_STAT(float, mana)
        ADD_OBJECT_STAT(float, LV_mana)
        ADD_OBJECT_STAT(float, maxmana)
        ADD_OBJECT_STAT(float, mana_REGEN)
        ADD_OBJECT_STAT(float, LV_mana_REGEN)



};

