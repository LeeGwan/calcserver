#include "DamageSpell.h"
#include "../../../ObjectComponent/Object.h"
#include "../../../Spells/SpellBook/SpellBook.h"
#include "../../../Spells/SpellFlags/SpellFlags.h"
#include "../../../Spells/SpellData/SpellData.h"
#include "../../../Spells/Spellslot/Spellslot.h"
#include "../../../Spells/SpellDataResource/SpellDataResource.h"

// DamageSpell ������ - Ư�� ��ų ������ �⺻ ������ ���� �ʱ�ȭ
DamageSpell::DamageSpell(Object* source, const Slot& spell_slot) : slot(spell_slot)
{
	// �ҽ� ������Ʈ���� �ش� ��ų�� �⺻ ������ ���� ��������
	// SpellBook -> Slot -> SpellData -> SpellDataResource -> BaseDamage ������ ����
	const std::vector<float> base_dmg_vector = source->Get_P_SpellBook()->get_slot(ToSlotindex(spell_slot))->Get_SpellData()->Get_Spell_Data_Resource()->Get_Spell_Base_Damage();

	// �⺻ ������ �迭�� �� ����
	// base_dmg_vector[0]�� ��ų ���� 0 (������� ����)�� �ǹ��ϹǷ� �ε��� 0���� ����
	// �ִ� 4�� ���������� ���� (�Ϲ� ��ų�� 1-5����, �ñر�� 1-3����)
	for (size_t i = 0; i < base_dmg_vector.size() && i < 4; i++)
	{
		base_damage[i] = base_dmg_vector[i]; // ������ �⺻ ������ ����
	}
}