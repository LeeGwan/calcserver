#pragma once
#include "../../../utils/Vector.h"
#include <mutex>
#include <shared_mutex>
#include <string>
#include "../../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
#define ADD_CHARACTER_MESH_FULL(TYPE, NAME)                                \
private:                                                                \
    TYPE NAME = {};                                                     \
public:                                                                 \
    TYPE Get_##NAME() const { std::shared_lock lock(CharacterMeshmutex); return NAME; } \
    CharacterMesh& Set_##NAME(const TYPE& value) { std::unique_lock lock(CharacterMeshmutex); NAME = value; return *this; }

class CharacterMesh final
{
public:
	void clear();
    ADD_CHARACTER_MESH_FULL(Vec3, CapsuleSIze)
    ADD_CHARACTER_MESH_FULL(Vec3, CapsuleLocation)
    ADD_CHARACTER_MESH_FULL(Vec3, Mesh_Location)
    ADD_CHARACTER_MESH_FULL(Vec3, Mesh_Rotation)
    ADD_CHARACTER_MESH_FULL(Vec3, Mesh_Scale)
     operator Struct_CharacterMesh() const
    {
        Struct_CharacterMesh out;
        out.CapsuleSIze = CapsuleSIze;
        out.CapsuleLocation = CapsuleLocation;
        out.Mesh_Location = Mesh_Location;
        out.Mesh_Rotation = Mesh_Rotation;
        out.Mesh_Scale = Mesh_Scale;
        return out;
    }
private:
	mutable std::shared_mutex CharacterMeshmutex;

};

