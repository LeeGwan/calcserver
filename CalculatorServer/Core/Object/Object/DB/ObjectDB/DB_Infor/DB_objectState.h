#pragma once

#include "../../../Flags.h"
typedef unsigned char      uint8_t;
class objectState;
#define ADD_OBJECT_STATE(TYPE, NAME) \
private: \
    TYPE NAME = {}; \
public: \
    TYPE Get_##NAME() const { return NAME; } \
    DB_objectState& Set_##NAME(const TYPE& value) {NAME = value; return *this; }

class DB_objectState final
{
public:
    DB_objectState();
    
    void CopyTo(objectState& out) const;
    ADD_OBJECT_STATE(unsigned int, Level)
    ADD_OBJECT_STATE(float, XP)
    ADD_OBJECT_STATE(float, MaxXP)
    ADD_OBJECT_STATE(int, Team)
    ADD_OBJECT_STATE(float, DeadTime)
    ADD_OBJECT_STATE(unsigned long, ObjectState)
    /*
    Alive = 1 << 0,
    Dead = 1 << 1,
    */
    ADD_OBJECT_STATE(unsigned long, GameObjectActionState)
   /*
    CanAttack = 1 << 0,
    CanCrit = 1 << 1,
    CanCast = 1 << 2,
    CanMove = 1 << 3,
    Immovable = 1 << 4,
    Stealthed = 1 << 5,
    Obscured = 1 << 6,
    Taunted = 1 << 7,
    Feared = 1 << 8,
    Fleeing = 1 << 9,
    Supressed = 1 << 10,
    Sleep = 1 << 11,
    Ghosted = 1 << 13,
    Charmed = 1 << 17,
    Slowed = 1 << 24,
    */
    ADD_OBJECT_STATE(unsigned long, ObjectTypeFlag)
    /*
     Object = (1 << 0),			//0x1
     AI=(1 << 1),				//0x2
     Hero = (1 << 2),			//0x4
     Minion= (1<<3),				//0x8
     Melee_Minion=(1<<4),		//0x10
     Ranged_Minion = (1 << 5),	//0x20
     AttackableUnit = (1 << 6),	//0x40
     Turret = (1 << 7),			//0x80
     Nexus = (1 << 8),			//0x160
     Spell_Missile = (1 << 9),	//0x180
     Spell_Circle = (1 << 10),	//0x200
     Spell_Custom = (1 << 11),	//0x220
     */
    ADD_OBJECT_STATE(uint8_t, CurrentOrderType)
    ADD_OBJECT_STATE(uint8_t, BeforeOrderType)
    ADD_OBJECT_STATE(float, CanchangeTime)
};