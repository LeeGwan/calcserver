#include "DB_objectState.h"
#include"../../../objectState/objectState.h"


DB_objectState::DB_objectState()
{
    Level = 0;
    XP = 0.0f;
    MaxXP = 0.0f;
    Team = 0;
    DeadTime = 0.0f;
    ObjectState = 0UL;
    GameObjectActionState = 0UL;
    ObjectTypeFlag = 0UL;
    CurrentOrderType = 0;
}

void DB_objectState::CopyTo(objectState& out) const
{
    out.Set_Level(Get_Level());
    out.Set_XP(Get_XP());
    out.Set_MaxXP(Get_MaxXP());
    out.Set_Team(Get_Team());
    out.Set_DeadTime(Get_DeadTime());
    out.Set_ObjectState(Get_ObjectState());
    out.Set_GameObjectActionState(Get_GameObjectActionState());
    out.Set_ObjectTypeFlag(Get_ObjectTypeFlag());
    out.Set_CurrentOrderType(Get_CurrentOrderType());

    out.Set_AttackTime(Get_CanchangeTime());
}
