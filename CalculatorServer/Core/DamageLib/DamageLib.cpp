#include "DamageLib.h"
#include "../Object/Object/ObjectComponent/Object.h"
#include "../Object/Object/CombatStats/CombatStats.h"
#include "../Object/Object/ObjectStats/objectStats.h"
#include "../Object/Object/objectState/objectState.h"
#include "../Object/Object/Flags.h"
#include "../Object/Object/DB/SpellDamageDB/ChampSpell/Ezreal/EzrealDamage.h"
#include <Windows.h>
#include "../Object/Object/TargetSelector/TargetSelector.h"
#include "../Object/Object/Navgation/Navigation.h"
#include "../Object/Object/VMT/VMT.h"
// 일반 공격 데미지 계산 (데미지 타입에 따른 분기)
std::mutex damage_mutex;
void DamageLib::compute_damage(Object* source, Object* target)
{
	Enums::DamageType damage_type = ToDamageType(source->Get_CombatStats_Component().Get_DamageType());
	switch (damage_type)
	{
	case Enums::DamageType::Raw:
		compute_Raw_damage(source, target);
		break;
	case Enums::DamageType::AD:
		compute_physical_damage(source, target);
		break;
	case Enums::DamageType::AP:
		compute_magical_damage(source, target);
		break;
	default:
		break;
	}
}

// 스킬 데미지 계산 (챔피언별 스킬 데미지 + 방어력 계산)
void DamageLib::compute_spell_damage(Object* source, Object* target, const Slot& slot, unsigned int skill_level)
{
	std::string name = source->Get_Name();
	float damage = 0.0f;

	// 챔피언별 스킬 데미지 계산
	if (!name.compare("ezreal"))
	{
		Ezrealdamage ezreal(source);
		damage = ezreal.get_damage(target, slot, skill_level);
	}

	if (damage == 0.0f) return;

	// 물리 데미지로 가정하여  방어력 적용 (스킬은 물리데미지)
	float totalarmor = target->Get_CombatStats_Component().Get_total_armor();
	float finalDamage = damage * (100.0f / (100.0f + totalarmor));

	// 최종 데미지 적용
	apply_damage_to_target(source, target, finalDamage);
}

// 물리 데미지 계산 (방어력 관통 적용)
void DamageLib::compute_physical_damage(Object* source, Object* target)
{
	int target_LV = target->Get_objectState_Component().Get_Level();

	// 1. 고정 방어력 관통 (치명타) - 레벨에 따라 스케일링
	float flatArmorPen = source->Get_CombatStats_Component().Get_lethality()
		* (0.6f + 0.4f * (float)target_LV / 18.f);

	// 2. 퍼센트 방어력 관통
	float multiArmorPen = source->Get_CombatStats_Component().Get_Armor_Penetration(); // ex: 0.3f == 30%

	// 3. 방어력 계산 (보너스 방어력에만 % 관통 적용)
	float totalArmor = target->Get_CombatStats_Component().Get_total_armor();
	float bonusArmor = target->Get_CombatStats_Component().Get_bonus_armor();
	float baseArmor = totalArmor - bonusArmor;

	// % 관통은 보너스 방어력에만 적용
	bonusArmor *= (1.f - multiArmorPen);

	// 고정 관통 적용
	float reducedArmor = (baseArmor + bonusArmor) - flatArmorPen;

	// 4. 데미지 감소율 계산
	float damageMultiplier = calculate_damage_reduction(reducedArmor);

	// 5. 최종 데미지 계산 및 적용
	float source_AD_damage = source->Get_CombatStats_Component().Get_total_AD();
	float finalDamage = source_AD_damage * damageMultiplier;

	apply_damage_to_target(source, target, finalDamage);
}

// 마법 데미지 계산 (TODO: 구현 필요)
void DamageLib::compute_magical_damage(Object* source, Object* target)
{
	// TODO: 마법 관통력과 마법 저항력을 고려한 마법 데미지 계산
}

// 고정 데미지 (방어력 무시)
void DamageLib::compute_Raw_damage(Object* source, Object* target)
{
	float source_Raw_damage = source->Get_CombatStats_Component().Get_total_AD();
	apply_damage_to_target(source, target, source_Raw_damage);
}

// 공통 함수: 데미지 감소율 계산
float DamageLib::calculate_damage_reduction(float armor)
{
	if (armor >= 0.f)
	{
		// 일반적인 데미지 감소 공식: 100 / (100 + 방어력)
		return 100.f / (100.f + armor);
	}
	else
	{
		// 음수 방어력일 때 (방어력 관통이 방어력보다 높을 때)
		// 데미지 증가 공식
		return 2.f - (100.f / (100.f - armor));
	}
}

// 공통 함수: 타겟에게 데미지 적용 및 죽음 처리
void DamageLib::apply_damage_to_target(Object* source, Object* target, float damage)
{
	// 현재 체력 가져오기
	{
		std::lock_guard<std::mutex> lock(damage_mutex);
		float currentHP = target->Get_ObjectStats_Component().Get_health();

		// 새로운 체력 계산 (최소 0)
		float newHP = (std::max)(0.f, currentHP - damage);
		target->Get_ObjectStats_Component().Set_health(newHP);


		if (target->Get_VMT_Component()->is_dead())return;
		// 죽음 처리
		if (newHP <= 0.f)
		{
			handle_target_death(source, target);
		//	OutputDebugStringA((target->Get_Hash() + " :사망\n").c_str());
		}
	}
}

// 공통 함수: 타겟 죽음 처리
void DamageLib::handle_target_death(Object* source, Object* target)
{
	int target_LV = target->Get_objectState_Component().Get_Level();
	float respawnTime = Get_respawn_time(target_LV);

	// 상태를 죽음으로 변경

	source->Get_TargetSelector_Component()->clear();
	source->Get_objectState_Component().Set_CurrentOrderType(Enums::CurrentOrderType::Idle);
	source->Get_objectState_Component().Set_AttackTime(0.0f);
	source->Get_Navigation_Component()->clear();

	target->Get_objectState_Component().Set_CurrentOrderType(Enums::CurrentOrderType::Dead);
	target->Get_TargetSelector_Component()->clear();
	target->Get_Navigation_Component()->clear();
	target->Get_objectState_Component().Set_DeadTime(respawnTime);
}

// 레벨에 따른 리스폰 시간 계산
float DamageLib::Get_respawn_time(const int& LV)
{
	return 1.0f;//2.0f + (LV * 2.5f);
}