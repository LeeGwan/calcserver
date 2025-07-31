#include "DamageChamp.h"
#include "../../../Spells/SpellFlags/SpellFlags.h"
#include "../DamageSpell/DamageSpell.h"

// 데미지 챔피언 기본 클래스 생성자
DamageChamp::DamageChamp(Object* src) : src(src)
{
	// 각 스킬 슬롯에 대한 DamageSpell 객체 생성
	Q = std::make_unique<DamageSpell>(src, Slot::Q); // Q 스킬
	W = std::make_unique<DamageSpell>(src, Slot::W); // W 스킬
	E = std::make_unique<DamageSpell>(src, Slot::E); // E 스킬
	R = std::make_unique<DamageSpell>(src, Slot::R); // R 스킬 (궁극기)
}

// 소멸자 - 스마트 포인터가 자동으로 메모리 해제
DamageChamp::~DamageChamp() = default;

// 스킬 슬롯에 해당하는 DamageSpell 포인터 반환
DamageSpell* DamageChamp::get_spell(Slot slot_id) const
{
	if (slot_id == Slot::Q)
		return Q.get(); // Q 스킬 반환
	if (slot_id == Slot::W)
		return W.get(); // W 스킬 반환
	if (slot_id == Slot::E)
		return E.get(); // E 스킬 반환
	if (slot_id == Slot::R)
		return R.get(); // R 스킬 반환

	return nullptr; // 잘못된 슬롯일 경우 nullptr 반환
}

// 소스 오브젝트(챔피언) 포인터 반환
Object* DamageChamp::get_src()
{
	return src;
}