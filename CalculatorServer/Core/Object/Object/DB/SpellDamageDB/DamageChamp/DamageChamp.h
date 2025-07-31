#pragma once

#include <memory>

// ���� ����
class DamageSpell;
class Object;
enum class Slot : unsigned int;

// ��� è�Ǿ� ������ ����� �⺻ Ŭ���� (�߻� Ŭ����)
// �� è�Ǿ𺰷� ��ӹ޾Ƽ� ������ ������ ��� ���� ����
class DamageChamp
{
public:
	// ������: è�Ǿ� ������Ʈ�� �޾Ƽ� 4�� ��ų ���� �ʱ�ȭ
	DamageChamp(Object* src);

	// ���� �Ҹ���: ��� Ŭ������ �ùٸ� �Ҹ��� ���� �ʿ�
	~DamageChamp();

	// ���� ���� �Լ�: �� è�Ǿ𿡼� �ݵ�� �����ؾ� �ϴ� ������ ��� �Լ�
	// è�Ǿ𺰷� ��ų ������ ������ �ٸ��Ƿ� �߻� �Լ��� ����
	virtual float get_damage(Object* target, Slot spell_slot, int level, int stage = 1, int stacks = 1) = 0;

	// Ư�� ��ų ������ DamageSpell ��ü ������ ��ȯ
	DamageSpell* get_spell(Slot slot_id) const;

	// �ҽ� ������Ʈ(è�Ǿ�) ������ ��ȯ
	Object* get_src();

private:
	Object* src;                          // �������� �ִ� è�Ǿ� ������Ʈ
	std::unique_ptr<DamageSpell> Q;       // Q ��ų ������ (����Ʈ �����ͷ� �ڵ� �޸� ����)
	std::unique_ptr<DamageSpell> W;       // W ��ų ������
	std::unique_ptr<DamageSpell> E;       // E ��ų ������  
	std::unique_ptr<DamageSpell> R;       // R ��ų ������ (�ñر�)
};