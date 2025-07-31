#include "RLStructures.h"
// SkillStatusInfo 생성자
SkillStatusInfo::SkillStatusInfo() :
    q_ready(false), e_ready(false), r_ready(false),
    q_cooldown_ratio(1.0f), e_cooldown_ratio(1.0f), r_cooldown_ratio(1.0f),
    q_mana_cost(0.0f), e_mana_cost(0.0f), r_mana_cost(0.0f),
    has_enough_mana_q(false), has_enough_mana_e(false), has_enough_mana_r(false),
    q_level(0), e_level(0), r_level(0) {

    // 누락된 필드들 초기화
    for (int i = 0; i < 4; ++i) {
        skilltype[i] = static_cast<SpellType>(0); // 기본값으로 초기화
        forblink[i] = Vec3(0.0f, 0.0f, 0.0f);   // 무효한 위치로 초기화
    }
}

// NavigationStatusInfo 생성자
NavigationStatusInfo::NavigationStatusInfo() :
    has_navigation_path(false), total_waypoints(0), current_waypoint_index(0),
    progress_ratio(0.0f), distance_to_target(0.0f), distance_to_final(0.0f),
    is_path_blocked(false), estimated_travel_time(0.0f) {
}

// TileInfo 생성자
TileInfo::TileInfo() :
    is_walkable(false), is_safe(false), is_enemy_spell_area(false),
    is_ally_spell_area(false), has_missile(false), is_structure(false),
    danger_level(1.0f), raw_flags(0), tilePos({0.0f,0.0f,0.0f}) {
}


// GameStateInfo 텐서 변환 구현
torch::Tensor GameStateInfo::to_tensor() const {
    std::vector<float> state_data;
    // 플레이어 기본 정보 (7차원) - z축 추가
    state_data.push_back(player_position.x / 15000.0f);
    state_data.push_back(player_position.y / 15000.0f);
    state_data.push_back(player_position.z / 1000.0f);  // z축 추가 (높이 정보)
    state_data.push_back(player_hp_ratio);
    state_data.push_back(player_mp_ratio);
    state_data.push_back(player_team == 1 ? 1.0f : -1.0f);
    state_data.push_back(search_radius / 2.0f);

    // 적 정보 (6차원) - z축 추가
    state_data.push_back(enemy_position.x / 15000.0f);
    state_data.push_back(enemy_position.y / 15000.0f);
    state_data.push_back(enemy_position.z / 1000.0f);  // z축 추가
    state_data.push_back(enemy_hp_ratio);
    state_data.push_back(distance_to_enemy / 2000.0f);
    state_data.push_back(has_enemy_target ? 1.0f : 0.0f);

    // 스킬 상태 (39차원) - forblink z축들 추가
    state_data.push_back(skill_status.q_ready ? 1.0f : 0.0f);
    state_data.push_back(skill_status.e_ready ? 1.0f : 0.0f);
    state_data.push_back(skill_status.r_ready ? 1.0f : 0.0f);
    state_data.push_back(skill_status.q_cooldown_ratio);
    state_data.push_back(skill_status.e_cooldown_ratio);
    state_data.push_back(skill_status.r_cooldown_ratio);
    state_data.push_back(skill_status.q_mana_cost / 100.0f);
    state_data.push_back(skill_status.e_mana_cost / 100.0f);
    state_data.push_back(skill_status.r_mana_cost / 100.0f);
    state_data.push_back(skill_status.has_enough_mana_q ? 1.0f : 0.0f);
    state_data.push_back(skill_status.has_enough_mana_e ? 1.0f : 0.0f);
    state_data.push_back(skill_status.has_enough_mana_r ? 1.0f : 0.0f);
    state_data.push_back(skill_status.q_level / 5.0f);
    state_data.push_back(skill_status.e_level / 5.0f);
    state_data.push_back(skill_status.r_level / 5.0f);

    // skilltype[4] (4차원)
    for (int i = 0; i < 4; ++i) {
        state_data.push_back(static_cast<float>(skill_status.skilltype[i]) / 10.0f);
    }

    // forblink[4] (12차원 = 4개 위치 × 3(x,y,z))
    for (int i = 0; i < 4; ++i) {
        state_data.push_back(skill_status.forblink[i].x / 15000.0f);
        state_data.push_back(skill_status.forblink[i].y / 15000.0f);
        state_data.push_back(skill_status.forblink[i].z / 1000.0f);  // z축 추가
    }

    // 네비게이션 상태 (12차원) - z축들 추가
    state_data.push_back(navigation_status.has_navigation_path ? 1.0f : 0.0f);
    state_data.push_back(navigation_status.total_waypoints / 20.0f);
    state_data.push_back(navigation_status.progress_ratio);
    state_data.push_back(navigation_status.distance_to_target / 1000.0f);
    state_data.push_back(navigation_status.distance_to_final / 2000.0f);
    state_data.push_back(navigation_status.is_path_blocked ? 1.0f : 0.0f);
    state_data.push_back(navigation_status.current_target.x / 15000.0f);
    state_data.push_back(navigation_status.current_target.y / 15000.0f);
    state_data.push_back(navigation_status.current_target.z / 1000.0f);  // z축 추가
    state_data.push_back(navigation_status.final_destination.x / 15000.0f);  // final_destination 추가
    state_data.push_back(navigation_status.final_destination.y / 15000.0f);
    state_data.push_back(navigation_status.final_destination.z / 1000.0f);
    state_data.push_back(navigation_status.estimated_travel_time / 60.0f);

    // 현재 타일 (9차원) - tilePos z축 추가
    state_data.push_back(current_tile.is_walkable ? 1.0f : 0.0f);
    state_data.push_back(current_tile.is_safe ? 1.0f : 0.0f);
    state_data.push_back(current_tile.is_enemy_spell_area ? 1.0f : 0.0f);
    state_data.push_back(current_tile.is_ally_spell_area ? 1.0f : 0.0f);
    state_data.push_back(current_tile.has_missile ? 1.0f : 0.0f);
    state_data.push_back(current_tile.is_structure ? 1.0f : 0.0f);
    state_data.push_back(current_tile.danger_level);
    state_data.push_back(current_tile.raw_flags / 65535.0f);
    state_data.push_back(current_tile.tilePos.z / 1000.0f);  // z축 추가

    // 8방향 타일 (64차원 = 8×8) - 각 타일의 z축 추가
    for (int i = 0; i < 8; ++i) {
        state_data.push_back(surrounding_tiles[i].is_walkable ? 1.0f : 0.0f);
        state_data.push_back(surrounding_tiles[i].is_safe ? 1.0f : 0.0f);
        state_data.push_back(surrounding_tiles[i].is_enemy_spell_area ? 1.0f : 0.0f);
        state_data.push_back(surrounding_tiles[i].has_missile ? 1.0f : 0.0f);
        state_data.push_back(surrounding_tiles[i].danger_level);
        state_data.push_back(best_direction_safety[i]);
        state_data.push_back(direction_blocked[i] ? 0.0f : 1.0f);
        state_data.push_back(surrounding_tiles[i].tilePos.z / 1000.0f);  // z축 추가
    }

    // 전략적 정보 (11차원) - safePos z축 추가
    state_data.push_back(safePos.x / 15000.0f);
    state_data.push_back(safePos.y / 15000.0f);
    state_data.push_back(safePos.z / 1000.0f);  // z축 추가
    state_data.push_back(in_enemy_attack_range ? 1.0f : 0.0f);
    state_data.push_back(can_attack_enemy ? 1.0f : 0.0f);
    state_data.push_back(std::min(enemy_count_nearby / 5.0f, 1.0f));
    state_data.push_back(player_attack_range / 800.0f);
    state_data.push_back(player_movement_speed / 500.0f);
    state_data.push_back(is_near_ally ? 1.0f : 0.0f);
    state_data.push_back(std::min(time_since_last_combat / 30.0f, 1.0f));

    // 높이 관련 추가 정보 (3차원)
    // 플레이어와 적의 높이 차이
    float height_diff = (player_position.z - enemy_position.z) / 1000.0f;
    state_data.push_back(height_diff);

    // 현재 위치와 안전 위치의 높이 차이
    float safe_height_diff = (player_position.z - safePos.z) / 1000.0f;
    state_data.push_back(safe_height_diff);

    // 현재 타일과 주변 타일들의 평균 높이 차이
    float avg_surrounding_height = 0.0f;
    for (int i = 0; i < 8; ++i) {
        avg_surrounding_height += surrounding_tiles[i].tilePos.z;
    }
    avg_surrounding_height /= 8.0f;
    float relative_height = (current_tile.tilePos.z - avg_surrounding_height) / 1000.0f;
    state_data.push_back(relative_height);
 

    return torch::tensor(state_data, torch::kFloat32);
}

DQNImpl::DQNImpl(int64_t input_size, int64_t output_size) {
    // 레이어 정의 (더 깊은 네트워크)
    fc1 = register_module("fc1", torch::nn::Linear(input_size, 512));
    bn1 = register_module("bn1", torch::nn::BatchNorm1d(512));
    fc2 = register_module("fc2", torch::nn::Linear(512, 256));
    bn2 = register_module("bn2", torch::nn::BatchNorm1d(256));
    fc3 = register_module("fc3", torch::nn::Linear(256, 128));
    fc4 = register_module("fc4", torch::nn::Linear(128, output_size));
    dropout = register_module("dropout", torch::nn::Dropout(0.3));
}

torch::Tensor DQNImpl::forward(torch::Tensor x) {
    try {
        // 배치 크기가 1일 때 BatchNorm 문제 해결
        bool use_batch_norm = x.size(0) > 1; // 배치 크기가 1보다 클 때만 BatchNorm 사용

        auto fc1_out = fc1->forward(x);

        if (use_batch_norm) {
            x = torch::relu(bn1->forward(fc1_out));
        }
        else {
            x = torch::relu(fc1_out); // BatchNorm 스킵
        }

        x = dropout->forward(x);

        auto fc2_out = fc2->forward(x);

        if (use_batch_norm) {
            x = torch::relu(bn2->forward(fc2_out));
        }
        else {
            x = torch::relu(fc2_out); // BatchNorm 스킵
        }

        x = dropout->forward(x);
        x = torch::relu(fc3->forward(x));
        x = fc4->forward(x); // Q-값 출력

        return x;

    }
    catch (const std::exception& e) {
        std::cerr << "[DQN] Forward pass error: " << e.what() << std::endl;
        std::cerr << "[DQN] Input shape was: " << x.sizes() << std::endl;
        throw;
    }

}