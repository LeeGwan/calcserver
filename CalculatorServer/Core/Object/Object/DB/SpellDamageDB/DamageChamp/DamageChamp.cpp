#include "DamageChamp.h"
#include "../../../Spells/SpellFlags/SpellFlags.h"
#include "../DamageSpell/DamageSpell.h"

// ������ è�Ǿ� �⺻ Ŭ���� ������
DamageChamp::DamageChamp(Object* src) : src(src)
{
	// �� ��ų ���Կ� ���� DamageSpell ��ü ����
	Q = std::make_unique<DamageSpell>(src, Slot::Q); // Q ��ų
	W = std::make_unique<DamageSpell>(src, Slot::W); // W ��ų
	E = std::make_unique<DamageSpell>(src, Slot::E); // E ��ų
	R = std::make_unique<DamageSpell>(src, Slot::R); // R ��ų (�ñر�)
}

// �Ҹ��� - ����Ʈ �����Ͱ� �ڵ����� �޸� ����
DamageChamp::~DamageChamp() = default;

// ��ų ���Կ� �ش��ϴ� DamageSpell ������ ��ȯ
DamageSpell* DamageChamp::get_spell(Slot slot_id) const
{
	if (slot_id == Slot::Q)
		return Q.get(); // Q ��ų ��ȯ
	if (slot_id == Slot::W)
		return W.get(); // W ��ų ��ȯ
	if (slot_id == Slot::E)
		return E.get(); // E ��ų ��ȯ
	if (slot_id == Slot::R)
		return R.get(); // R ��ų ��ȯ

	return nullptr; // �߸��� ������ ��� nullptr ��ȯ
}

// �ҽ� ������Ʈ(è�Ǿ�) ������ ��ȯ
Object* DamageChamp::get_src()
{
	return src;
}