#pragma once
#include "../../../utils/Vector.h"
#include <mutex>
#include <shared_mutex>
#include <string>
#include<vector>
#define ADD_NAVIGATION_MEMBER(TYPE, NAME)                                \
private:                                                                \
    TYPE NAME = {};                                                     \
public:                                                                 \
    TYPE Get_##NAME() const { std::shared_lock lock(Navigationmutex); return NAME; } \
    Navigation& Set_##NAME(const TYPE& value) { std::unique_lock lock(Navigationmutex); NAME = value; return *this; }
class Navigation final
{
public:
	void clear();
    void Add_Current_NaviPath_Index(const int in_Index);
    ADD_NAVIGATION_MEMBER(int, Current_NaviPath_Index)
    ADD_NAVIGATION_MEMBER(std::vector<Vec3>, NaviPath)


private:
	mutable std::shared_mutex Navigationmutex;

};

