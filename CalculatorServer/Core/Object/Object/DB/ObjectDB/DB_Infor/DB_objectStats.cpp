#include "DB_ObjectStats.h"
#include "../../../objectStats/objectStats.h"

DB_ObjectStats::DB_ObjectStats()
{
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
}

void DB_ObjectStats::CopyTo(ObjectStats& out) const
{
    out.Set_missileSpeed(Get_missileSpeed());
    out.Set_movementspeed(Get_movementspeed());
    out.Set_bonus_movementspeed(Get_bonus_movementspeed());
    out.Set_attackspeed(Get_attackspeed());
    out.Set_LV_attackspeed(Get_LV_attackspeed());
    out.Set_bonus_attackspeed(Get_bonus_attackspeed());
    out.Set_base_attack_range(Get_base_attack_range());
    out.Set_bonus_attack_range(Get_bonus_attack_range());
    out.Set_health(Get_health());
    out.Set_LV_health(Get_LV_health());

    out.Set_maxhelth(Get_maxhelth());
    out.Set_health_REGEN(Get_health_REGEN());
    out.Set_LV_health_REGEN(Get_LV_health_REGEN());

    out.Set_mana(Get_mana());
    out.Set_LV_mana(Get_LV_mana());
    out.Set_maxmana(Get_maxmana());
    out.Set_mana_REGEN(Get_mana_REGEN());
    out.Set_LV_mana_REGEN(Get_LV_mana_REGEN());

}


