#pragma once

#include <memory>
#include "../../DamageChamp/DamageChamp.h"

// ���� ����
class Object;
enum class Slot : unsigned int;

// ����� è�Ǿ� ���� ������ ��� Ŭ����
// DamageChamp �⺻ Ŭ������ ��ӹ޾� ������� ��ų ������ ���� ����
class Ezrealdamage : public DamageChamp
{
public:
	// ������: ����� ������Ʈ�� �޾Ƽ� �ʱ�ȭ
	Ezrealdamage(Object* source);

	// ���� ���� �Լ� �������̵�: ����� ��ų�� ������ ���
	// target: �������� ���� ��� (���� �̻��)
	// spell_slot: ��ų ���� (Q, W, E, R)
	// level: ��ų ���� (1-5, R�� 1-3)
	// stage: ��ų �ܰ� (�⺻�� 1, �ٴܰ� ��ų��)
	// stacks: ���� �� (�⺻�� 1, ������ ��ų��)
	float get_damage(Object* target, Slot spell_slot, int level, int stage = 1, int stacks = 1);
};