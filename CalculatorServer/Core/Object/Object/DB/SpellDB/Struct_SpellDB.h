#pragma once
#include <string>
#include <vector>
#include"../../Spells/SpellFlags/SpellFlags.h"
#define ADD_SPELL_DATA(TYPE, NAME)                                      \
private:                                                                      \
    TYPE NAME = {};                                                           \
public:                                                                       \
    TYPE Get_##NAME() const { return NAME; }                                  \
    Struct_SpellDB& Set_##NAME(const TYPE& value) { NAME = value; return *this; } \


struct Struct_SpellDB
{
    ADD_SPELL_DATA(Slot,ID)
    ADD_SPELL_DATA(std::string, HeroName)
    ADD_SPELL_DATA(std::string, SpellName)
    ADD_SPELL_DATA(std::vector<float>, Spell_Base_Mana)
    ADD_SPELL_DATA(std::vector<float>, Spell_Base_Damage)
    ADD_SPELL_DATA(std::vector<float>, Spell_Base_CoolDownTime)
    ADD_SPELL_DATA(float, Spell_Base_CastRange)
    ADD_SPELL_DATA(float, Spell_Base_CastRadius)
    ADD_SPELL_DATA(float, Spell_Base_missile_speed)
    ADD_SPELL_DATA(SpellType, Spell_Type) 
    ADD_SPELL_DATA(CollisionObject, Collision_Object)
    ADD_SPELL_DATA(DangerLevel, Danger_Level)
    ADD_SPELL_DATA(float, CastDelay)
  
};