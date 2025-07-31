#pragma once
#include <mutex>
#include <shared_mutex>
#include<memory>
#include <string>
class SpellDataResource;
class SpellData final
{
public:
	SpellData();
	~SpellData();
	std::string			Get_Spell_Name()const;
	SpellDataResource*	Get_Spell_Data_Resource()const;
	void				Set_Spell_Name(const std::string& in_Spell_Name);
	void				Release();
private:
	mutable std::shared_mutex SpellDatamutex;
	std::string Spell_Name;
	std::unique_ptr<SpellDataResource> Spell_Data_Resource;
};

