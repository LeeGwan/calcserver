#include "ObjectLib.h"
#include "DB/Nexus.h"
#include "DB/Turret.h"
#include "DB/Ezreal.h"
#include "DB/Practice_Tool.h"
void ObjectDB::initialize()
{
	Nexus::load(ObjectDBList);
	Turret::load(ObjectDBList);
	Ezreal::load(ObjectDBList);
	Practice_Tool::load(ObjectDBList);
}

void ObjectDB::Release()
{
	ObjectDBList.clear();
}

bool ObjectDB::Get_ObjectDB(Struct_ObjectDB& out, const std::string& obj_name)
{

	for (const auto& object : ObjectDBList)
	{

		if (object->heroname == obj_name)
		{
			out = *object;

			return true;
		}
	}
	return false;
}
