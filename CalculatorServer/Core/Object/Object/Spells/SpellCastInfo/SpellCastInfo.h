#pragma once
#include <string>
enum class SpellType : unsigned int;
class Spellslot;
class SpellCastInfo final
{
	SpellType	Which_Spell_Type()const;
	Spellslot*	Get_Spell_slot()const;

	void		Set_Spell_slot(Spellslot* in_Spell_slot);
private:
	Spellslot* Spell_slot;
	
	
};

