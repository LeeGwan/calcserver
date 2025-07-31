#include "GameStateAnalyzer.h"
#include"../RLStructures/RLStructures.h"
#include "../../Object/Object/ObjectComponent/Object.h"
#include "../../Object/Object/objectStats/objectStats.h"
#include "../../Object/Object/Transform/Transform.h"
#include "../../Object/Object/VMT/VMT.h"
#include "../../utils/Vector.h"
#include "../../Object/Object/objectState/objectState.h"
#include "../../utils/Find_OBJ/Find_OBJ.h"
#include "../../Object/Object/Spells/SpellBook/SpellBook.h"
#include "../NavigationAnalyzer/NavigationAnalyzer.h"
#include "../../Navmesh/NavMesh.h"
#include "../../Algorithm/NavigationService.h"
#include "../AIflag/AIflags.h"
#include "../../Object/Object/CharacterMesh/CharacterMesh.h"
#include <algorithm>
GameStateAnalyzer::GameStateAnalyzer()
{
	navigation_analyzer = std::make_unique<NavigationAnalyzer>();
}

GameStateAnalyzer::~GameStateAnalyzer() = default;

GameStateInfo GameStateAnalyzer::extract_game_state(NavMesh* navmesh, Object* ai_object, const std::unordered_map<std::string, Object*>& all_objects, float deltatime, float maptime)
{
	float base_buffer = 0.05f;      // 최소 5cm 버퍼
	float ratio_buffer = 1.2f;      // 120% 비율
	float min_radius = 0.1f;        // 최소 반경
	float max_radius = 1.0f;        // 최대 반경
	float search_radius = 0.0f;
	float last_combat_time = 0.0f;
	GameStateInfo state;
	Object* closest_enemy = nullptr;

	//  NEW: 플레이어 위치 설정 추가
	state.player_position = ai_object->Get_Transform_Component().Get_Position();

	auto& stats = ai_object->Get_ObjectStats_Component();
	auto& statecomponent = ai_object->Get_objectState_Component();
	state.player_hp_ratio = stats.Get_health() / (std::max)(stats.Get_maxhelth(), 1.0f);
	state.player_mp_ratio = stats.Get_mana() / (std::max)(stats.Get_maxmana(), 1.0f);
	state.player_attack_range = stats.Get_total_Attack_range();
	state.player_movement_speed = stats.Get_total_MovementSpeed();

	float movement_speed = state.player_movement_speed;
	float distance_this_frame_cm = movement_speed * deltatime;
	search_radius = (distance_this_frame_cm * ratio_buffer) + base_buffer;
	search_radius = (std::max)(search_radius, min_radius);
	search_radius = (std::min)(search_radius, max_radius);
	state.search_radius = search_radius;
	state.player_team = ai_object->Get_Team();

	//  NEW: 적 찾기 시 더 엄격한 검증
	if (Find_OBJ::Find_Pos_is_near_Enemy(ai_object, closest_enemy, all_objects))
	{
		//  적이 실제로 살아있는지 확인
		if (closest_enemy &&
			!closest_enemy->Get_VMT_Component()->is_dead()) {

			state.enemy_hash = closest_enemy->Get_Hash();
			state.enemy_position = closest_enemy->Get_Transform_Component().Get_Position();
			auto& enemy_stats = closest_enemy->Get_ObjectStats_Component();
			state.enemy_hp_ratio = enemy_stats.Get_health() / std::max(enemy_stats.Get_maxhelth(), 1.0f);
			state.distance_to_enemy = state.player_position.distance(state.enemy_position);
			state.has_enemy_target = true;
		
			 //  NEW: HP가 0 이하인 적은 유효하지 않은 것으로 처리
			if (state.enemy_hp_ratio <= 0.0f) {
				state.has_enemy_target = false;
				state.can_attack_enemy = false;
				state.in_enemy_attack_range = false;
			}
			else {

				state.can_attack_enemy = ((state.distance_to_enemy <= state.player_attack_range)&& (statecomponent.Get_AttackTime() <= maptime));
				float enemy_attack_range = enemy_stats.Get_total_Attack_range();
				state.in_enemy_attack_range = (state.distance_to_enemy <= enemy_attack_range * 1.2f);
			}
		}
		else {
			// 죽은 적이거나 유효하지 않은 적
			state.has_enemy_target = false;
		}
	}
	else {
		state.has_enemy_target = false;
	}

	//  NEW: has_enemy_target이 false일 때 기본값 설정
	if (!state.has_enemy_target) {
		state.distance_to_enemy = 10000.0f;
		state.can_attack_enemy = false;
		state.in_enemy_attack_range = false;
		state.enemy_position = state.player_position;
		state.enemy_hp_ratio = 0.0f; //  죽은 적임을 명확히 표시
	}

	state.skill_status = ai_object->Get_P_SpellBook()->analyze_skill_status(navmesh,ai_object, state.search_radius);

	state.navigation_status = navigation_analyzer->analyze_navigation_status(navmesh, ai_object, deltatime);
	analyze_navmesh_state(navmesh, state, ai_object, search_radius);

	//공격사거리 *2 감지
	state.enemy_count_nearby = Find_OBJ::count_nearby_enemies(ai_object, all_objects, state.player_attack_range * 2.0f);
	state.is_near_ally = Find_OBJ::check_near_ally(ai_object, all_objects);//아군 
	last_combat_time = ai_object->Get_CombatStats_Component().Get_last_combat_time();
	state.time_since_last_combat = maptime- last_combat_time;

	return state;
}

void GameStateAnalyzer::analyze_navmesh_state(NavMesh* navmesh, GameStateInfo& state, Object* ai_object, float search_radius)
{
	Vec3 current_pos = ai_object->Get_Transform_Component().Get_Position();
	float Hitbox = ai_object->Get_CharacterMesh_Component().Get_CapsuleSIze().x;
	int team = ai_object->Get_Team();
	std::vector<safeposINFO> check;
	state.current_tile = navmesh->Get_NavigationService()->analyze_tile_at_position(navmesh->Get_LeagueNavMesh(),
		navmesh->Get_navQuery(),
		current_pos, Hitbox, team);
	navmesh->Get_NavigationService()->analyze_surrounding_tiles(navmesh->Get_LeagueNavMesh(), navmesh->Get_navQuery(),
		current_pos, search_radius, Hitbox, team, state.surrounding_tiles);

	navmesh->Get_NavigationService()->calculate_direction_safety(navmesh->Get_LeagueNavMesh(), navmesh->Get_navQuery(),
		current_pos, search_radius, Hitbox, team,
		state.best_direction_safety,
		state.direction_blocked);
	if (!navmesh->Get_NavigationService()->Find_Safe_Direction_Recursively(navmesh->Get_LeagueNavMesh(), navmesh->Get_navQuery(),
		current_pos, search_radius, 1500.0f, Hitbox, team,
		check))
	{
		state.safePos = { 0,0,0 };
		return;
	}
	std::sort(check.begin(), check.end(), [](const safeposINFO& a, const safeposINFO& b) {
		return a.safety > b.safety; // 안전도 높은 순으로 정렬
		});
	state.safePos = check[0].safepos;

}

float GameStateAnalyzer::calculate_reward(Object* ai_object, AIAction action, const GameStateInfo& prev_state, const GameStateInfo& current_state, bool episode_done)
{
	float reward = 0.0f;
	SpellBook* spellbook = ai_object->Get_P_SpellBook();

	// === 1. 기본 생존 보상 ===
	reward += 0.1f;

	// === 2. HP 변화 보상/페널티 (더 극적이게) ===
	float hp_change = current_state.player_hp_ratio - prev_state.player_hp_ratio;
	reward += hp_change * 20.0f; // 15.0f -> 20.0f로 증가 (생존 중요성 강조)

	// === 3. MP 효율적 사용 보상 (전투 편향 방지) ===
	float mp_change = prev_state.player_mp_ratio - current_state.player_mp_ratio;
	if (mp_change > 0.0f) {
		reward += calculate_mana_usage_reward(action, mp_change, current_state);
	}

	// === 4. 스킬 사용 보상/페널티 (크게 완화) ===
	reward += calculate_skill_usage_reward(spellbook, action, prev_state, current_state);

	// === 5. 내비게이션 효율성 보상 ===
	reward += calculate_navigation_reward(action, prev_state, current_state);

	// === 6. NavMesh 기반 안전도 보상 (핵심 - 대폭 강화!) ===
	reward += calculate_safety_reward(prev_state, current_state, action);

	// === 7. 전투 보상 (전략적 개선 - 대폭 완화) ===
	reward += calculate_combat_reward(ai_object, action, prev_state, current_state);

	// === 8. 전략적 위치 보상 ===
	reward += calculate_strategic_reward(prev_state, current_state, action);

	// === 9. 에피소드 종료 보상/페널티 ===
	if (episode_done) {
		if (ai_object->Get_VMT_Component()->is_dead()) {
			reward -= 100.0f; // 80.0f -> 100.0f로 증가 (죽음에 대한 강한 페널티)
		}
		else {
			reward += 50.0f;  // 40.0f -> 50.0f로 증가 (생존 보상 강화)
		}
	}

	// === 10. 보상 클리핑 (안정성) ===
	reward = std::max(-100.0f, std::min(100.0f, reward)); // 범위 확장

	return reward;
}

float GameStateAnalyzer::calculate_mana_usage_reward(AIAction action, float mp_used, const GameStateInfo& current_state)
{
	float reward = 0.0f;

	if (action == AIAction::SKILL_Q || action == AIAction::SKILL_E || action == AIAction::SKILL_R) {
		if (current_state.has_enemy_target) {
			// 적이 있을 때 스킬 사용은 긍정 (완화)
			reward += 1.5f; // 2.5f -> 1.5f로 완화

			// 적의 HP가 낮을 때 스킬 사용은 더 긍정
			if (current_state.enemy_hp_ratio < 0.4f) {
				reward += 2.5f; // 4.0f -> 2.5f로 완화
			}

			// 근거리에서 스킬 사용시 추가 보상 (완화)
			if (current_state.distance_to_enemy < 600.0f) {
				reward += 1.0f; // 1.5f -> 1.0f로 완화
			}
		}
		else {
			// E스킬(이동기)는 예외적으로 덜 페널티
			if (action == AIAction::SKILL_E) {
				reward -= mp_used * 6.0f; // 8.0f -> 6.0f로 완화
			}
			else {
				reward -= mp_used * 10.0f; // 12.0f -> 10.0f로 완화
			}
		}
	}

	return reward;
}

float GameStateAnalyzer::calculate_skill_usage_reward(SpellBook* spellbook, AIAction action, const GameStateInfo& prev_state, const GameStateInfo& current_state)
{
	float reward = 0.0f;

	switch (action) {
	case AIAction::SKILL_Q:
		if (prev_state.skill_status.q_ready) {
			if (prev_state.has_enemy_target && prev_state.distance_to_enemy < spellbook->Get_spell_range(0) && prev_state.enemy_hp_ratio < 0.3f) {
				reward += 4.0f; // 8.0f -> 4.0f로 대폭 완화
			}
			else if (prev_state.has_enemy_target && prev_state.distance_to_enemy < spellbook->Get_spell_range(0)) {
				reward += 2.5f; // 6.0f -> 2.5f로 대폭 완화
			}
			else {
				reward -= 1.5f; // 2.0f -> 1.5f로 완화
			}
		}
		else {
			reward -= 4.0f; // 6.0f -> 4.0f로 완화
		}
		break;

	case AIAction::SKILL_E: // 순간이동/이동 스킬
		if (prev_state.skill_status.e_ready) {
			// 위험 상황에서 E 사용 - 더 관대한 조건
			if (prev_state.current_tile.danger_level > 0.8f && prev_state.player_hp_ratio < 0.5f) {
				reward += 9.0f; // 12.0f -> 8.0f
			}
			else if (prev_state.current_tile.danger_level > 0.5f) {
				reward += 6.0f; // 10.0f -> 6.0f
			}
			// 적극적일 때 적 공격범위에서 탈출
			else if (prev_state.in_enemy_attack_range && prev_state.player_hp_ratio < 0.6f) {
				reward += 5.0f; // 8.0f -> 5.0f
			}
			// 적과의 거리 조절 (더 유연하게)
			else if (prev_state.has_enemy_target) {
				if (prev_state.distance_to_enemy < 300.0f) {
					reward += 2.0f; // 4.0f -> 2.0f
				}
				else if (prev_state.distance_to_enemy > 700.0f) {
					reward += 1.0f; // 2.5f -> 1.0f
				}
				else {
					reward -= 0.5f; // 1.0f -> 0.5f로 완화
				}
			}
			else {
				reward -= 1.0f; // 2.0f -> 1.0f로 완화
			}
		}
		else {
			reward -= 5.0f; // 8.0f -> 5.0f로 완화
		}
		break;

	case AIAction::SKILL_R: // 궁극기
		if (prev_state.skill_status.r_ready) {
			if (prev_state.has_enemy_target) {
				// 적 체력이 낮을 때 긍정 (완화)
				if (prev_state.enemy_hp_ratio < 0.6f) {
					reward += 10.0f; // 20.0f -> 10.0f로 대폭 완화
				}
				// 여러 적이 근처에 있을 때 긍정 (완화)
				if (prev_state.enemy_count_nearby >= 2) {
					reward += 6.0f; // 12.0f -> 6.0f로 완화
				}
				// 일반적인 전투 상황 (완화)
				if (prev_state.distance_to_enemy < 600.0f) {
					reward += 3.0f; // 8.0f -> 3.0f로 대폭 완화
				}
			}
			else {
				reward -= 8.0f; // 12.0f -> 8.0f로 완화
			}
		}
		else {
			reward -= 8.0f; // 12.0f -> 8.0f로 완화
		}
		break;
	}

	return reward;
}

float GameStateAnalyzer::calculate_navigation_reward(AIAction action, const GameStateInfo& prev_state, const GameStateInfo& current_state)
{
	float reward = 0.0f;

	if (action == AIAction::FOLLOW_NAVIGATION) {
		if (prev_state.navigation_status.has_navigation_path) {
			// 경로 진행 보상
			if (current_state.navigation_status.progress_ratio > prev_state.navigation_status.progress_ratio) {
				reward += 3.0f;
			}

			// 차단된 경로를 따라가려 하면 페널티
			if (prev_state.navigation_status.is_path_blocked) {
				reward -= 5.0f;
			}

			// 목표에 가까워지면 보상
			if (current_state.navigation_status.distance_to_target < prev_state.navigation_status.distance_to_target) {
				reward += 2.0f;
			}
		}
		else {
			reward -= 2.0f; // 경로가 없는데 따라가려 함
		}
	}
	else if (action == AIAction::CANCEL_NAVIGATION) {
		// 위험하거나 차단된 경로 취소는 좋음
		if (prev_state.navigation_status.is_path_blocked ||
			prev_state.current_tile.danger_level > 0.7f) {
			reward += 4.0f;
		}
		else {
			reward -= 3.0f; // 불필요한 경로 취소
		}
	}

	return reward;
}

float GameStateAnalyzer::calculate_safety_reward(const GameStateInfo& prev_state, const GameStateInfo& current_state, AIAction action)
{
	float reward = 0.0f;

	// === 안전도 개선 보상 (대폭 강화!) ===
	float current_safety = 1.0f - current_state.current_tile.danger_level;
	float prev_safety = 1.0f - prev_state.current_tile.danger_level;
	float safety_improvement = current_safety - prev_safety;

	reward += safety_improvement * 25.0f; // 10.0f -> 25.0f로 대폭 강화!

	// === 즉시 위험에서 벗어나면 큰 보상 (강화!) ===
	if (prev_state.current_tile.has_missile && prev_state.current_tile.is_enemy_spell_area) {
		if (!current_state.current_tile.has_missile || !current_state.current_tile.is_enemy_spell_area) {
			reward += 40.0f; // 25.0f -> 40.0f로 대폭 강화!
		}
	}

	// === 현재 위험도에 따른 페널티 (더 관대하게) ===
	if (current_state.current_tile.danger_level > 0.9f) {
		reward -= 15.0f; // 6.0f -> 15.0f로 강화 (극도로 위험한 지역 강력 페널티)
	}
	else if (current_state.current_tile.danger_level > 0.7f) {
		reward -= 8.0f; // 3.0f -> 8.0f로 강화
	}
	else if (current_state.current_tile.danger_level > 0.5f) {
		reward -= 3.0f; // 새로 추가 - 중간 위험도에 대한 페널티
	}

	// === 즉시 위험 지역 진입에서 큰 페널티 (강화!) ===
	if (current_state.current_tile.has_missile && current_state.current_tile.is_enemy_spell_area) {
		reward -= 50.0f; // 35.0f -> 50.0f로 대폭 강화!
	}

	// === 적 스펠 지역 페널티 (강화!) ===
	if (current_state.current_tile.is_enemy_spell_area) {
		reward -= 8.0f; // 4.0f -> 8.0f로 강화
	}

	// === 아군 스펠 지역에서 보상 ===
	if (current_state.current_tile.is_ally_spell_area) {
		reward += 3.0f; // 2.5f -> 3.0f로 약간 강화
	}

	// === 미사일 회피 보상 (강화!) ===
	if (prev_state.current_tile.has_missile && !current_state.current_tile.has_missile) {
		reward += 20.0f; // 12.0f -> 20.0f로 강화!
	}

	// === 안전 지역 유지 보상 (새로 추가!) ===
	if (current_state.current_tile.is_safe && current_state.current_tile.danger_level < 0.2f) {
		reward += 5.0f; // 안전한 지역에 머무르는 것에 대한 보상
	}

	// === 위험 지역에서 안전 지역으로 이동 시 추가 보상 (새로 추가!) ===
	if (prev_state.current_tile.danger_level > 0.6f && current_state.current_tile.danger_level < 0.3f) {
		reward += 15.0f; // 위험에서 안전으로 이동하는 것에 대한 큰 보상
	}

	return reward;
}

float GameStateAnalyzer::calculate_combat_reward(Object* ai_object, AIAction action, const GameStateInfo& prev_state, const GameStateInfo& current_state)
{
	float reward = 0.0f;

	if (current_state.has_enemy_target && prev_state.has_enemy_target) {
		// === 적절한 거리 유지 보상 (카이팅) - 완화 ===
		float optimal_distance = ai_object->Get_ObjectStats_Component().Get_total_Attack_range() * 0.8f;
		float distance_diff = std::abs(current_state.distance_to_enemy - optimal_distance);

		if (distance_diff < 100.0f) {
			reward += 1.5f; // 3.0f -> 1.5f로 대폭 완화
		}
		else if (distance_diff > 400.0f) {
			reward -= 0.8f; // 1.5f -> 0.8f로 완화
		}

		// === 너무 가까이 가면 위험 (더 관대하게) ===
		if (current_state.distance_to_enemy < 120.0f && current_state.player_hp_ratio < 0.3f) {
			reward -= 3.0f; // 6.0f -> 3.0f로 대폭 완화
		}

		// === 적이 적체력일 때 추격 보상 (완화) ===
		if (current_state.enemy_hp_ratio < 0.25f &&
			current_state.distance_to_enemy < prev_state.distance_to_enemy) {
			reward += 2.0f; // 5.0f -> 2.0f로 대폭 완화
		}
	}

	// === 전투 관련 로직 개선 ===

	// 전투 중일 때 (time_since_last_combat < 3초)
	if (current_state.time_since_last_combat < 3.0f) {
		// 현재 활발한 전투 중
		if (current_state.enemy_hp_ratio < prev_state.enemy_hp_ratio) {
			reward += 1.0f; // 2.5f -> 1.0f로 대폭 완화
		}
	}

	// === 연속 전투 플레이 체크 (더 관대하게) ===
	if (current_state.time_since_last_combat < 3.0f && prev_state.time_since_last_combat < 3.0f) {
		// 계속 전투 중일 때
		if (current_state.player_hp_ratio < 0.2f && // 0.25f -> 0.2f로 완화
			(action == AIAction::ATTACK_TARGET || action == AIAction::MOVE_TO_ENEMY)) {
			reward -= 3.0f; // 5.0f -> 3.0f로 완화
		}
	}

	// === 전투 진입 후 즉각 보상 (완화) ===
	if (prev_state.time_since_last_combat > 5.0f && current_state.time_since_last_combat < 1.0f) {
		// 5초 이상 정적 후 전투 진입
		if (current_state.player_hp_ratio > 0.4f && current_state.player_mp_ratio > 0.2f) { // 조건 완화
			reward += 1.5f; // 3.0f -> 1.5f로 대폭 완화
		}
	}

	// === 장기간 비전투 상태에서의 행동 평가 (더 관대하게) ===
	if (current_state.time_since_last_combat > 8.0f) {
		if (current_state.has_enemy_target) {
			// 소극적인 행동들 - 전투 회피 페널티 (완화)
			if (action == AIAction::RETREAT_SAFE ||
				action == AIAction::STAY_POSITION ||
				action == AIAction::SKILL_E) {
				// 정당한 이유가 있는 경우는 페널티 완화
				if (current_state.player_hp_ratio < 0.25f || current_state.current_tile.danger_level > 0.7f) {
					reward += 1.5f; // 정당한 후퇴는 적은 페널티
				}
				else {
					reward -= 2.5f; // 3.0f -> 1.5f로 대폭 완화
				}
			}
			else if (action == AIAction::ATTACK_TARGET || action == AIAction::MOVE_TO_ENEMY ||
				action == AIAction::SKILL_Q || action == AIAction::SKILL_R) {
				reward += 1.0f; // 2.0f -> 1.0f로 완화
			}
		}
	}

	// === 공격 행동 평가 (대폭 완화!) ===
	if (action == AIAction::ATTACK_TARGET) {
		if (current_state.can_attack_enemy) {
			reward += 3.0f; // 8.0f -> 3.0f로 대폭 완화!

			// 적의 HP가 감소했으면 추가 보상 (완화)
			if (current_state.enemy_hp_ratio < prev_state.enemy_hp_ratio) {
				reward += 2.0f; // 6.0f -> 2.0f로 대폭 완화!
			}

			// 킬 확률이 높은 상황에서 공격 (완화)
			if (prev_state.enemy_hp_ratio < 0.2f) {
				reward += 4.0f; // 12.0f -> 4.0f로 대폭 완화!
			}
		}
		else {
			reward -= 2.0f; // 4.0f -> 2.0f로 완화
		}
	}

	// === 적에게 접근하는 행동 평가 (강화!) ===
	if (action == AIAction::MOVE_TO_ENEMY) {
		if (current_state.has_enemy_target) {
			// 공격 가능 거리에 도달하면 큰 보상
			if (current_state.can_attack_enemy && !prev_state.can_attack_enemy) {
				reward += 8.0f; 
			}
			// 멀리 있는 적에게 접근하는 것 강화
			else if (current_state.distance_to_enemy > 1200.0f) {
				reward += 6.0f; 
			}
			else if (current_state.distance_to_enemy > 800.0f) {
				reward += 4.0f; 
			}
			else if (current_state.distance_to_enemy > 400.0f) {
				reward += 2.0f; 
			}
			// 거리 단축 보상 추가
			else if (current_state.distance_to_enemy < prev_state.distance_to_enemy) {
				reward += 3.0f; 
			}
			// 너무 가까워지면 페널티 (기존 유지)
			else if (current_state.distance_to_enemy < 120.0f&& current_state.player_hp_ratio<0.3f) {
				reward -= 2.5f;
			}

			// 적 체력이 낮을 때 추격 보상 추가
			if (current_state.enemy_hp_ratio < 0.3f) {
				reward += 5.0f; // 마무리 추격 보상
			}
		}
		else {
			reward -= 1.0f; // 기존 유지
		}
	}

	return reward;
}

float GameStateAnalyzer::calculate_strategic_reward(const GameStateInfo& prev_state, const GameStateInfo& current_state, AIAction action)
{
	float reward = 0.0f;

	// === 응급 상황 보상 로직 (새로 추가) ===

	// 모든 방향이 막힌 상황인지 확인
	int blocked_count = 0;
	for (int i = 0; i < 8; ++i) {
		if (prev_state.direction_blocked[i]) {
			blocked_count++;
		}
	}

	bool emergency_situation = (blocked_count >= 7); // 7개 이상 막혔으면 응급 상황

	if (emergency_situation) {
		if (action == AIAction::RETREAT_SAFE) {
			// 응급 상황에서 안전한 곳으로 후퇴하면 매우 큰 보상
			reward += 20.0f;

			// 위험도가 실제로 감소했다면 추가 보상
			if (current_state.current_tile.danger_level < prev_state.current_tile.danger_level) {
				reward += 15.0f;
			}
		}
		else if (action == AIAction::SKILL_E && prev_state.current_tile.danger_level > 0.6f) {
			// 응급 상황에서 E스킬로 탈출하면 큰 보상
			reward += 18.0f;
		}
		else if (action == AIAction::STAY_POSITION) {
			// 응급 상황에서는 STAY도 필요할 수 있음
			if (prev_state.current_tile.danger_level < 0.4f) {
				reward += 5.0f; // 안전한 곳에서 대기
			}
			else {
				reward -= 8.0f; // 위험한 곳에서 대기는 여전히 나쁨
			}
		}
		else if (action >= AIAction::MOVE_NORTH && action <= AIAction::MOVE_SOUTHWEST) {
			// 응급 상황에서 이동 가능한 방향으로 움직이면 보상
			int direction_index = static_cast<int>(action);
			if (direction_index < 8 && !prev_state.direction_blocked[direction_index]) {
				reward += 12.0f;

				// 안전도가 개선되면 추가 보상
				if (current_state.current_tile.danger_level < prev_state.current_tile.danger_level) {
					reward += 10.0f;
				}
			}
		}
	}

	// === 기존 로직들 유지 ===

	// 팀원 근처에서의 행동 평가
	if (current_state.is_near_ally) {
		if (current_state.player_hp_ratio < 0.5f) {
			reward += 4.0f;
		}
		else if (current_state.has_enemy_target && current_state.distance_to_enemy < 500.0f) {
			reward += 2.0f;
		}
	}

	// 안전 지역으로의 이동 (기존 로직 강화!)
	if (action == AIAction::RETREAT_SAFE) {
		if (prev_state.player_hp_ratio < 0.6f) {
			reward += 12.0f;
		}
		else if (prev_state.current_tile.danger_level > 0.5f) {
			reward += 15.0f; // 위험 지역에서 탈출하는 것 더 강화
		}
		else if (prev_state.current_tile.is_enemy_spell_area) {
			reward += 18.0f; // 적 스펠 범위에서 탈출하면 큰 보상
		}
		else if (prev_state.current_tile.has_missile) {
			reward += 20.0f; // 미사일 위험에서 탈출하면 매우 큰 보상
		}
	}

	// 제자리 대기 평가 (기존 로직 유지하되 응급상황 제외)
	if (action == AIAction::STAY_POSITION && !emergency_situation) {
		if (current_state.current_tile.is_safe && !current_state.has_enemy_target) {
			reward += 1.0f;
		}
		else if (current_state.current_tile.danger_level > 0.5f) {
			reward -= 6.0f;
		}
		else if (current_state.has_enemy_target && current_state.can_attack_enemy) {
			reward -= 1.5f;
		}
	}

	// 다수 적 상황에서의 행동
	if (current_state.enemy_count_nearby >= 2) {
		if (action == AIAction::RETREAT_SAFE || action == AIAction::SKILL_E) {
			reward += 6.0f;
		}
		else if (action == AIAction::ATTACK_TARGET && current_state.player_hp_ratio > 0.8f) {
			reward -= 2.0f;
		}
	}

	// MP 관리 보상
	if (current_state.player_mp_ratio < 0.1f &&
		(action == AIAction::SKILL_Q || action == AIAction::SKILL_E || action == AIAction::SKILL_R)) {
		reward -= 3.0f;
	}

	// HP 상태에 따른 행동 평가
	if (current_state.player_hp_ratio < 0.2f) {
		if (current_state.current_tile.danger_level < prev_state.current_tile.danger_level) {
			reward += 6.0f;
		}

		if (action == AIAction::ATTACK_TARGET && current_state.in_enemy_attack_range) {
			reward -= 4.0f;
		}
	}

	return reward;
}

