#include "SpellCastInfo.h"
#include "../Spellslot/Spellslot.h"
#include "../SpellData/SpellData.h"
#include "../SpellDataResource/SpellDataResource.h"
#include "../SpellFlags/SpellFlags.h"
SpellType SpellCastInfo::Which_Spell_Type() const
{
	if (!Spell_slot)return SpellType::None;

	return Spell_slot->Get_SpellData()->Get_Spell_Data_Resource()->Get_SpellType();
}

Spellslot* SpellCastInfo::Get_Spell_slot() const
{
	return Spell_slot;
}



void SpellCastInfo::Set_Spell_slot(Spellslot* in_Spell_slot)
{
	Spell_slot = in_Spell_slot;
}
