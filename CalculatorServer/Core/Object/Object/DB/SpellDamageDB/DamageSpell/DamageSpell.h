#pragma once

#include <vector>

// 전방 선언
class Object;
enum class Slot : unsigned int;

// 개별 스킬의 데미지 정보를 담는 클래스
// 스킬의 기본 데미지, 슬롯 정보 등을 저장
class DamageSpell
{
public:
	// 생성자: 소스 오브젝트와 스킬 슬롯을 받아서 해당 스킬의 기본 데미지 정보 로드
	DamageSpell(Object* source, const Slot& spell_slot);

	Slot slot;                           // 이 스펠이 속한 스킬 슬롯 (Q, W, E, R)
	float base_damage[6] = { 0, 0, 0, 0, 0, 0 }; // 레벨별 기본 데미지 배열
	// [0]: 레벨 1, [1]: 레벨 2, ... [5]: 레벨 6
	// 일반 스킬은 1-5레벨, 궁극기는 1-3레벨 사용
};