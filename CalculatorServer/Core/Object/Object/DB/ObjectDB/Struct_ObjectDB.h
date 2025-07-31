#pragma once
#include <string>

#include "DB_Infor/DB_CombatStats.h"
#include "DB_Infor/DB_objectState.h"
#include "DB_Infor/DB_objectStats.h"
#include "DB_Infor/DB_CharacterMesh.h"


struct Struct_ObjectDB
{

    std::string heroname;
    DB_CombatStats O_CombatStats;
    DB_ObjectStats O_objectStats;
    DB_objectState O_objectState;
    DB_CharacterMesh O_CharacterMesh;
    Struct_ObjectDB() {}
    Struct_ObjectDB(const Struct_ObjectDB& other) :
        O_CombatStats(other.O_CombatStats),
        O_objectStats(other.O_objectStats),
        O_objectState(other.O_objectState),
        O_CharacterMesh(other.O_CharacterMesh)
    {
    }


    Struct_ObjectDB& operator=(const Struct_ObjectDB& other)
    {
        if (this != &other)
        {
            heroname = other.heroname;
            O_CombatStats = other.O_CombatStats;
            O_objectStats = other.O_objectStats;
            O_objectState = other.O_objectState;
            O_CharacterMesh = other.O_CharacterMesh;
        }
        return *this;
    }



};