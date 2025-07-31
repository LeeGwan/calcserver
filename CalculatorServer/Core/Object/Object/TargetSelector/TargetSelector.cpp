#include "TargetSelector.h"
#include "../ObjectComponent/Object.h"
Object* TargetSelector::Get_Taret_obj()
{
    std::shared_lock lock(TargetSelectormutex);
    return Targetobj;
}

void TargetSelector::Set_Taret_obj(Object* in_obj)
{
    std::unique_lock lock(TargetSelectormutex);
    Targetobj = in_obj;
}

void TargetSelector::clear()
{
    std::unique_lock lock(TargetSelectormutex);
    Target_Pos = { 0.f,0.f,0.f };
    TargetHash.clear();
    Targetobj = nullptr;
}
