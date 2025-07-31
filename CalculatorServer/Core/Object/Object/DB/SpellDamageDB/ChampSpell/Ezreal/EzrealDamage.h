#pragma once

#include <memory>
#include "../../DamageChamp/DamageChamp.h"

// 전방 선언
class Object;
enum class Slot : unsigned int;

// 이즈리얼 챔피언 전용 데미지 계산 클래스
// DamageChamp 기본 클래스를 상속받아 이즈리얼만의 스킬 데미지 공식 구현
class Ezrealdamage : public DamageChamp
{
public:
	// 생성자: 이즈리얼 오브젝트를 받아서 초기화
	Ezrealdamage(Object* source);

	// 순수 가상 함수 오버라이드: 이즈리얼 스킬별 데미지 계산
	// target: 데미지를 받을 대상 (현재 미사용)
	// spell_slot: 스킬 슬롯 (Q, W, E, R)
	// level: 스킬 레벨 (1-5, R은 1-3)
	// stage: 스킬 단계 (기본값 1, 다단계 스킬용)
	// stacks: 스택 수 (기본값 1, 스택형 스킬용)
	float get_damage(Object* target, Slot spell_slot, int level, int stage = 1, int stacks = 1);
};