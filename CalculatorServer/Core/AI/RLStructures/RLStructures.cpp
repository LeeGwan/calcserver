#include "RLStructures.h"
// SkillStatusInfo ������
SkillStatusInfo::SkillStatusInfo() :
    q_ready(false), e_ready(false), r_ready(false),
    q_cooldown_ratio(1.0f), e_cooldown_ratio(1.0f), r_cooldown_ratio(1.0f),
    q_mana_cost(0.0f), e_mana_cost(0.0f), r_mana_cost(0.0f),
    has_enough_mana_q(false), has_enough_mana_e(false), has_enough_mana_r(false),
    q_level(0), e_level(0), r_level(0) {

    // ������ �ʵ�� �ʱ�ȭ
    for (int i = 0; i < 4; ++i) {
        skilltype[i] = static_cast<SpellType>(0); // �⺻������ �ʱ�ȭ
        forblink[i] = Vec3(0.0f, 0.0f, 0.0f);   // ��ȿ�� ��ġ�� �ʱ�ȭ
    }
}

// NavigationStatusInfo ������
NavigationStatusInfo::NavigationStatusInfo() :
    has_navigation_path(false), total_waypoints(0), current_waypoint_index(0),
    progress_ratio(0.0f), distance_to_target(0.0f), distance_to_final(0.0f),
    is_path_blocked(false), estimated_travel_time(0.0f) {
}

// TileInfo ������
TileInfo::TileInfo() :
    is_walkable(false), is_safe(false), is_enemy_spell_area(false),
    is_ally_spell_area(false), has_missile(false), is_structure(false),
    danger_level(1.0f), raw_flags(0), tilePos({0.0f,0.0f,0.0f}) {
}


// GameStateInfo �ټ� ��ȯ ����
torch::Tensor GameStateInfo::to_tensor() const {
    std::vector<float> state_data;
    // �÷��̾� �⺻ ���� (7����) - z�� �߰�
    state_data.push_back(player_position.x / 15000.0f);
    state_data.push_back(player_position.y / 15000.0f);
    state_data.push_back(player_position.z / 1000.0f);  // z�� �߰� (���� ����)
    state_data.push_back(player_hp_ratio);
    state_data.push_back(player_mp_ratio);
    state_data.push_back(player_team == 1 ? 1.0f : -1.0f);
    state_data.push_back(search_radius / 2.0f);

    // �� ���� (6����) - z�� �߰�
    state_data.push_back(enemy_position.x / 15000.0f);
    state_data.push_back(enemy_position.y / 15000.0f);
    state_data.push_back(enemy_position.z / 1000.0f);  // z�� �߰�
    state_data.push_back(enemy_hp_ratio);
    state_data.push_back(distance_to_enemy / 2000.0f);
    state_data.push_back(has_enemy_target ? 1.0f : 0.0f);

    // ��ų ���� (39����) - forblink z��� �߰�
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

    // skilltype[4] (4����)
    for (int i = 0; i < 4; ++i) {
        state_data.push_back(static_cast<float>(skill_status.skilltype[i]) / 10.0f);
    }

    // forblink[4] (12���� = 4�� ��ġ �� 3(x,y,z))
    for (int i = 0; i < 4; ++i) {
        state_data.push_back(skill_status.forblink[i].x / 15000.0f);
        state_data.push_back(skill_status.forblink[i].y / 15000.0f);
        state_data.push_back(skill_status.forblink[i].z / 1000.0f);  // z�� �߰�
    }

    // �׺���̼� ���� (12����) - z��� �߰�
    state_data.push_back(navigation_status.has_navigation_path ? 1.0f : 0.0f);
    state_data.push_back(navigation_status.total_waypoints / 20.0f);
    state_data.push_back(navigation_status.progress_ratio);
    state_data.push_back(navigation_status.distance_to_target / 1000.0f);
    state_data.push_back(navigation_status.distance_to_final / 2000.0f);
    state_data.push_back(navigation_status.is_path_blocked ? 1.0f : 0.0f);
    state_data.push_back(navigation_status.current_target.x / 15000.0f);
    state_data.push_back(navigation_status.current_target.y / 15000.0f);
    state_data.push_back(navigation_status.current_target.z / 1000.0f);  // z�� �߰�
    state_data.push_back(navigation_status.final_destination.x / 15000.0f);  // final_destination �߰�
    state_data.push_back(navigation_status.final_destination.y / 15000.0f);
    state_data.push_back(navigation_status.final_destination.z / 1000.0f);
    state_data.push_back(navigation_status.estimated_travel_time / 60.0f);

    // ���� Ÿ�� (9����) - tilePos z�� �߰�
    state_data.push_back(current_tile.is_walkable ? 1.0f : 0.0f);
    state_data.push_back(current_tile.is_safe ? 1.0f : 0.0f);
    state_data.push_back(current_tile.is_enemy_spell_area ? 1.0f : 0.0f);
    state_data.push_back(current_tile.is_ally_spell_area ? 1.0f : 0.0f);
    state_data.push_back(current_tile.has_missile ? 1.0f : 0.0f);
    state_data.push_back(current_tile.is_structure ? 1.0f : 0.0f);
    state_data.push_back(current_tile.danger_level);
    state_data.push_back(current_tile.raw_flags / 65535.0f);
    state_data.push_back(current_tile.tilePos.z / 1000.0f);  // z�� �߰�

    // 8���� Ÿ�� (64���� = 8��8) - �� Ÿ���� z�� �߰�
    for (int i = 0; i < 8; ++i) {
        state_data.push_back(surrounding_tiles[i].is_walkable ? 1.0f : 0.0f);
        state_data.push_back(surrounding_tiles[i].is_safe ? 1.0f : 0.0f);
        state_data.push_back(surrounding_tiles[i].is_enemy_spell_area ? 1.0f : 0.0f);
        state_data.push_back(surrounding_tiles[i].has_missile ? 1.0f : 0.0f);
        state_data.push_back(surrounding_tiles[i].danger_level);
        state_data.push_back(best_direction_safety[i]);
        state_data.push_back(direction_blocked[i] ? 0.0f : 1.0f);
        state_data.push_back(surrounding_tiles[i].tilePos.z / 1000.0f);  // z�� �߰�
    }

    // ������ ���� (11����) - safePos z�� �߰�
    state_data.push_back(safePos.x / 15000.0f);
    state_data.push_back(safePos.y / 15000.0f);
    state_data.push_back(safePos.z / 1000.0f);  // z�� �߰�
    state_data.push_back(in_enemy_attack_range ? 1.0f : 0.0f);
    state_data.push_back(can_attack_enemy ? 1.0f : 0.0f);
    state_data.push_back(std::min(enemy_count_nearby / 5.0f, 1.0f));
    state_data.push_back(player_attack_range / 800.0f);
    state_data.push_back(player_movement_speed / 500.0f);
    state_data.push_back(is_near_ally ? 1.0f : 0.0f);
    state_data.push_back(std::min(time_since_last_combat / 30.0f, 1.0f));

    // ���� ���� �߰� ���� (3����)
    // �÷��̾�� ���� ���� ����
    float height_diff = (player_position.z - enemy_position.z) / 1000.0f;
    state_data.push_back(height_diff);

    // ���� ��ġ�� ���� ��ġ�� ���� ����
    float safe_height_diff = (player_position.z - safePos.z) / 1000.0f;
    state_data.push_back(safe_height_diff);

    // ���� Ÿ�ϰ� �ֺ� Ÿ�ϵ��� ��� ���� ����
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
    // ���̾� ���� (�� ���� ��Ʈ��ũ)
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
        // ��ġ ũ�Ⱑ 1�� �� BatchNorm ���� �ذ�
        bool use_batch_norm = x.size(0) > 1; // ��ġ ũ�Ⱑ 1���� Ŭ ���� BatchNorm ���

        auto fc1_out = fc1->forward(x);

        if (use_batch_norm) {
            x = torch::relu(bn1->forward(fc1_out));
        }
        else {
            x = torch::relu(fc1_out); // BatchNorm ��ŵ
        }

        x = dropout->forward(x);

        auto fc2_out = fc2->forward(x);

        if (use_batch_norm) {
            x = torch::relu(bn2->forward(fc2_out));
        }
        else {
            x = torch::relu(fc2_out); // BatchNorm ��ŵ
        }

        x = dropout->forward(x);
        x = torch::relu(fc3->forward(x));
        x = fc4->forward(x); // Q-�� ���

        return x;

    }
    catch (const std::exception& e) {
        std::cerr << "[DQN] Forward pass error: " << e.what() << std::endl;
        std::cerr << "[DQN] Input shape was: " << x.sizes() << std::endl;
        throw;
    }

}