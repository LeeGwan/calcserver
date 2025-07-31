#include "SpellLib.h"
#include"DB/ezreal_spell.h"
#include <algorithm>
void SpellDB::initialize()
{
	ezreal_spell::load(SpellDBList);
}

void SpellDB::Release()
{
	SpellDBList.clear();
}

bool SpellDB::Get_SpellDB(std::vector<Struct_SpellDB>& out, const std::string& hero_name)
{
	
	for (const auto& object : SpellDBList)
	{
		
		if (object->Get_HeroName() == hero_name)
		{
			out.push_back(*object);
		}
	}
	if(out.empty())return false;

	
	return true;
}
