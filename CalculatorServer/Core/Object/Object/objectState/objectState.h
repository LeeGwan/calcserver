#pragma once
#include <mutex>
#include <shared_mutex>
#include "../Flags.h"
#include "../../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
typedef unsigned char      uint8_t;
enum objectStateChangedFlags : uint16_t
{
    Level_Changed = 1 << 0,  // bit 0
    XPChanged = 1 << 1,
    MaxXPChanged = 1 << 2,
    DeadTimeChanged = 1 << 3,
    ObjectStateChanged = 1 << 4,
    CurrentOrderTypeChanged = 1 << 5,
};
#define ADD_OBJECT_STATE(TYPE, NAME) \
private: \
    TYPE NAME = {}; \
public: \
    TYPE Get_##NAME() const { std::shared_lock lock(objectStatemutex); return NAME; } \
    objectState& Set_##NAME(const TYPE& value) { std::unique_lock lock(objectStatemutex); NAME = value; return *this; }

#define CHECK_OBJECTSTATE_STATE(TYPE, NAME, FLAG) \
private: \
TYPE NAME = {}; \
public: \
TYPE Get_##NAME() const { std::shared_lock lock(objectStatemutex); return NAME; } \
objectState& Set_##NAME(const TYPE& value) { \
std::unique_lock lock(objectStatemutex); \
NAME = value; \
ChangedFlags |= FLAG; \
return *this; \
}
class objectState final
{
public:
    objectState();


    void clear();
    inline bool HasFlag(const uint16_t& flags)const { std::shared_lock lock(objectStatemutex); return (ChangedFlags & flags) != 0; };
    inline void ClearFlag(const uint16_t& flags) { std::unique_lock lock(objectStatemutex); ChangedFlags &= ~flags; };
    inline bool Has_Anti_Check_Level() { std::shared_lock lock(objectStatemutex); return Anti_Check_Level; };
    inline void Set_Anti_Check_Level(const bool& in_Anti_Check_Level) {std::unique_lock lock(objectStatemutex); Anti_Check_Level = in_Anti_Check_Level;}
    uint8_t Get_CurrentOrderType()const;
    void Set_CurrentOrderType(uint8_t in_CurrentOrderType);
    CHECK_OBJECTSTATE_STATE(unsigned, Level, objectStateChangedFlags::Level_Changed)//클라이언트
        CHECK_OBJECTSTATE_STATE(float, XP, objectStateChangedFlags::XPChanged)//클라이언트
        CHECK_OBJECTSTATE_STATE(float, MaxXP, objectStateChangedFlags::MaxXPChanged)//클라이언트
        ADD_OBJECT_STATE(int, Team)
        CHECK_OBJECTSTATE_STATE(float, DeadTime, objectStateChangedFlags::DeadTimeChanged)//클라이언트
        CHECK_OBJECTSTATE_STATE(uint8_t, ObjectState, objectStateChangedFlags::ObjectStateChanged)//클라이언트
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
       // CHECK_OBJECTSTATE_STATE(uint8_t, CurrentOrderType, CurrentOrderTypeChanged)//클라이언트
        ADD_OBJECT_STATE(uint8_t, BeforeOrderType)
        ADD_OBJECT_STATE(float, AttackTime)
        ADD_OBJECT_STATE(bool, IsStateRestored);
   

private:
    mutable std::shared_mutex objectStatemutex;
    mutable std::shared_mutex currentordermutex;
    uint16_t ChangedFlags = 0;
    bool Anti_Check_Level=false;
    uint8_t CurrentOrderType = 0;


};