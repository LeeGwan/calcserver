#include "CharacterMesh.h"

void CharacterMesh::clear()
{
    std::unique_lock lock(CharacterMeshmutex);
    CapsuleSIze = { 0.f,0.f,0.f };
    Mesh_Location = { 0.f,0.f,0.f };
    Mesh_Rotation = { 0.f,0.f,0.f };
    Mesh_Scale = { 0.f,0.f,0.f };
}
