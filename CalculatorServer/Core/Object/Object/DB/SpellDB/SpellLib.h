#pragma once
#include "Struct_SpellDB.h"
#include <memory>

namespace SpellDB
{
	
	inline std::vector<std::unique_ptr<Struct_SpellDB>> SpellDBList;
	void initialize();
	void Release();
	bool Get_SpellDB(std::vector<Struct_SpellDB>& out, const std::string& hero_name);
}
