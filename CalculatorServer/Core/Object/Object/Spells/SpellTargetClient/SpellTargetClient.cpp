#include "SpellTargetClient.h"

SpellTargetClient::SpellTargetClient()
{
    CasterObject = nullptr;
    TargetObject = nullptr;
    
}

Object* SpellTargetClient::Get_CasterObject() const
{
    std::shared_lock Lock(SpellTargetClientmutex);
    return CasterObject;
}

Object* SpellTargetClient::Get_TargetObject() const
{
    std::shared_lock Lock(SpellTargetClientmutex);
    return TargetObject;
}

Vec3 SpellTargetClient::Get_Start_Position() const
{
    std::shared_lock Lock(SpellTargetClientmutex);
    return Start_Position;
}

Vec3 SpellTargetClient::Get_End_Position() const
{
    std::shared_lock Lock(SpellTargetClientmutex);
    return End_Position;
}

void SpellTargetClient::Set_CasterObject(Object* in_Caster)
{
    std::shared_lock Lock(SpellTargetClientmutex);
    CasterObject = in_Caster;
}

void SpellTargetClient::Set_TargetObject(Object* in_Target)
{
    std::unique_lock Lock(SpellTargetClientmutex);
    TargetObject = in_Target;
}

void SpellTargetClient::Set_Start_Position(const Vec3& in_Start_Position)
{
    std::unique_lock Lock(SpellTargetClientmutex);
    Start_Position = in_Start_Position;
}

void SpellTargetClient::Set_End_Position(const Vec3& in_End_Position)
{
    std::unique_lock Lock(SpellTargetClientmutex);
    End_Position =in_End_Position;
}
void SpellTargetClient::Release()
{
    std::unique_lock Lock(SpellTargetClientmutex);
 
}
