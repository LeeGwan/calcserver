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
	float base_buffer = 0.05f;      // �ּ� 5cm ����
	float ratio_buffer = 1.2f;      // 120% ����
	float min_radius = 0.1f;        // �ּ� �ݰ�
	float max_radius = 1.0f;        // �ִ� �ݰ�
	float search_radius = 0.0f;
	float last_combat_time = 0.0f;
	GameStateInfo state;
	Object* closest_enemy = nullptr;

	//  NEW: �÷��̾� ��ġ ���� �߰�
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

	//  NEW: �� ã�� �� �� ������ ����
	if (Find_OBJ::Find_Pos_is_near_Enemy(ai_object, closest_enemy, all_objects))
	{
		//  ���� ������ ����ִ��� Ȯ��
		if (closest_enemy &&
			!closest_enemy->Get_VMT_Component()->is_dead()) {

			state.enemy_hash = closest_enemy->Get_Hash();
			state.enemy_position = closest_enemy->Get_Transform_Component().Get_Position();
			auto& enemy_stats = closest_enemy->Get_ObjectStats_Component();
			state.enemy_hp_ratio = enemy_stats.Get_health() / std::max(enemy_stats.Get_maxhelth(), 1.0f);
			state.distance_to_enemy = state.player_position.distance(state.enemy_position);
			state.has_enemy_target = true;
		
			 //  NEW: HP�� 0 ������ ���� ��ȿ���� ���� ������ ó��
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
			// ���� ���̰ų� ��ȿ���� ���� ��
			state.has_enemy_target = false;
		}
	}
	else {
		state.has_enemy_target = false;
	}

	//  NEW: has_enemy_target�� false�� �� �⺻�� ����
	if (!state.has_enemy_target) {
		state.distance_to_enemy = 10000.0f;
		state.can_attack_enemy = false;
		state.in_enemy_attack_range = false;
		state.enemy_position = state.player_position;
		state.enemy_hp_ratio = 0.0f; //  ���� ������ ��Ȯ�� ǥ��
	}

	state.skill_status = ai_object->Get_P_SpellBook()->analyze_skill_status(navmesh,ai_object, state.search_radius);

	state.navigation_status = navigation_analyzer->analyze_navigation_status(navmesh, ai_object, deltatime);
	analyze_navmesh_state(navmesh, state, ai_object, search_radius);

	//���ݻ�Ÿ� *2 ����
	state.enemy_count_nearby = Find_OBJ::count_nearby_enemies(ai_object, all_objects, state.player_attack_range * 2.0f);
	state.is_near_ally = Find_OBJ::check_near_ally(ai_object, all_objects);//�Ʊ� 
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
		return a.safety > b.safety; // ������ ���� ������ ����
		});
	state.safePos = check[0].safepos;

}

float GameStateAnalyzer::calculate_reward(Object* ai_object, AIAction action, const GameStateInfo& prev_state, const GameStateInfo& current_state, bool episode_done)
{
	float reward = 0.0f;
	SpellBook* spellbook = ai_object->Get_P_SpellBook();

	// === 1. �⺻ ���� ���� ===
	reward += 0.1f;

	// === 2. HP ��ȭ ����/���Ƽ (�� �����̰�) ===
	float hp_change = current_state.player_hp_ratio - prev_state.player_hp_ratio;
	reward += hp_change * 20.0f; // 15.0f -> 20.0f�� ���� (���� �߿伺 ����)

	// === 3. MP ȿ���� ��� ���� (���� ���� ����) ===
	float mp_change = prev_state.player_mp_ratio - current_state.player_mp_ratio;
	if (mp_change > 0.0f) {
		reward += calculate_mana_usage_reward(action, mp_change, current_state);
	}

	// === 4. ��ų ��� ����/���Ƽ (ũ�� ��ȭ) ===
	reward += calculate_skill_usage_reward(spellbook, action, prev_state, current_state);

	// === 5. ������̼� ȿ���� ���� ===
	reward += calculate_navigation_reward(action, prev_state, current_state);

	// === 6. NavMesh ��� ������ ���� (�ٽ� - ���� ��ȭ!) ===
	reward += calculate_safety_reward(prev_state, current_state, action);

	// === 7. ���� ���� (������ ���� - ���� ��ȭ) ===
	reward += calculate_combat_reward(ai_object, action, prev_state, current_state);

	// === 8. ������ ��ġ ���� ===
	reward += calculate_strategic_reward(prev_state, current_state, action);

	// === 9. ���Ǽҵ� ���� ����/���Ƽ ===
	if (episode_done) {
		if (ai_object->Get_VMT_Component()->is_dead()) {
			reward -= 100.0f; // 80.0f -> 100.0f�� ���� (������ ���� ���� ���Ƽ)
		}
		else {
			reward += 50.0f;  // 40.0f -> 50.0f�� ���� (���� ���� ��ȭ)
		}
	}

	// === 10. ���� Ŭ���� (������) ===
	reward = std::max(-100.0f, std::min(100.0f, reward)); // ���� Ȯ��

	return reward;
}

float GameStateAnalyzer::calculate_mana_usage_reward(AIAction action, float mp_used, const GameStateInfo& current_state)
{
	float reward = 0.0f;

	if (action == AIAction::SKILL_Q || action == AIAction::SKILL_E || action == AIAction::SKILL_R) {
		if (current_state.has_enemy_target) {
			// ���� ���� �� ��ų ����� ���� (��ȭ)
			reward += 1.5f; // 2.5f -> 1.5f�� ��ȭ

			// ���� HP�� ���� �� ��ų ����� �� ����
			if (current_state.enemy_hp_ratio < 0.4f) {
				reward += 2.5f; // 4.0f -> 2.5f�� ��ȭ
			}

			// �ٰŸ����� ��ų ���� �߰� ���� (��ȭ)
			if (current_state.distance_to_enemy < 600.0f) {
				reward += 1.0f; // 1.5f -> 1.0f�� ��ȭ
			}
		}
		else {
			// E��ų(�̵���)�� ���������� �� ���Ƽ
			if (action == AIAction::SKILL_E) {
				reward -= mp_used * 6.0f; // 8.0f -> 6.0f�� ��ȭ
			}
			else {
				reward -= mp_used * 10.0f; // 12.0f -> 10.0f�� ��ȭ
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
				reward += 4.0f; // 8.0f -> 4.0f�� ���� ��ȭ
			}
			else if (prev_state.has_enemy_target && prev_state.distance_to_enemy < spellbook->Get_spell_range(0)) {
				reward += 2.5f; // 6.0f -> 2.5f�� ���� ��ȭ
			}
			else {
				reward -= 1.5f; // 2.0f -> 1.5f�� ��ȭ
			}
		}
		else {
			reward -= 4.0f; // 6.0f -> 4.0f�� ��ȭ
		}
		break;

	case AIAction::SKILL_E: // �����̵�/�̵� ��ų
		if (prev_state.skill_status.e_ready) {
			// ���� ��Ȳ���� E ��� - �� ������ ����
			if (prev_state.current_tile.danger_level > 0.8f && prev_state.player_hp_ratio < 0.5f) {
				reward += 9.0f; // 12.0f -> 8.0f
			}
			else if (prev_state.current_tile.danger_level > 0.5f) {
				reward += 6.0f; // 10.0f -> 6.0f
			}
			// �������� �� �� ���ݹ������� Ż��
			else if (prev_state.in_enemy_attack_range && prev_state.player_hp_ratio < 0.6f) {
				reward += 5.0f; // 8.0f -> 5.0f
			}
			// ������ �Ÿ� ���� (�� �����ϰ�)
			else if (prev_state.has_enemy_target) {
				if (prev_state.distance_to_enemy < 300.0f) {
					reward += 2.0f; // 4.0f -> 2.0f
				}
				else if (prev_state.distance_to_enemy > 700.0f) {
					reward += 1.0f; // 2.5f -> 1.0f
				}
				else {
					reward -= 0.5f; // 1.0f -> 0.5f�� ��ȭ
				}
			}
			else {
				reward -= 1.0f; // 2.0f -> 1.0f�� ��ȭ
			}
		}
		else {
			reward -= 5.0f; // 8.0f -> 5.0f�� ��ȭ
		}
		break;

	case AIAction::SKILL_R: // �ñر�
		if (prev_state.skill_status.r_ready) {
			if (prev_state.has_enemy_target) {
				// �� ü���� ���� �� ���� (��ȭ)
				if (prev_state.enemy_hp_ratio < 0.6f) {
					reward += 10.0f; // 20.0f -> 10.0f�� ���� ��ȭ
				}
				// ���� ���� ��ó�� ���� �� ���� (��ȭ)
				if (prev_state.enemy_count_nearby >= 2) {
					reward += 6.0f; // 12.0f -> 6.0f�� ��ȭ
				}
				// �Ϲ����� ���� ��Ȳ (��ȭ)
				if (prev_state.distance_to_enemy < 600.0f) {
					reward += 3.0f; // 8.0f -> 3.0f�� ���� ��ȭ
				}
			}
			else {
				reward -= 8.0f; // 12.0f -> 8.0f�� ��ȭ
			}
		}
		else {
			reward -= 8.0f; // 12.0f -> 8.0f�� ��ȭ
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
			// ��� ���� ����
			if (current_state.navigation_status.progress_ratio > prev_state.navigation_status.progress_ratio) {
				reward += 3.0f;
			}

			// ���ܵ� ��θ� ���󰡷� �ϸ� ���Ƽ
			if (prev_state.navigation_status.is_path_blocked) {
				reward -= 5.0f;
			}

			// ��ǥ�� ��������� ����
			if (current_state.navigation_status.distance_to_target < prev_state.navigation_status.distance_to_target) {
				reward += 2.0f;
			}
		}
		else {
			reward -= 2.0f; // ��ΰ� ���µ� ���󰡷� ��
		}
	}
	else if (action == AIAction::CANCEL_NAVIGATION) {
		// �����ϰų� ���ܵ� ��� ��Ҵ� ����
		if (prev_state.navigation_status.is_path_blocked ||
			prev_state.current_tile.danger_level > 0.7f) {
			reward += 4.0f;
		}
		else {
			reward -= 3.0f; // ���ʿ��� ��� ���
		}
	}

	return reward;
}

float GameStateAnalyzer::calculate_safety_reward(const GameStateInfo& prev_state, const GameStateInfo& current_state, AIAction action)
{
	float reward = 0.0f;

	// === ������ ���� ���� (���� ��ȭ!) ===
	float current_safety = 1.0f - current_state.current_tile.danger_level;
	float prev_safety = 1.0f - prev_state.current_tile.danger_level;
	float safety_improvement = current_safety - prev_safety;

	reward += safety_improvement * 25.0f; // 10.0f -> 25.0f�� ���� ��ȭ!

	// === ��� ���迡�� ����� ū ���� (��ȭ!) ===
	if (prev_state.current_tile.has_missile && prev_state.current_tile.is_enemy_spell_area) {
		if (!current_state.current_tile.has_missile || !current_state.current_tile.is_enemy_spell_area) {
			reward += 40.0f; // 25.0f -> 40.0f�� ���� ��ȭ!
		}
	}

	// === ���� ���赵�� ���� ���Ƽ (�� �����ϰ�) ===
	if (current_state.current_tile.danger_level > 0.9f) {
		reward -= 15.0f; // 6.0f -> 15.0f�� ��ȭ (�ص��� ������ ���� ���� ���Ƽ)
	}
	else if (current_state.current_tile.danger_level > 0.7f) {
		reward -= 8.0f; // 3.0f -> 8.0f�� ��ȭ
	}
	else if (current_state.current_tile.danger_level > 0.5f) {
		reward -= 3.0f; // ���� �߰� - �߰� ���赵�� ���� ���Ƽ
	}

	// === ��� ���� ���� ���Կ��� ū ���Ƽ (��ȭ!) ===
	if (current_state.current_tile.has_missile && current_state.current_tile.is_enemy_spell_area) {
		reward -= 50.0f; // 35.0f -> 50.0f�� ���� ��ȭ!
	}

	// === �� ���� ���� ���Ƽ (��ȭ!) ===
	if (current_state.current_tile.is_enemy_spell_area) {
		reward -= 8.0f; // 4.0f -> 8.0f�� ��ȭ
	}

	// === �Ʊ� ���� �������� ���� ===
	if (current_state.current_tile.is_ally_spell_area) {
		reward += 3.0f; // 2.5f -> 3.0f�� �ణ ��ȭ
	}

	// === �̻��� ȸ�� ���� (��ȭ!) ===
	if (prev_state.current_tile.has_missile && !current_state.current_tile.has_missile) {
		reward += 20.0f; // 12.0f -> 20.0f�� ��ȭ!
	}

	// === ���� ���� ���� ���� (���� �߰�!) ===
	if (current_state.current_tile.is_safe && current_state.current_tile.danger_level < 0.2f) {
		reward += 5.0f; // ������ ������ �ӹ����� �Ϳ� ���� ����
	}

	// === ���� �������� ���� �������� �̵� �� �߰� ���� (���� �߰�!) ===
	if (prev_state.current_tile.danger_level > 0.6f && current_state.current_tile.danger_level < 0.3f) {
		reward += 15.0f; // ���迡�� �������� �̵��ϴ� �Ϳ� ���� ū ����
	}

	return reward;
}

float GameStateAnalyzer::calculate_combat_reward(Object* ai_object, AIAction action, const GameStateInfo& prev_state, const GameStateInfo& current_state)
{
	float reward = 0.0f;

	if (current_state.has_enemy_target && prev_state.has_enemy_target) {
		// === ������ �Ÿ� ���� ���� (ī����) - ��ȭ ===
		float optimal_distance = ai_object->Get_ObjectStats_Component().Get_total_Attack_range() * 0.8f;
		float distance_diff = std::abs(current_state.distance_to_enemy - optimal_distance);

		if (distance_diff < 100.0f) {
			reward += 1.5f; // 3.0f -> 1.5f�� ���� ��ȭ
		}
		else if (distance_diff > 400.0f) {
			reward -= 0.8f; // 1.5f -> 0.8f�� ��ȭ
		}

		// === �ʹ� ������ ���� ���� (�� �����ϰ�) ===
		if (current_state.distance_to_enemy < 120.0f && current_state.player_hp_ratio < 0.3f) {
			reward -= 3.0f; // 6.0f -> 3.0f�� ���� ��ȭ
		}

		// === ���� ��ü���� �� �߰� ���� (��ȭ) ===
		if (current_state.enemy_hp_ratio < 0.25f &&
			current_state.distance_to_enemy < prev_state.distance_to_enemy) {
			reward += 2.0f; // 5.0f -> 2.0f�� ���� ��ȭ
		}
	}

	// === ���� ���� ���� ���� ===

	// ���� ���� �� (time_since_last_combat < 3��)
	if (current_state.time_since_last_combat < 3.0f) {
		// ���� Ȱ���� ���� ��
		if (current_state.enemy_hp_ratio < prev_state.enemy_hp_ratio) {
			reward += 1.0f; // 2.5f -> 1.0f�� ���� ��ȭ
		}
	}

	// === ���� ���� �÷��� üũ (�� �����ϰ�) ===
	if (current_state.time_since_last_combat < 3.0f && prev_state.time_since_last_combat < 3.0f) {
		// ��� ���� ���� ��
		if (current_state.player_hp_ratio < 0.2f && // 0.25f -> 0.2f�� ��ȭ
			(action == AIAction::ATTACK_TARGET || action == AIAction::MOVE_TO_ENEMY)) {
			reward -= 3.0f; // 5.0f -> 3.0f�� ��ȭ
		}
	}

	// === ���� ���� �� �ﰢ ���� (��ȭ) ===
	if (prev_state.time_since_last_combat > 5.0f && current_state.time_since_last_combat < 1.0f) {
		// 5�� �̻� ���� �� ���� ����
		if (current_state.player_hp_ratio > 0.4f && current_state.player_mp_ratio > 0.2f) { // ���� ��ȭ
			reward += 1.5f; // 3.0f -> 1.5f�� ���� ��ȭ
		}
	}

	// === ��Ⱓ ������ ���¿����� �ൿ �� (�� �����ϰ�) ===
	if (current_state.time_since_last_combat > 8.0f) {
		if (current_state.has_enemy_target) {
			// �ұ����� �ൿ�� - ���� ȸ�� ���Ƽ (��ȭ)
			if (action == AIAction::RETREAT_SAFE ||
				action == AIAction::STAY_POSITION ||
				action == AIAction::SKILL_E) {
				// ������ ������ �ִ� ���� ���Ƽ ��ȭ
				if (current_state.player_hp_ratio < 0.25f || current_state.current_tile.danger_level > 0.7f) {
					reward += 1.5f; // ������ ����� ���� ���Ƽ
				}
				else {
					reward -= 2.5f; // 3.0f -> 1.5f�� ���� ��ȭ
				}
			}
			else if (action == AIAction::ATTACK_TARGET || action == AIAction::MOVE_TO_ENEMY ||
				action == AIAction::SKILL_Q || action == AIAction::SKILL_R) {
				reward += 1.0f; // 2.0f -> 1.0f�� ��ȭ
			}
		}
	}

	// === ���� �ൿ �� (���� ��ȭ!) ===
	if (action == AIAction::ATTACK_TARGET) {
		if (current_state.can_attack_enemy) {
			reward += 3.0f; // 8.0f -> 3.0f�� ���� ��ȭ!

			// ���� HP�� ���������� �߰� ���� (��ȭ)
			if (current_state.enemy_hp_ratio < prev_state.enemy_hp_ratio) {
				reward += 2.0f; // 6.0f -> 2.0f�� ���� ��ȭ!
			}

			// ų Ȯ���� ���� ��Ȳ���� ���� (��ȭ)
			if (prev_state.enemy_hp_ratio < 0.2f) {
				reward += 4.0f; // 12.0f -> 4.0f�� ���� ��ȭ!
			}
		}
		else {
			reward -= 2.0f; // 4.0f -> 2.0f�� ��ȭ
		}
	}

	// === ������ �����ϴ� �ൿ �� (��ȭ!) ===
	if (action == AIAction::MOVE_TO_ENEMY) {
		if (current_state.has_enemy_target) {
			// ���� ���� �Ÿ��� �����ϸ� ū ����
			if (current_state.can_attack_enemy && !prev_state.can_attack_enemy) {
				reward += 8.0f; 
			}
			// �ָ� �ִ� ������ �����ϴ� �� ��ȭ
			else if (current_state.distance_to_enemy > 1200.0f) {
				reward += 6.0f; 
			}
			else if (current_state.distance_to_enemy > 800.0f) {
				reward += 4.0f; 
			}
			else if (current_state.distance_to_enemy > 400.0f) {
				reward += 2.0f; 
			}
			// �Ÿ� ���� ���� �߰�
			else if (current_state.distance_to_enemy < prev_state.distance_to_enemy) {
				reward += 3.0f; 
			}
			// �ʹ� ��������� ���Ƽ (���� ����)
			else if (current_state.distance_to_enemy < 120.0f&& current_state.player_hp_ratio<0.3f) {
				reward -= 2.5f;
			}

			// �� ü���� ���� �� �߰� ���� �߰�
			if (current_state.enemy_hp_ratio < 0.3f) {
				reward += 5.0f; // ������ �߰� ����
			}
		}
		else {
			reward -= 1.0f; // ���� ����
		}
	}

	return reward;
}

float GameStateAnalyzer::calculate_strategic_reward(const GameStateInfo& prev_state, const GameStateInfo& current_state, AIAction action)
{
	float reward = 0.0f;

	// === ���� ��Ȳ ���� ���� (���� �߰�) ===

	// ��� ������ ���� ��Ȳ���� Ȯ��
	int blocked_count = 0;
	for (int i = 0; i < 8; ++i) {
		if (prev_state.direction_blocked[i]) {
			blocked_count++;
		}
	}

	bool emergency_situation = (blocked_count >= 7); // 7�� �̻� �������� ���� ��Ȳ

	if (emergency_situation) {
		if (action == AIAction::RETREAT_SAFE) {
			// ���� ��Ȳ���� ������ ������ �����ϸ� �ſ� ū ����
			reward += 20.0f;

			// ���赵�� ������ �����ߴٸ� �߰� ����
			if (current_state.current_tile.danger_level < prev_state.current_tile.danger_level) {
				reward += 15.0f;
			}
		}
		else if (action == AIAction::SKILL_E && prev_state.current_tile.danger_level > 0.6f) {
			// ���� ��Ȳ���� E��ų�� Ż���ϸ� ū ����
			reward += 18.0f;
		}
		else if (action == AIAction::STAY_POSITION) {
			// ���� ��Ȳ������ STAY�� �ʿ��� �� ����
			if (prev_state.current_tile.danger_level < 0.4f) {
				reward += 5.0f; // ������ ������ ���
			}
			else {
				reward -= 8.0f; // ������ ������ ���� ������ ����
			}
		}
		else if (action >= AIAction::MOVE_NORTH && action <= AIAction::MOVE_SOUTHWEST) {
			// ���� ��Ȳ���� �̵� ������ �������� �����̸� ����
			int direction_index = static_cast<int>(action);
			if (direction_index < 8 && !prev_state.direction_blocked[direction_index]) {
				reward += 12.0f;

				// �������� �����Ǹ� �߰� ����
				if (current_state.current_tile.danger_level < prev_state.current_tile.danger_level) {
					reward += 10.0f;
				}
			}
		}
	}

	// === ���� ������ ���� ===

	// ���� ��ó������ �ൿ ��
	if (current_state.is_near_ally) {
		if (current_state.player_hp_ratio < 0.5f) {
			reward += 4.0f;
		}
		else if (current_state.has_enemy_target && current_state.distance_to_enemy < 500.0f) {
			reward += 2.0f;
		}
	}

	// ���� ���������� �̵� (���� ���� ��ȭ!)
	if (action == AIAction::RETREAT_SAFE) {
		if (prev_state.player_hp_ratio < 0.6f) {
			reward += 12.0f;
		}
		else if (prev_state.current_tile.danger_level > 0.5f) {
			reward += 15.0f; // ���� �������� Ż���ϴ� �� �� ��ȭ
		}
		else if (prev_state.current_tile.is_enemy_spell_area) {
			reward += 18.0f; // �� ���� �������� Ż���ϸ� ū ����
		}
		else if (prev_state.current_tile.has_missile) {
			reward += 20.0f; // �̻��� ���迡�� Ż���ϸ� �ſ� ū ����
		}
	}

	// ���ڸ� ��� �� (���� ���� �����ϵ� ���޻�Ȳ ����)
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

	// �ټ� �� ��Ȳ������ �ൿ
	if (current_state.enemy_count_nearby >= 2) {
		if (action == AIAction::RETREAT_SAFE || action == AIAction::SKILL_E) {
			reward += 6.0f;
		}
		else if (action == AIAction::ATTACK_TARGET && current_state.player_hp_ratio > 0.8f) {
			reward -= 2.0f;
		}
	}

	// MP ���� ����
	if (current_state.player_mp_ratio < 0.1f &&
		(action == AIAction::SKILL_Q || action == AIAction::SKILL_E || action == AIAction::SKILL_R)) {
		reward -= 3.0f;
	}

	// HP ���¿� ���� �ൿ ��
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

