#pragma once

#include <vector>

// ���� ����
class Object;
enum class Slot : unsigned int;

// ���� ��ų�� ������ ������ ��� Ŭ����
// ��ų�� �⺻ ������, ���� ���� ���� ����
class DamageSpell
{
public:
	// ������: �ҽ� ������Ʈ�� ��ų ������ �޾Ƽ� �ش� ��ų�� �⺻ ������ ���� �ε�
	DamageSpell(Object* source, const Slot& spell_slot);

	Slot slot;                           // �� ������ ���� ��ų ���� (Q, W, E, R)
	float base_damage[6] = { 0, 0, 0, 0, 0, 0 }; // ������ �⺻ ������ �迭
	// [0]: ���� 1, [1]: ���� 2, ... [5]: ���� 6
	// �Ϲ� ��ų�� 1-5����, �ñر�� 1-3���� ���
};