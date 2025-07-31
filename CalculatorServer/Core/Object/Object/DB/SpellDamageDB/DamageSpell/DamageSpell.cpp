#include "DamageSpell.h"
#include "../../../ObjectComponent/Object.h"
#include "../../../Spells/SpellBook/SpellBook.h"
#include "../../../Spells/SpellFlags/SpellFlags.h"
#include "../../../Spells/SpellData/SpellData.h"
#include "../../../Spells/Spellslot/Spellslot.h"
#include "../../../Spells/SpellDataResource/SpellDataResource.h"

// DamageSpell 생성자 - 특정 스킬 슬롯의 기본 데미지 정보 초기화
DamageSpell::DamageSpell(Object* source, const Slot& spell_slot) : slot(spell_slot)
{
	// 소스 오브젝트에서 해당 스킬의 기본 데미지 벡터 가져오기
	// SpellBook -> Slot -> SpellData -> SpellDataResource -> BaseDamage 순으로 접근
	const std::vector<float> base_dmg_vector = source->Get_P_SpellBook()->get_slot(ToSlotindex(spell_slot))->Get_SpellData()->Get_Spell_Data_Resource()->Get_Spell_Base_Damage();

	// 기본 데미지 배열에 값 복사
	// base_dmg_vector[0]은 스킬 레벨 0 (사용하지 않음)을 의미하므로 인덱스 0부터 시작
	// 최대 4개 레벨까지만 복사 (일반 스킬은 1-5레벨, 궁극기는 1-3레벨)
	for (size_t i = 0; i < base_dmg_vector.size() && i < 4; i++)
	{
		base_damage[i] = base_dmg_vector[i]; // 레벨별 기본 데미지 저장
	}
}