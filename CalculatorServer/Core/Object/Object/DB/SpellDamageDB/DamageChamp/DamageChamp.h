#pragma once

#include <memory>

// 전방 선언
class DamageSpell;
class Object;
enum class Slot : unsigned int;

// 모든 챔피언 데미지 계산의 기본 클래스 (추상 클래스)
// 각 챔피언별로 상속받아서 고유한 데미지 계산 로직 구현
class DamageChamp
{
public:
	// 생성자: 챔피언 오브젝트를 받아서 4개 스킬 슬롯 초기화
	DamageChamp(Object* src);

	// 가상 소멸자: 상속 클래스의 올바른 소멸을 위해 필요
	~DamageChamp();

	// 순수 가상 함수: 각 챔피언에서 반드시 구현해야 하는 데미지 계산 함수
	// 챔피언별로 스킬 데미지 공식이 다르므로 추상 함수로 선언
	virtual float get_damage(Object* target, Slot spell_slot, int level, int stage = 1, int stacks = 1) = 0;

	// 특정 스킬 슬롯의 DamageSpell 객체 포인터 반환
	DamageSpell* get_spell(Slot slot_id) const;

	// 소스 오브젝트(챔피언) 포인터 반환
	Object* get_src();

private:
	Object* src;                          // 데미지를 주는 챔피언 오브젝트
	std::unique_ptr<DamageSpell> Q;       // Q 스킬 데이터 (스마트 포인터로 자동 메모리 관리)
	std::unique_ptr<DamageSpell> W;       // W 스킬 데이터
	std::unique_ptr<DamageSpell> E;       // E 스킬 데이터  
	std::unique_ptr<DamageSpell> R;       // R 스킬 데이터 (궁극기)
};