#include "SpellData.h"
#include "../SpellDataResource/SpellDataResource.h"
SpellData::SpellData()
{
	Spell_Name = "";
	Spell_Data_Resource = std::make_unique<SpellDataResource>();
}
SpellData::~SpellData()= default;
std::string SpellData::Get_Spell_Name() const
{
	std::shared_lock Lock(SpellDatamutex);
	return Spell_Name;
}

SpellDataResource* SpellData::Get_Spell_Data_Resource() const
{
	std::shared_lock Lock(SpellDatamutex);
	return Spell_Data_Resource.get();
}

void SpellData::Set_Spell_Name(const std::string& in_Spell_Name)
{
	std::unique_lock Lock(SpellDatamutex);
	Spell_Name = in_Spell_Name;
}
void SpellData::Release()
{
	std::unique_lock Lock(SpellDatamutex);

}