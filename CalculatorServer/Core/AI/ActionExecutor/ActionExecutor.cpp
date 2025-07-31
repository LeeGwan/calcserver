#include "ActionExecutor.h"
#include "../../Object/Object/ObjectComponent/Object.h"
#include "../../Object/Object/Transform/Transform.h"
#include "../../utils/Vector.h"
#include "../AIflag/AIflags.h"
#include "../../../RoutineServer/Message/OptimizedPacketHandler/OptimizedPacketHandler.h"
#include "../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
#include "../../utils/Find_OBJ/Find_OBJ.h"
#include "../../Object/Object/Flags.h"
#include "../../Algorithm/Prediction/Prediction.h"
#include "../../Algorithm/Prediction/PredictionResult.h"
#include "../../Object/Object/Spells/SpellBook/SpellBook.h"
#include "../../Object/Object/TargetSelector/TargetSelector.h"
#include "../../Object/Object/Navgation/Navigation.h"
#include "../../Object/Object/Spells/SpellFlags/SpellFlags.h"
#include "../../Object/Object/VMT/VMT.h"
#include "../RLStructures/RLStructures.h"
#include <iostream>

ActionExecutor::ActionExecutor(unsigned int mapid) :mapid(mapid) {}

ActionExecutor::~ActionExecutor() = default;

bool ActionExecutor::execute_action(Object* ai_object, Object* target, AIAction action, const GameStateInfo& state, float search_radius)
{
	Vec3 SafePos;
	try {
		// 기본 유효성 검사
		if (!ai_object) {
			std::cerr << "[ActionExecutor] AI object is null" << std::endl;
			return false;
		}

		switch (action) {
		case AIAction::MOVE_NORTH:
		case AIAction::MOVE_SOUTH:
		case AIAction::MOVE_WEST:
		case AIAction::MOVE_EAST:
		case AIAction::MOVE_NORTHEAST:
		case AIAction::MOVE_NORTHWEST:
		case AIAction::MOVE_SOUTHEAST:
		case AIAction::MOVE_SOUTHWEST:
			return execute_movement(ai_object, action, search_radius);

		case AIAction::MOVE_TO_ENEMY:
			if (!target) {
				std::cerr << "[ActionExecutor] MOVE_TO_ENEMY: Target is null" << std::endl;
				return false;
			}
			return execute_move_to_enemy(ai_object, target);

		case AIAction::ATTACK_TARGET:
			if (!target) {
				std::cerr << "[ActionExecutor] ATTACK_TARGET: Target is null" << std::endl;
				return false;
			}
			if (target->Get_VMT_Component()->is_dead()) {
				std::cerr << "[ActionExecutor] ATTACK_TARGET: Target is dead" << std::endl;
				return false;
			}
			return execute_attack(ai_object, target);

		case AIAction::SKILL_Q:
			if (!target) {
				std::cerr << "[ActionExecutor] SKILL_Q: Target is null" << std::endl;
				return false;
			}
			return execute_skill(ai_object, target, Slot::Q);

		case AIAction::SKILL_E:
			// 수정: SafePos가 유효하지 않으면 false 반환 (논리 수정)
			SafePos = state.skill_status.forblink[2];
			if (!SafePos.is_valid()) {
				std::cerr << "[ActionExecutor] SKILL_E: Invalid safe position" << std::endl;
				return false;
			}
			return execute_blinkskill(ai_object, SafePos, Slot::E);

		case AIAction::SKILL_R:
			if (!target) {
				std::cerr << "[ActionExecutor] SKILL_R: Target is null" << std::endl;
				return false;
			}
			return execute_skill(ai_object, target, Slot::R);

		case AIAction::RETREAT_SAFE:
		{
			SafePos = state.safePos;
			if (!SafePos.is_valid()) {
				std::cerr << "[ActionExecutor] RETREAT_SAFE: Invalid safe position" << std::endl;
				return false;
			}

			return execute_retreat(ai_object, SafePos);
		}

		case AIAction::STAY_POSITION:
			return execute_stay(ai_object);

		case AIAction::FOLLOW_NAVIGATION:
			// 네비게이션 경로가 있는지 확인
			if (!ai_object->Get_Navigation_Component() ||
				ai_object->Get_Navigation_Component()->Get_NaviPath().empty()) {
				std::cerr << "[ActionExecutor] FOLLOW_NAVIGATION: No navigation path available" << std::endl;
				return false;
			}
			return execute_follow_navigation(ai_object);

		case AIAction::CANCEL_NAVIGATION:
			return execute_cancel_navigation(ai_object);

		default:
			std::cerr << "[ActionExecutor] Unknown action: " << static_cast<int>(action) << std::endl;
			return false;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "[ActionExecutor] Exception in execute_action: " << e.what() << std::endl;
		return false;
	}
}

bool ActionExecutor::execute_movement(Object* ai_object, AIAction action, float search_radius)
{
	int direction_index = static_cast<int>(action); // MOVE_NORTH = 0, MOVE_SOUTH = 1, ...
	if (direction_index < 0 || direction_index >= 8) return false;
	Vec3 current_pos = ai_object->Get_Transform_Component().Get_Position();
	Vec3 target_pos = current_pos.get_direction_offset(direction_index, search_radius);
	return send_move_command(ai_object, target_pos);
}

bool ActionExecutor::execute_move_to_enemy(Object* ai_object, Object* target)
{
	return send_attack_command(ai_object, target);
}

bool ActionExecutor::execute_attack(Object* ai_object, Object* target)
{
	return send_attack_command(ai_object, target);
}

bool ActionExecutor::execute_skill(Object* ai_object, Object* target, Slot skill_slot)
{
	// if()
	Vec3 aiPos = ai_object->Get_Transform_Component().Get_Position();
	Vec3 endPos;
	PredictionResult testpre;
	if (!target) return false;

	float range = ai_object->Get_P_SpellBook()->Get_spell_range(ToSlotindex(skill_slot));
	float missile_speed = ai_object->Get_P_SpellBook()->Get_spell_missilespeed(ToSlotindex(skill_slot));

	if (range == 0.0f && missile_speed == 0.0f) {
		return false;
	}

	testpre = Prediction::PredictMissileTarget(aiPos, target, missile_speed, range);//  moveTowards
	endPos = aiPos.moveTowards(testpre.predictedPosition, range);
	return send_skill_command(ai_object, target, skill_slot, endPos);
}

bool ActionExecutor::execute_blinkskill(Object* ai_object, const Vec3& pos, Slot skill_slot)
{
	return send_skill_command(ai_object, nullptr, skill_slot, pos);
}

bool ActionExecutor::execute_retreat(Object* ai_object, const Vec3& SafePos)
{
	return send_move_command(ai_object, SafePos);
}

bool ActionExecutor::execute_stay(Object* ai_object)
{

	
	ai_object->Get_objectState_Component()
		.Set_CurrentOrderType(Enums::CurrentOrderType::Idle);
	ai_object->Get_objectState_Component().Set_AttackTime(0.0f);
	ai_object->Get_TargetSelector_Component()->clear();
	ai_object->Get_Navigation_Component()->clear();

	return true;
}

bool ActionExecutor::execute_follow_navigation(Object* ai_object)
{
	return true;
}

bool ActionExecutor::execute_cancel_navigation(Object* ai_object)
{
	return execute_stay(ai_object);
}

bool ActionExecutor::send_move_command(Object* ai_object, const Vec3& target_pos)
{
	ObjectMoveOrderType moveEvent;
	moveEvent.mapid = mapid;
	moveEvent.Type = PacketType::Input;
	moveEvent.Move_INFOR.TargetObjectHash = "";
	moveEvent.Move_INFOR.ObjectHash = ai_object->Get_Hash();
	moveEvent.Move_INFOR.EndPos = target_pos;
	moveEvent.Move_INFOR.CurrentOrderType = Enums::CurrentOrderType::MoveTo;
	OptimizedPacketHandler::ProcessMovePacket(moveEvent);
	return true;
}

bool ActionExecutor::send_attack_command(Object* ai_object, Object* target)
{
	ObjectMoveOrderType moveEvent;
	moveEvent.mapid = mapid;
	moveEvent.Type = PacketType::Input;
	if (target)
	{
		moveEvent.Move_INFOR.TargetObjectHash = target->Get_Hash();
	}
	else
	{
		return false;
	}
	moveEvent.Move_INFOR.ObjectHash = ai_object->Get_Hash();

	moveEvent.Move_INFOR.EndPos = target->Get_Transform_Component().Get_Position();
	moveEvent.Move_INFOR.CurrentOrderType = Enums::CurrentOrderType::MoveTo;
	OptimizedPacketHandler::ProcessMovePacket(moveEvent);
	return true;
}

bool ActionExecutor::send_skill_command(Object* ai_object, Object* target, Slot skill_slot, const Vec3& target_pos)
{
	ObjectCastOrderType castEvent;
	castEvent.mapid = mapid;
	if (target)
	{
		castEvent.CastSpell_INFOR.TargetObjectHash = target->Get_Hash();
	}
	else
	{
		castEvent.CastSpell_INFOR.TargetObjectHash = "";
	}
	castEvent.Type = PacketType::CreateCastSpell;
	castEvent.CastSpell_INFOR.ObjectHash = ai_object->Get_Hash();

	castEvent.CastSpell_INFOR.EndPos = target_pos;
	castEvent.CastSpell_INFOR.CurrentOrderType = FromSlot(skill_slot);
	OptimizedPacketHandler::ProcessCastSpellPacket(castEvent);
	return true;
}

