#include "EzrealDamage.h"
#include"../../DamageSpell/DamageSpell.h"
#include "../../../../Spells/SpellFlags/SpellFlags.h"
#include "../../../../ObjectComponent/Object.h"
#include"../../../../CombatStats/CombatStats.h"

// 이즈리얼 데미지 계산기 생성자 - 부모 클래스 DamageChamp 초기화
Ezrealdamage::Ezrealdamage(Object* source) : DamageChamp(source) {};

// 이즈리얼 스킬별 데미지 계산 함수
float Ezrealdamage::get_damage(Object* target, Slot spell_slot, int level, int stage, int stacks)
{
	// 해당 스킬 슬롯의 스펠 정보 가져오기
	DamageSpell* spell = get_spell(spell_slot);
	if (!spell || level == 0)return 0.0f; // 스펠이 없거나 레벨이 0이면 데미지 0

	// 소스 오브젝트의 전체 공격력과 주문력 가져오기
	float totalAD = get_src()->Get_CombatStats_Component().Get_total_AD();
	float totalAP = get_src()->Get_CombatStats_Component().Get_total_AP();
	float damage = 0.0f;

	// 스킬 슬롯에 따른 데미지 계산
	switch (spell_slot)
	{
	case Slot::Q: // Q 스킬: 신비한 화살
	{
		// 기본 데미지 + (1.3 * 총 공격력) + (0.15 * 총 주문력)
		damage = spell->base_damage[level - 1] + 1.3 * totalAD + .15 * totalAP;
		break;
	}
	case Slot::W: // W 스킬: 정수 흐름 (미구현)
	{
		// TODO: W 스킬 데미지 계산 구현 필요
		break;
	}
	case Slot::E: // E 스킬: 비전 이동 (미구현)
	{
		// TODO: E 스킬 데미지 계산 구현 필요
		break;
	}
	case Slot::R: // R 스킬: 정조준 일격
	{
		// 기본 데미지 + (1.0 * 총 공격력) + (0.9 * 총 주문력)
		damage = spell->base_damage[level - 1] + 1 * totalAD + .9 * totalAP;
		break;
	}

	default:
		return 0.0f; // 알 수 없는 스킬 슬롯
	}


	return damage;
}