#include "DB_CharacterMesh.h"
#include "../../../CharacterMesh/CharacterMesh.h"


DB_CharacterMesh::DB_CharacterMesh()
{
	CapsuleSIze = { 0.f,0.f, 0.f };
	CapsuleLocation = { 0.f,0.f, 0.f };
	Mesh_Location = { 0.f,0.f, 0.f };
	Mesh_Rotation = { 0.f,0.f, 0.f };
	Mesh_Scale = { 0.f,0.f, 0.f };
}

void DB_CharacterMesh::CopyTo(CharacterMesh& out) const
{
	out.Set_CapsuleSIze(Get_CapsuleSIze());
	out.Set_CapsuleLocation(Get_CapsuleLocation());
	out.Set_Mesh_Location(Get_Mesh_Location());
	out.Set_Mesh_Rotation(Get_Mesh_Rotation());
	out.Set_Mesh_Scale(Get_Mesh_Scale());
}
