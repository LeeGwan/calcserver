#include "SkillAbility.h"
#include "../../Object/Object/ObjectComponent/Object.h"
#include "../../Object/Object/Spells/Spellslot/Spellslot.h"
#include "../../Object/Object/Spells/SpellFlags/SpellFlags.h"
#include "../../Error/is_valid.h"
#include "../../Object/Object/objectStats/objectStats.h"
#include "../../Object/Object/CharacterMesh/CharacterMesh.h"
#include "../../Algorithm/NavigationService.h"
#include "../../Navmesh/NavMesh.h"
#include"../../utils/Find_OBJ/Find_OBJ.h"
#include"../../Algorithm/SkillDetected/SkillDetected.h"
#include "../../Object/Object/Spells/SpellClient/MissileClient/MissileClient.h"
#include "../../Object/Object/Spells/SpellClient/BlinkClient/BlinkClient.h"
#include "../../Object/Object/Spells/SpellDataResource/SpellDataResource.h"
#include"../../Object/Object/Spells/SpellTargetClient/SpellTargetClient.h"
#include"../../Object/Object/Spells/SpellData/SpellData.h"
#include "../../Algorithm/Prediction/PredictionResult.h"
#include "../../Algorithm/Prediction/Prediction.h"
#include "../../Object/Object/Navgation/Navigation.h"
#include "../../Object/Object/VMT/VMT.h"
#include <Windows.h>
// ==========================================================================
// Ability 인터페이스 구현
// ==========================================================================

SkillAbility::SkillAbility()
{
}

bool SkillAbility::can_use_Ability()
{
	std::shared_lock lock(SkillAbility_mtx);
	return use_obj;
}

void SkillAbility::Set_use_Ability(bool in_use_obj)
{
	std::unique_lock lock(SkillAbility_mtx);
	use_obj = in_use_obj;
}

void SkillAbility::Release()
{
	std::unique_lock lock(SkillAbility_mtx);
	Cast_Delay = 0.0f;
	this->use_obj = true;
}

// ==========================================================================
// 스킬 초기화
// ==========================================================================

void SkillAbility::Init_Ability(NavMesh* Nav_Mesh, SkillDetected*& m_SkillDetected, Object* this_hero, Object* Target_hero,
	const CastSpellINFOR& task, Spellslot* in_Spell_slot,
	const Slot& in_Slot_Id, const float& Deltatime)
{
	// 스킬 데이터 가져오기
	unsigned int Spell_Lv = in_Spell_slot->Get_SpellLv();

	SpellDataResource* SpellData = in_Spell_slot->Get_SpellData()->Get_Spell_Data_Resource();
	CollisionObject collisionObj = SpellData->Get_Collision_Object();
	PolyFlags Team;
	if (this_hero->Get_Team() == 1)
	{
		Team = PolyFlags::Team1;
	}
	else
	{
		Team = PolyFlags::Team2;
	}
	// 스킬 기본 정보 설정
	PolyFlags Danger_Level = FromDangerLevel(SpellData->Get_Danger_Level()) | Team;

	SpellTargetClient* Target_Client = in_Spell_slot->Get_Target_Client();

	// 스킬 스탯 계산
	float CoolDownTime = SpellData->Get_Spell_Base_CoolDownTime(Spell_Lv);
	float Mana = SpellData->Get_Spell_Base_Mana(Spell_Lv);
	float CastRange = SpellData->Get_Spell_Base_CastRange();
	float radius = SpellData->Get_Spell_Base_CastRadius();
	float missile_speed = SpellData->Get_Spell_Base_missile_speed();
	float castdelay = SpellData->Get_CastDelay();
	// 캐스팅 지연 시간 설정
	Set_Cast_Delay(castdelay);

	// 스킬 타입 및 위치 정보 설정
	SpellType Spell_Type = SpellData->Get_SpellType();
	Vec3 Start_POS = this_hero->Get_Transform_Component().Get_Position();

	// 타겟 클라이언트 설정
	Target_Client->Set_CasterObject(this_hero);
	Target_Client->Set_Start_Position(Start_POS);


	// 쿨다운 및 마나 소모 적용
	in_Spell_slot->Set_CoolDownTime(CoolDownTime);
	this_hero->Get_ObjectStats_Component().Set_mana(
		this_hero->Get_ObjectStats_Component().Get_mana() - Mana);
	// 위치 및 슬롯 정보 설정
	Set_StartPosion(in_Spell_slot->Get_Target_Client()->Get_Start_Position());

	Set_Slot_Id(in_Slot_Id);
	Set_Spell_Type(Spell_Type);
	Set_P_Spell_Slot(in_Spell_slot);

	// 스킬 정보 설정
	Set_Hash(this_hero->Get_Hash());
	//캐릭터 사이즈
	Vec3 character_size = this_hero->Get_CharacterMesh_Component().Get_CapsuleSIze();
	// 타겟이 있는 경우 타겟 정보 설정
	if (Target_hero && MemoryCheck::is_valid<Object>(Target_hero))
	{
		Target_Client->Set_TargetObject(Target_hero);
		Set_TargetHash(Target_hero->Get_Hash());
	}



	// 스킬 타입별 영역 계산
	if (Is_Spell())
	{
		switch (Spell_Type)
		{

		case SpellType::Missile:
		{
			MissileClient missile;
			if (!this_hero->Get_VMT_Component()->is_ai())
			{
				Vec3 endPos;
				endPos = Start_POS.moveTowards(task.EndPos, CastRange);
				Target_Client->Set_End_Position(endPos);
				Set_EndPosion(endPos);
				missile.InitSpell_Missile(Nav_Mesh, m_SkillDetected, Start_POS, endPos, radius, missile_speed, castdelay, Danger_Level, skillid, To_unsignedlong(collisionObj));
			}
			else
			{

				Target_Client->Set_End_Position(task.EndPos);
				Set_EndPosion(task.EndPos);
				missile.InitSpell_Missile(Nav_Mesh, m_SkillDetected, Start_POS, task.EndPos, radius, missile_speed, castdelay, Danger_Level, skillid, To_unsignedlong(collisionObj));
			}


			break;
		}
		case SpellType::Blink:
		{
			BlinkClient Blink;
			Vec3 Nav_startPos = Start_POS.to_Nav_POS();
			Vec3 Nav_endPos = task.EndPos.to_Nav_POS();
			Vec3 Nav_character_size(character_size.x * 0.01, character_size.z * 0.01, character_size.y * 0.01);

			Vec3 endPos = Nav_Mesh->Get_NavigationService()->GetCheckEndPosition(Nav_Mesh->Get_navQuery(), Nav_startPos, Nav_endPos, Nav_character_size).to_Unreal_POS();
			Vec3 limitedEndPos = Start_POS.clamp_range_2D(endPos, CastRange);
			Blink.OnProcessSpellBlink(this_hero, limitedEndPos);
			Target_Client->Set_End_Position(endPos);
			Set_EndPosion(endPos);


			break;
		}
		default:
			return;
		}
	}

}

// ==========================================================================
// 스킬 타입 확인 함수들
// ==========================================================================

bool SkillAbility::Is_Spell()
{
	const auto Check_Spell_Type = Get_Spell_Type();
	return (Check_Spell_Type != SpellType::Attack) &&
		(Check_Spell_Type != SpellType::None);
}

bool SkillAbility::Is_Line()
{
	return Get_Spell_Type() == SpellType::Line;
}

bool SkillAbility::Is_Missile()
{
	return Get_Spell_Type() == SpellType::Missile;
}

bool SkillAbility::Is_Circle()
{
	return Get_Spell_Type() == SpellType::Circle;
}

bool SkillAbility::Is_Blink()
{
	return Get_Spell_Type() == SpellType::Blink;
}

bool SkillAbility::Is_Attack()
{
	return Get_Spell_Type() == SpellType::Attack;
}

// ==========================================================================
// Getter 함수들 (스레드 안전)
// ==========================================================================



std::string SkillAbility::Get_Hash() const
{
	std::shared_lock lock(SkillAbility_mtx);
	return Hash;
}

std::string SkillAbility::Get_TargetHash() const
{
	std::shared_lock lock(SkillAbility_mtx);
	return TargetHash;
}

Vec3 SkillAbility::Get_StartPosion() const
{
	std::shared_lock lock(SkillAbility_mtx);
	return StartPosion;
}

Vec3 SkillAbility::Get_EndPosion() const
{
	std::shared_lock lock(SkillAbility_mtx);
	return EndPosion;
}

Slot SkillAbility::Get_Slot_Id() const
{
	std::shared_lock lock(SkillAbility_mtx);
	return Slot_Id;
}

SpellType SkillAbility::Get_Spell_Type() const
{
	std::shared_lock lock(SkillAbility_mtx);
	return Spell_Type;
}

Spellslot* SkillAbility::Get_P_Spell_Slot()
{
	std::shared_lock lock(SkillAbility_mtx);
	return P_Spell_Slot;
}

float SkillAbility::Get_SpellDuration_Time() const
{
	std::shared_lock lock(SkillAbility_mtx);
	return SpellDuration;
}

float SkillAbility::Get_Cast_Delay() const
{
	std::shared_lock lock(SkillAbility_mtx);
	return Cast_Delay;
}



// ==========================================================================
// Setter 함수들 (스레드 안전)
// ==========================================================================

void SkillAbility::Set_SpellDuration_Time(const float& in_SpellDuration)
{
	std::unique_lock lock(SkillAbility_mtx);
	SpellDuration = in_SpellDuration;
}

void SkillAbility::Set_Cast_Delay(const float& in_Cast_Delay)
{
	std::unique_lock lock(SkillAbility_mtx);
	Cast_Delay = in_Cast_Delay;
}

void SkillAbility::Set_Hash(const std::string& in_hash)
{
	std::unique_lock lock(SkillAbility_mtx);
	Hash = in_hash;
}

void SkillAbility::Set_TargetHash(const std::string& in_TargetHash)
{
	std::unique_lock lock(SkillAbility_mtx);
	TargetHash = in_TargetHash;
}

void SkillAbility::Set_StartPosion(const Vec3& in_StartPosion)
{
	std::unique_lock lock(SkillAbility_mtx);
	StartPosion = in_StartPosion;
}

void SkillAbility::Set_EndPosion(const Vec3& in_EndPosion)
{
	std::unique_lock lock(SkillAbility_mtx);
	EndPosion = in_EndPosion;
}

void SkillAbility::Set_Slot_Id(const Slot& in_Slot_Id)
{
	std::unique_lock lock(SkillAbility_mtx);
	Slot_Id = in_Slot_Id;
}

void SkillAbility::Set_Spell_Type(const SpellType& in_Spell_Type)
{
	std::unique_lock lock(SkillAbility_mtx);
	Spell_Type = in_Spell_Type;
}

void SkillAbility::Set_P_Spell_Slot(Spellslot* in_P_Spell_Slot)
{
	std::unique_lock lock(SkillAbility_mtx);
	P_Spell_Slot = in_P_Spell_Slot;
}



unsigned int SkillAbility::Get_skillid() const
{
	std::shared_lock lock(SkillAbility_mtx);
	return skillid;
}

void SkillAbility::Set_Skillid(const unsigned int& inskillid)
{
	std::unique_lock lock(SkillAbility_mtx);
	skillid = inskillid;
}
