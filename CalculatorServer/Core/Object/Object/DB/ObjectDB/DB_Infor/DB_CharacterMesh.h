#pragma once

#include <mutex>
#include <shared_mutex>
#include <string>
#include "../../../../../utils/Vector.h"
class CharacterMesh;

#define ADD_CHARACTER_MESH_FULL(TYPE, NAME)                        \
private:                                                        \
    TYPE NAME = {};                                             \
public:                                                         \
    TYPE Get_##NAME() const { return NAME; } \
    DB_CharacterMesh& Set_##NAME(const TYPE& value) { NAME = value; return *this; } 
   

class DB_CharacterMesh final
{
public:
    DB_CharacterMesh();
    void CopyTo(CharacterMesh& out) const;
    ADD_CHARACTER_MESH_FULL(Vec3, CapsuleSIze)
    ADD_CHARACTER_MESH_FULL(Vec3, CapsuleLocation)
    ADD_CHARACTER_MESH_FULL(Vec3, Mesh_Location)
    ADD_CHARACTER_MESH_FULL(Vec3, Mesh_Rotation)
    ADD_CHARACTER_MESH_FULL(Vec3, Mesh_Scale)


};

