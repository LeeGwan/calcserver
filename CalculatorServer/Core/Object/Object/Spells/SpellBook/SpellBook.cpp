#include "SpellBook.h"
#include "../../../../Error/ErrorClass.h"
#include "../../Flags.h"
#include "../../../../utils/Vector.h"
#include "../Spellslot/Spellslot.h"
#include "../../DB/SpellDB/Struct_SpellDB.h"
#include "../../ObjectComponent/Object.h"
#include "../SpellData/SpellData.h"
#include "../SpellDataResource/SpellDataResource.h"
#include "../SpellTargetClient/SpellTargetClient.h"
#include "../../../../Ability/SkillAbility/SkillAbility.h"
#include "../SpellFlags/SpellFlags.h"
#include "../../../../Error/is_valid.h"
#include "../SpellClient/MissileClient/MissileClient.h"
#include "../SpellClient/AttackClient/AttackClient.h"
#include "../SpellClient/BlinkClient/BlinkClient.h"
#include "../SpellClient/CircleClient/CircleClient.h"
#include "../../../../Algorithm/NavigationService.h"
#include "../SpellClient/LineClient/LineClient.h"
#include "../../../../Navmesh/NavMesh.h"
#include "../../../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
#include"../../../../Algorithm/SkillDetected/SkillDetected.h"
#include "../../../../AI/RLStructures/RLStructures.h"
SpellBook::SpellBook()
{
	// 모든 스펠 슬롯 초기화
	for (size_t i = 0; i < BASIC_SPELL_SLOTS; ++i)
	{
		spell_slots_[i] = std::make_unique<Spellslot>();
	}
}

SpellBook::~SpellBook() = default;


Spellslot* SpellBook::get_slot(uint8_t index)
{
	return is_valid_slot_index(index) ? spell_slots_[index].get() : nullptr;
}

const Spellslot* SpellBook::get_slot(uint8_t index) const
{
	return is_valid_slot_index(index) ? spell_slots_[index].get() : nullptr;
}

std::vector<float> SpellBook::get_slotDB_cooldown_time() const
{
	const Spellslot* slot;
	std::vector<float> cooldowntime;
	for (int i = 0; i < BASIC_SPELL_SLOTS; ++i)
	{
		slot = spell_slots_[i].get();
		int lv = slot->Get_SpellLv();
		cooldowntime.push_back(slot->Get_SpellData()->Get_Spell_Data_Resource()->Get_Spell_Base_CoolDownTime(slot->Get_SpellLv()));
	}
	if (cooldowntime.empty())return std::vector<float>();

	return cooldowntime;
}

float SpellBook::get_slotDB_cooldown_time(uint8_t index) const
{
	if (!is_valid_slot_index(index)) return 0.0f;

	const auto* slot = spell_slots_[index].get();
	return slot->Get_SpellData()->Get_Spell_Data_Resource()->Get_Spell_Base_CoolDownTime(slot->Get_SpellLv());
}

void SpellBook::import_from_database(const std::vector<Struct_SpellDB>& spell_db)
{
	// 기본 스펠 슬롯(Q,W,E,R)만 데이터베이스에서 로드
	const size_t import_count = (std::min)(spell_db.size(), BASIC_SPELL_SLOTS);

	for (size_t i = 0; i < import_count; ++i)
	{
		const auto& db_spell = spell_db[i];
		auto* slot = spell_slots_[i].get();
		auto* spell_data = slot->Get_SpellData();
		auto* spell_resource = spell_data->Get_Spell_Data_Resource();

		// 스펠 기본 정보 설정
		slot->Set_Spell_Slot_id(db_spell.Get_ID());
		slot->Set_SpellLv(1);  // 임시로 레벨 1 설정
		slot->Set_CoolDownTime(0.0f);

		// 스펠 데이터 설정
		spell_data->Set_Spell_Name(db_spell.Get_SpellName());

		// 스펠 리소스 데이터 설정
		spell_resource->Set_CastDelay(db_spell.Get_CastDelay());
		spell_resource->Set_Danger_Level(db_spell.Get_Danger_Level());
		spell_resource->Set_Get_Collision_Object(db_spell.Get_Collision_Object());
		spell_resource->Set_SpellType(db_spell.Get_Spell_Type());
		spell_resource->Set_Spell_Base_CastRadius(db_spell.Get_Spell_Base_CastRadius());
		spell_resource->Set_Spell_Base_CastRange(db_spell.Get_Spell_Base_CastRange());
		spell_resource->Set_Spell_Base_missile_speed(db_spell.Get_Spell_Base_missile_speed());
		spell_resource->Set_Spell_Base_CoolDownTime(db_spell.Get_Spell_Base_CoolDownTime());
		spell_resource->Set_Spell_Base_Damage(db_spell.Get_Spell_Base_Damage());
		spell_resource->Set_Spell_Base_Mana(db_spell.Get_Spell_Base_Mana());
	}
}

void SpellBook::Update_cooldowntime(const float& deltatime)
{
	for (const auto& slot : spell_slots_)
	{
		if (slot) // nullptr 체크
		{
			float cooltime = slot->Get_CoolDownTime();
			if (cooltime <= 0.0f) continue; // 이미 쿨다운이 끝났으면 스킵

			float newcooltime = cooltime - deltatime;

			// 0보다 작아지면 0으로 설정
			if (newcooltime <= 0.0f)
			{
				slot->Set_CoolDownTime(0.0f);

				// 쿨다운 완료 이벤트 (선택사항)
				// OnSpellCooldownComplete(slot);
			}
			else
			{
				slot->Set_CoolDownTime(newcooltime);
			}
		}
	}
}

ErrorCode::Spell_ErrorType SpellBook::try_prepare_spell_slot(
	Object* caster,
	Object* target,
	const CastSpellINFOR& cast_info,
	Spellslot*& out_spell_slot,
	Slot& out_slot,
	float delta_time) const
{
	// 스펠 타입에서 슬롯 인덱스 추출
	const uint8_t slot_index = get_slot_index_from_order_type(cast_info.CurrentOrderType, out_slot);
	if (slot_index == 255) // 유효하지 않은 인덱스
	{
		return ErrorCode::Spell_ErrorType::Spell_Error_Is_not_Spell;
	}

	// 스펠 상태 확인
	const SpellState state = get_spell_state(caster, slot_index);
	if (state != SpellState::Ready)
	{
		return convert_spell_state_to_error(state);
	}

	// 스펠 슬롯 할당
	out_spell_slot = spell_slots_[slot_index].get();
	if (!out_spell_slot)
	{
		return ErrorCode::Spell_ErrorType::Spell_Error_Cant_Find_SpellSlotList;
	}

	return ErrorCode::Spell_ErrorType::Spell_Successed;
}

ErrorCode::Spell_ErrorType SpellBook::create_spell_object(
	NavMesh* nav_mesh,
	SkillDetected*& m_SkillDetected,
	SkillAbility* cast_info,
	Object* caster,
	Object* target,
	const CastSpellINFOR& task,
	Spellslot* spell_slot,
	Slot slot,
	float delta_time) const
{
	if (!MemoryCheck::is_valid<SkillAbility>(cast_info))
	{
		return ErrorCode::Spell_Error_Dont_have_OBJ;
	}
	std::unique_lock lock(m_SkillDetectedMutex);
	{
		cast_info->Init_Ability(nav_mesh, m_SkillDetected, caster, target, task, spell_slot, slot, delta_time);
	}
	return ErrorCode::Spell_Successed;
}

ErrorCode::Spell_ErrorType SpellBook::process_spell_cast(
	const std::unordered_map<std::string, Object*>& objects,
	OutputComponent* m_outputComponent,
	int m_mapID,
	NavMesh* nav_mesh,
	SkillDetected*& m_SkillDetected,
	SkillAbility* cast_info,
	float delta_time,
	const float& Maptime) const
{
	const SpellType spell_type = cast_info->Get_Spell_Type();

	// 스펠 타입별 처리 (스택 객체 사용으로 성능 최적화)
	switch (spell_type)
	{
	case SpellType::Line:
	{


	}
	break;

	case SpellType::Missile:
	{
		MissileClient missile_client;
		std::unique_lock<std::shared_mutex> lock(m_SkillDetectedMutex);
		{
			missile_client.OnProcessSpell_Missile(objects, m_outputComponent, m_mapID, nav_mesh, m_SkillDetected, cast_info, delta_time, Maptime);
		}
	}
	break;

	case SpellType::Circle:
	{


	}
	break;

	case SpellType::Blink:
	{


	}
	break;

	case SpellType::Attack:
	{


	}
	break;

	default:
		return ErrorCode::Spell_ErrorType::Spell_Error_Unknown;
	}

	return ErrorCode::Spell_ErrorType::Spell_Successed;
}

ErrorCode::Spell_ErrorType SpellBook::delete_spell_object(
	NavMesh* nav_mesh,
	SkillDetected*& m_SkillDetected,
	SkillAbility*& cast_info) const
{
	if (!cast_info) return ErrorCode::Spell_ErrorType::Spell_Error_Unknown;

	// 스펠 상태 초기화
	std::unique_lock<std::shared_mutex> lock(m_SkillDetectedMutex);
	{
		m_SkillDetected->RemoveSkillEffect(nav_mesh->Get_LeagueNavMesh(), cast_info->Get_skillid());
	}
	cast_info->Release();

	return ErrorCode::Spell_ErrorType::Spell_DeleteOBJ_Successed;
}

void SpellBook::clear()
{
	// 모든 스펠 슬롯 해제 (unique_ptr이 자동으로 메모리 해제)
	for (auto& slot : spell_slots_)
	{
		slot.reset();
	}
}

SkillStatusInfo SpellBook::analyze_skill_status(NavMesh* navmesh, Object* ai_object, float search_radius)
{
	SkillStatusInfo skill_info;
	try {
		skill_info.q_ready = (get_spell_state(ai_object, 0) == SpellState::Ready) ? true : false;
		analyze_individual_skill_details(navmesh,ai_object, Slot::Q, search_radius, skill_info.q_cooldown_ratio,
			skill_info.q_mana_cost, skill_info.has_enough_mana_q, skill_info.q_level, skill_info.forblink[0], skill_info.skilltype[0]);
		// E 스킬 분석 (Slot::E = 2)
		skill_info.e_ready = (get_spell_state(ai_object, 2) == SpellState::Ready) ? true : false;
		analyze_individual_skill_details(navmesh, ai_object, Slot::E, search_radius, skill_info.e_cooldown_ratio,
			skill_info.e_mana_cost, skill_info.has_enough_mana_e, skill_info.e_level, skill_info.forblink[2], skill_info.skilltype[2]);
	

		// R 스킬 분석 (Slot::R = 3)
		skill_info.r_ready = (get_spell_state(ai_object, 3) == SpellState::Ready) ? true : false;
		analyze_individual_skill_details(navmesh, ai_object, Slot::R, search_radius, skill_info.r_cooldown_ratio,
			skill_info.r_mana_cost, skill_info.has_enough_mana_r, skill_info.r_level, skill_info.forblink[3],skill_info.skilltype[3]);
	}

	catch (const std::exception& e) {
		std::cerr << "[RL] Error analyzing skills: " << e.what() << std::endl;
	}
	return skill_info;
}

float SpellBook::Get_spell_range(uint8_t index)
{
	const Spellslot* slot = get_slot(index);
	if (!slot)return 0.0f;
	return slot->Get_SpellData()->Get_Spell_Data_Resource()->Get_Spell_Base_CastRange();
}

float SpellBook::Get_spell_missilespeed(uint8_t index)
{
	const Spellslot* slot = get_slot(index);
	if (!slot)return 0.0f;
	return slot->Get_SpellData()->Get_Spell_Data_Resource()->Get_Spell_Base_missile_speed();
}

// Private 헬퍼 메서드들
SpellState SpellBook::get_spell_state(Object* caster, uint8_t index) const
{
	if (!is_valid_slot_index(index)) return SpellState::Unknown;

	const auto* slot = spell_slots_[index].get();
	const uint32_t spell_level = slot->Get_SpellLv();

	// 스펠을 배우지 않았음
	if (spell_level == 0) return SpellState::NotLearned;

	// 쿨다운 중
	if (slot->Get_CoolDownTime() > 0.0f) return SpellState::Cooldown;

	// 마나 부족 체크
	const float required_mana = slot->Get_SpellData()->Get_Spell_Data_Resource()->Get_Spell_Base_Mana(spell_level);
	const float current_mana = caster->Get_ObjectStats_Component().Get_mana();
	if (current_mana < required_mana) return SpellState::NoMana;

	return SpellState::Ready;
}

ErrorCode::Spell_ErrorType SpellBook::convert_spell_state_to_error(SpellState spell_state) const
{
	switch (spell_state)
	{
	case SpellState::Unknown:     return ErrorCode::Spell_ErrorType::Spell_Error_Unknown;
	case SpellState::CantSpell:   return ErrorCode::Spell_ErrorType::Spell_Error_CantSpell;
	case SpellState::Supressed:   return ErrorCode::Spell_ErrorType::Spell_Error_Supressed;
	case SpellState::NotLearned:  return ErrorCode::Spell_ErrorType::Spell_Error_NotLearned;
	case SpellState::Channeling:  return ErrorCode::Spell_ErrorType::Spell_Error_Channeling;
	case SpellState::Cooldown:    return ErrorCode::Spell_ErrorType::Spell_Error_Cooldown;
	case SpellState::NoMana:      return ErrorCode::Spell_ErrorType::Spell_Error_NoMana;
	default:                      return ErrorCode::Spell_ErrorType::Unkown;
	}
}

uint8_t SpellBook::get_slot_index_from_order_type(Enums::CurrentOrderType order_type, Slot& out_slot) const
{
	switch (order_type)
	{
	case Enums::CurrentOrderType::Spell_Q: out_slot = Slot::Q; return 0;
	case Enums::CurrentOrderType::Spell_W: out_slot = Slot::W; return 1;
	case Enums::CurrentOrderType::Spell_E: out_slot = Slot::E; return 2;
	case Enums::CurrentOrderType::Spell_R: out_slot = Slot::R; return 3;
	default: return 255; // 유효하지 않은 값
	}
}

bool SpellBook::is_valid_slot_index(uint8_t index) const
{
	return index < BASIC_SPELL_SLOTS;
}

void SpellBook::analyze_individual_skill_details(NavMesh* navmesh, Object* caster, Slot skill_slot,float search_radius, float& cooldown_ratio, float& mana_cost, bool& has_enough_mana, unsigned int& skill_level,Vec3& blinkpos,SpellType& spelltype)
{
	uint8_t slot_index = ToSlotindex(skill_slot);
	const Spellslot* slot = get_slot(slot_index);
	if (!slot) {
		cooldown_ratio = 1.0f;
		mana_cost = 0.0f;
		has_enough_mana = false;
		skill_level = 0;
		return;
	}
	
	skill_level = slot->Get_SpellLv();
	if (skill_level == 0) {
		cooldown_ratio = 1.0f;
		mana_cost = 0.0f;
		has_enough_mana = false;
		return;
	}

	float current_cooldown = slot->Get_CoolDownTime();
	float max_cooldown = get_slotDB_cooldown_time(slot_index);
	cooldown_ratio = (max_cooldown > 0) ? (current_cooldown / max_cooldown) : 0.0f;
	cooldown_ratio = std::min(cooldown_ratio, 1.0f);
	try {
		const SpellData* spell_data = slot->Get_SpellData();
		if (spell_data && spell_data->Get_Spell_Data_Resource()) {
			mana_cost = spell_data->Get_Spell_Data_Resource()->Get_Spell_Base_Mana(skill_level);
			float current_mana = caster->Get_ObjectStats_Component().Get_mana();
			has_enough_mana = (current_mana >= mana_cost);
			spelltype = slot->Get_spelltype();
			if(spelltype== SpellType::Blink)
			blinkpos = findgoodpos_for_blink(navmesh, caster, search_radius, slot_index);
		}
		else {
			mana_cost = 0.0f;
			has_enough_mana = true;
		}
	}
	catch (...) {
		mana_cost = 0.0f;
		has_enough_mana = true;
	}
}
Vec3 SpellBook::findgoodpos_for_blink(NavMesh* navmesh, Object* object, float search_radius, uint8_t slot_index)
{
	unsigned int playerteam = object->Get_Team();
	Vec3 currentPos = object->Get_Transform_Component().Get_Position();
	float skillrange = object->Get_P_SpellBook()->Get_spell_range(slot_index) - 50.0f;
	Vec3 can_skill_range(skillrange, 0.0f, skillrange);

	return navmesh->Get_NavigationService()->FindsafePos(navmesh->Get_LeagueNavMesh(), navmesh->Get_navQuery(), currentPos, can_skill_range,0.0f, playerteam);
}
