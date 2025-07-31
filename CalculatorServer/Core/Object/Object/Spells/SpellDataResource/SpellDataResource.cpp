#include "SpellDataResource.h"
#include "../SpellFlags/SpellFlags.h"
float SpellDataResource::Get_Spell_Base_Mana(const unsigned int& in_Lv) const
{
	std::shared_lock Lock(SpellDataResource);
	return Spell_Base_Mana[in_Lv];
}

float SpellDataResource::Get_Spell_Base_Damage(const unsigned int& in_Lv) const
{
	std::shared_lock Lock(SpellDataResource);
	return Spell_Base_Damage[in_Lv];
}

float SpellDataResource::Get_Spell_Base_CoolDownTime(const unsigned int& in_Lv) const
{
	std::shared_lock Lock(SpellDataResource);
	return Spell_Base_CoolDownTime[in_Lv];
}

float SpellDataResource::Get_Spell_Base_CastRange() const
{
	std::shared_lock Lock(SpellDataResource);
	return Spell_Base_CastRange;
}

std::vector<float> SpellDataResource::Get_Spell_Base_Damage() const
{
	std::shared_lock Lock(SpellDataResource);
	return Spell_Base_Damage;
}



float SpellDataResource::Get_Spell_Base_CastRadius() const
{
	std::shared_lock Lock(SpellDataResource);
	return Spell_Base_CastRadius;
}

float SpellDataResource::Get_Spell_Base_missile_speed() const
{
	return Spell_Base_missile_speed;
}

DangerLevel SpellDataResource::Get_Danger_Level() const
{
	return Danger_Level;
}

CollisionObject SpellDataResource::Get_Collision_Object() const
{
	return Collision_Object;
}

SpellType SpellDataResource::Get_SpellType() const
{
	std::shared_lock Lock(SpellDataResource);
	return Spell_Type;
}

float SpellDataResource::Get_CastDelay() const
{
	return CastDelay;
}

void SpellDataResource::Set_Spell_Base_Mana(const std::vector<float>& in_Spell_Base_Mana)
{
	std::unique_lock Lock(SpellDataResource);
	Spell_Base_Mana = in_Spell_Base_Mana;
}

void SpellDataResource::Set_Spell_Base_Damage(const std::vector<float>& in_Spell_Base_Damage)
{
	std::unique_lock Lock(SpellDataResource);
	Spell_Base_Damage = in_Spell_Base_Damage;
}

void SpellDataResource::Set_Spell_Base_CoolDownTime(const std::vector<float>& in_Spell_Base_CoolDownTime)
{
	std::unique_lock Lock(SpellDataResource);
	Spell_Base_CoolDownTime = in_Spell_Base_CoolDownTime;
}

void SpellDataResource::Set_Spell_Base_CastRange(const float& in_Spell_Base_CastRange)
{
	std::unique_lock Lock(SpellDataResource);
	Spell_Base_CastRange = in_Spell_Base_CastRange;
}



void SpellDataResource::Set_Spell_Base_CastRadius(const float& in_Spell_Base_CastRadius)
{
	std::unique_lock Lock(SpellDataResource);
	Spell_Base_CastRadius = in_Spell_Base_CastRadius;
}

void SpellDataResource::Set_Spell_Base_missile_speed(const float& in_Spell_Base_missile_speed)
{
	std::unique_lock Lock(SpellDataResource);
	Spell_Base_missile_speed = in_Spell_Base_missile_speed;
}

void SpellDataResource::Set_SpellType(const SpellType& in_SpellType)
{
	std::unique_lock Lock(SpellDataResource);
	Spell_Type = in_SpellType;
}
void SpellDataResource::Set_CastDelay(const float& in_Get_CastDelay)
{
	std::unique_lock Lock(SpellDataResource);
	CastDelay = in_Get_CastDelay;
}
void SpellDataResource::Set_Get_Collision_Object(const CollisionObject& in_Collision_Object)
{
	std::unique_lock Lock(SpellDataResource);
	Collision_Object = in_Collision_Object;
}
void SpellDataResource::Set_Danger_Level(const DangerLevel& in_Danger_Level)
{
	std::unique_lock Lock(SpellDataResource);
	Danger_Level = in_Danger_Level;
}
void SpellDataResource::Release()
{
	std::unique_lock Lock(SpellDataResource);

}
