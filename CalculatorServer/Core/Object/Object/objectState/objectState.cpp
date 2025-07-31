#include "objectState.h"

objectState::objectState()
{
    Level = 0;
    XP = 0.0f;
    MaxXP = 0.0f;
    Team = 0;
    DeadTime = 0.0f;
    ObjectState = 0UL;
    CurrentOrderType = 0;
    GameObjectActionState = 0UL;
    ObjectTypeFlag = 0UL;
    AttackTime = 0.f;
    ChangedFlags = 0;
}

void objectState::clear()
{
    std::unique_lock lock(objectStatemutex);
    Level = 0;
    XP = 0.0f;
    MaxXP = 0.0f;
    Team = 0;
    DeadTime = 0.0f;
    ObjectState = 0UL;
    CurrentOrderType = 0;
    GameObjectActionState = 0UL;
    ObjectTypeFlag = 0UL;
    AttackTime = 0.f;
    ChangedFlags = 0;
}

uint8_t objectState::Get_CurrentOrderType() const
{
    std::shared_lock lock(currentordermutex);

    return CurrentOrderType;
}

void objectState::Set_CurrentOrderType(uint8_t in_CurrentOrderType)
{
    std::unique_lock lock(currentordermutex);
    if (Get_DeadTime()>0.0f)return;
    ChangedFlags |= CurrentOrderTypeChanged;
    CurrentOrderType = in_CurrentOrderType;
}



