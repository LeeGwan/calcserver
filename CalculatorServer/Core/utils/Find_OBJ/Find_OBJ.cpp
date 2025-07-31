#include "Find_OBJ.h"
#include "../Vector.h"
#include "../../Object/Object/ObjectComponent/Object.h"
#include "../../Object/Object/VMT/VMT.h"
#include"../../Object/Object/struct_pos/struct_pos.h"
#include "../../Object/Object/Transform/Transform.h"
#include"../../Object/Object/objectStats/objectStats.h"
#include "../../Error/is_valid.h"
#include <algorithm>
#include <limits>
bool Find_OBJ::Get_OBJ_To_Use_Hash(const std::unordered_map<std::string, Object*>& all_objects, const std::string& TargetObjectHash, Object*& Target_hero)
{
	if (TargetObjectHash.empty())return false;
	const auto& it = all_objects.find(TargetObjectHash);
	{
		if (it != all_objects.end())
		{
			Target_hero = it->second;
		}
	}
	
	if (!MemoryCheck::is_valid<Object>(Target_hero))return false;

	return true;
}

bool Find_OBJ::Find_Pos_is_near_Enemy(Object* self_obj,Object*& Out_obj, const std::unordered_map<std::string, Object*>& all_objects)
{
	std::vector<struct_pos_return_obj> all_objects_List;
	float min_distance = std::numeric_limits<float>::max();
	Vec3 self_pos = self_obj->Get_Transform_Component().Get_Position();
	Vec3 target_POS;
	float distance = 0.f;
	for (const auto& [key, obj] : all_objects)
	{
		if (!MemoryCheck::is_valid<Object>(obj))continue;
		if (!self_obj->Get_VMT_Component()->is_enemy(obj) || obj->Get_VMT_Component()->is_dead()
			||!obj->Get_VMT_Component()->is_hero())continue;

		target_POS = obj->Get_Transform_Component().Get_Position();
		distance = target_POS.distance(self_pos);
		if (distance < min_distance)
		{
			all_objects_List.push_back(struct_pos_return_obj(obj, distance));
		}
		
	}
	if (all_objects_List.empty())
	{
		Out_obj = nullptr;
		return false;
	}

	std::sort(all_objects_List.begin(), all_objects_List.end());
	Out_obj= all_objects_List.front().obj;
	return true;
}

int Find_OBJ::count_nearby_enemies(Object* self_obj, const std::unordered_map<std::string, Object*>& all_objects, float range)
{
	int count = 0;
	Vec3 self_pos = self_obj->Get_Transform_Component().Get_Position();
	Vec3 target_POS;
	float distance = 0.0f;
	for (const auto& [key, obj] : all_objects)
	{
		if (!MemoryCheck::is_valid<Object>(obj))continue;
		if (!self_obj->Get_VMT_Component()->is_enemy(obj) || obj->Get_VMT_Component()->is_dead()
			|| !obj->Get_VMT_Component()->is_hero())continue;
		target_POS = obj->Get_Transform_Component().Get_Position();
		distance = target_POS.distance(self_pos);
		if (distance <= range) {
			count++;
		}
	}
	return count;
}

bool Find_OBJ::check_near_ally(Object* self_obj, const std::unordered_map<std::string, Object*>& all_objects)
{
	Vec3 self_pos = self_obj->Get_Transform_Component().Get_Position();
	Vec3 target_POS;
	float distance = 0.0f;
	float range = 0.0f;
	for (const auto& [key, obj] : all_objects)
	{
		if (!MemoryCheck::is_valid<Object>(obj))continue;
		if (obj->Get_VMT_Component()->is_dead())continue;
		if (obj->Get_VMT_Component()->is_team(self_obj))
		{
			target_POS = obj->Get_Transform_Component().Get_Position();
			range = obj->Get_ObjectStats_Component().Get_total_Attack_range();
			distance = target_POS.distance(self_pos);
			if (distance <= range)
			{
				return true;
			}
		}
		
		
	}
	return false;
}
