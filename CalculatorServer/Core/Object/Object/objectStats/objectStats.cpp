#include "ObjectStats.h"


ObjectStats::ObjectStats()
{
}

void ObjectStats::clear()
{
    std::unique_lock lock(ObjectStatsmutex);
    movementspeed = 0.f;
    bonus_movementspeed = 0.f;
    attackspeed = 0.f;
    bonus_attackspeed = 0.f;
    base_attack_range = 0.f;
    bonus_attack_range = 0.f;
    health = 0.f;
    maxhelth = 0.f;
    health_REGEN = 0.f;
    mana = 0.f;
    maxmana = 0.f;
    mana_REGEN = 0.f;
    ChangedFlags = 0;
}

float ObjectStats::Get_total_MovementSpeed() const
{
    return movementspeed+ bonus_movementspeed;
}

float ObjectStats::Get_total_Attackspeed() const
{
    return attackspeed+ bonus_attackspeed;
}

float ObjectStats::Get_total_Attack_range() const
{
    return base_attack_range+ bonus_attack_range;
}


