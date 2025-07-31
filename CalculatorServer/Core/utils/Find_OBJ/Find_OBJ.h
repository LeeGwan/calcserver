#pragma once
#include <unordered_map>
#include<string>

class Object;
class Vec3;
class Find_OBJ final
{
public:
	static bool Get_OBJ_To_Use_Hash(const std::unordered_map<std::string, Object*>& all_objects, const std::string& TargetObjectHash, Object*& Target_hero);
	static bool Find_Pos_is_near_Enemy(Object* self_obj, Object*& Out_obj, const std::unordered_map<std::string, Object*>& all_objects);
	static int count_nearby_enemies(Object* self_obj, const std::unordered_map<std::string, Object*>& all_objects, float range);
	static bool check_near_ally(Object* self_obj, const std::unordered_map<std::string, Object*>& all_objects);
};