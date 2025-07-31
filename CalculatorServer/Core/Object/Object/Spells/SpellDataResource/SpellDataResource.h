#pragma once
#include <mutex>
#include <shared_mutex>
#include <vector>

enum class SpellType : unsigned int;
enum class CollisionObject : unsigned long;
enum class DangerLevel;

class SpellDataResource final
{
public:
	float			Get_Spell_Base_Mana(const unsigned int& in_Lv)const;
	float			Get_Spell_Base_Damage(const unsigned int& in_Lv)const;
	float			Get_Spell_Base_CoolDownTime(const unsigned int& in_Lv)const;
	float			Get_Spell_Base_CastRange()const;
	std::vector<float> Get_Spell_Base_Damage()const;
	float			Get_Spell_Base_CastRadius ()const;
	float			Get_Spell_Base_missile_speed()const;
	DangerLevel		Get_Danger_Level()const;
	CollisionObject Get_Collision_Object()const;
	SpellType		Get_SpellType()const;
	float			Get_CastDelay()const;
	void			Set_Spell_Base_Mana(const std::vector<float>& in_Spell_Base_Mana);
	void			Set_Spell_Base_Damage(const std::vector<float>& in_Spell_Base_Damage);
	void			Set_Spell_Base_CoolDownTime(const std::vector<float>& in_Spell_Base_CoolDownTime);
	void			Set_Spell_Base_CastRange(const float& in_Spell_Base_CastRange);

	void			Set_Spell_Base_CastRadius(const float& in_Spell_Base_CastRadius);
	void			Set_Spell_Base_missile_speed(const float& in_Spell_Base_missile_speed);
	void			Set_SpellType(const SpellType& in_SpellType);
	void			Set_CastDelay(const float& in_Get_CastDelay);
	void			Set_Get_Collision_Object(const CollisionObject& in_Collision_Object);
	void			Set_Danger_Level(const DangerLevel& in_Danger_Level);
	void	 		Release();
private:
	mutable std::shared_mutex SpellDataResource;
	std::vector<float> Spell_Base_Mana;
	std::vector<float> Spell_Base_Damage;
	std::vector<float> Spell_Base_CoolDownTime;//lvº° ÄðÅ¸ÀÓ
	float Spell_Base_CastRange;
	
	float Spell_Base_CastRadius;
	float Spell_Base_missile_speed;
	SpellType Spell_Type;
	CollisionObject Collision_Object;
	DangerLevel Danger_Level;
	float CastDelay;
};

