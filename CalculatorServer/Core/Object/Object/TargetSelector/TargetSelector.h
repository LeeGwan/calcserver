#pragma once
#include <mutex>
#include <shared_mutex>
#include <string>
#include "../../../../Core/utils/Vector.h"
class Object;
#define ADD_TARGETSELECTOR_MEMBER(TYPE, NAME)                                \
private:                                                                \
    TYPE NAME = {};                                                     \
public:                                                                 \
    TYPE Get_##NAME() const { std::shared_lock lock(TargetSelectormutex); return NAME; } \
    TargetSelector& Set_##NAME(const TYPE& value) { std::unique_lock lock(TargetSelectormutex); NAME = value; return *this; }

class TargetSelector final
{
public:
    Object* Get_Taret_obj();
    void Set_Taret_obj(Object* in_obj);
	void clear();
 
    ADD_TARGETSELECTOR_MEMBER(std::string, TargetHash)
    ADD_TARGETSELECTOR_MEMBER(Vec3, Target_Pos)
   // ADD_TARGETSELECTOR_MEMBER(float, Time)

private:
	mutable std::shared_mutex TargetSelectormutex;
    Object* Targetobj;

};

