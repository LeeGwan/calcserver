#pragma once
#include "../../utils/Vector.h"
#include <torch/torch.h>
#include <string>
#include "../../Object/Object/Spells/SpellFlags/SpellFlags.h"
struct DirectionOffset {
    float x, y;
    std::string name;
};

// ��ų ���� ���� ����ü
struct SkillStatusInfo {
    SpellType skilltype[4];
    bool q_ready, e_ready, r_ready;
    float q_cooldown_ratio, e_cooldown_ratio, r_cooldown_ratio;
    float q_mana_cost, e_mana_cost, r_mana_cost;
    bool has_enough_mana_q, has_enough_mana_e, has_enough_mana_r;
    unsigned int q_level, e_level, r_level;
    Vec3 forblink[4];
    SkillStatusInfo();
};

// �׺���̼� ���� ���� ����ü
struct NavigationStatusInfo {
    bool has_navigation_path;
    int total_waypoints, current_waypoint_index;
    float progress_ratio, distance_to_target, distance_to_final;
    Vec3 current_target, final_destination;
    bool is_path_blocked;
    float estimated_travel_time;

    NavigationStatusInfo();
};

// NavMesh Ÿ�� ���� ����ü
struct TileInfo {
    bool is_walkable, is_safe, is_enemy_spell_area, is_ally_spell_area;
    bool has_missile, is_structure;
    float danger_level;
    unsigned short raw_flags;
    Vec3 tilePos;
    TileInfo();
};

// ���� ���� ����ü
struct GameStateInfo {
    // �÷��̾� �⺻ ����
    Vec3 player_position;
    float player_hp_ratio, player_mp_ratio;
    int player_team;
    float search_radius;
    // �� ����
    std::string enemy_hash;
    Vec3 enemy_position;
    float enemy_hp_ratio, distance_to_enemy;
    bool has_enemy_target;

    // ��ų�� �׺���̼� ����
    SkillStatusInfo skill_status;
    NavigationStatusInfo navigation_status;

    // NavMesh ����
    TileInfo current_tile;
    TileInfo surrounding_tiles[8];

    // ������ ����
    Vec3 safePos;
    bool in_enemy_attack_range, can_attack_enemy;
    int enemy_count_nearby;
    float best_direction_safety[8];
    bool direction_blocked[8];
    float player_attack_range, player_movement_speed;
    bool is_near_ally;
    float time_since_last_combat;

    // �ټ� ��ȯ
    torch::Tensor to_tensor() const;
    static constexpr int STATE_SIZE = 143;
};


// DQN �Ű�� ����
struct DQNImpl : torch::nn::Module {
    torch::nn::Linear fc1{ nullptr }, fc2{ nullptr }, fc3{ nullptr }, fc4{ nullptr };
    torch::nn::BatchNorm1d bn1{ nullptr }, bn2{ nullptr };
    torch::nn::Dropout dropout{ nullptr };

    DQNImpl(int64_t input_size, int64_t output_size);
    torch::Tensor forward(torch::Tensor x);
};
TORCH_MODULE(DQN);
struct safeposINFO
{
    Vec3 safepos;
    float safety;
    bool blocked;
};