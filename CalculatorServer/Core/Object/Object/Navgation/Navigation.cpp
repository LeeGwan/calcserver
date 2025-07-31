#include "Navigation.h"

void Navigation::clear()
{
    std::unique_lock lock(Navigationmutex);
    NaviPath.clear();
    Current_NaviPath_Index = 0;
}

void Navigation::Add_Current_NaviPath_Index(const int in_Index)
{
    std::unique_lock lock(Navigationmutex);
    Current_NaviPath_Index += in_Index;
}
