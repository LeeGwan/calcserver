#pragma once
#include "../../../utils/Vector.h"
#include "../ObjectComponent/Object.h"
struct struct_pos_for_navi
{
    Vec3 POS;
    float distance;
    struct_pos_for_navi(const Vec3& in_pos,const float& in_distance) : POS(in_pos), distance(in_distance) {}

    bool operator<(const struct_pos_for_navi& other) const
    {
        return distance < other.distance;
    }
};
struct struct_pos_return_obj
{
    Object* obj;
    float distance;
    struct_pos_return_obj(Object* in_obj, const float& in_distance) :obj(in_obj), distance(in_distance) {}
  
    bool operator<(const struct_pos_return_obj& other) const
    {
        return distance < other.distance;
    }
};