#include"RLAgent.h"
#include "../AIflag/AIflags.h"
#include <iostream>
#include<Windows.h>
RLAgent::RLAgent() :
    policy_net(GameStateInfo::STATE_SIZE, static_cast<int>(AIAction::TOTAL_ACTIONS)),
    target_net(GameStateInfo::STATE_SIZE, static_cast<int>(AIAction::TOTAL_ACTIONS)),
    replay_buffer(50000),
    device(torch::kCPU),  // CPU로 고정
    epsilon(1.0),
    gen(std::random_device{}()) {

    std::cout << "Torch version: " << TORCH_VERSION << std::endl;
    std::cout << "[RL] Using device: CPU" << std::endl;
    std::cout << "[RL] State size: " << GameStateInfo::STATE_SIZE << std::endl;
    std::cout << "[RL] Action size: " << static_cast<int>(AIAction::TOTAL_ACTIONS) << std::endl;

    // 네트워크를 CPU로 이동
    policy_net->to(device);
    target_net->to(device);

    // 옵티마이저 생성
    optimizer = std::make_unique<torch::optim::Adam>(policy_net->parameters(), learning_rate);

    // 타겟 네트워크 초기화
    update_target_network();

    std::cout << "[RL] DQN Agent initialized successfully on CPU" << std::endl;
}

RLAgent::~RLAgent() = default;


AIAction RLAgent::select_action(const std::string& hash, const GameStateInfo& game_state) {
    try {
        auto tensor_data = game_state.to_tensor();
        auto state_tensor = tensor_data.unsqueeze(0).to(device);

        // === 디버깅 코드 추가 시작 ===
        std::vector<bool> action_mask = get_action_mask(game_state);
    
        // === 디버깅 코드 추가 끝 ===

        std::uniform_real_distribution<> dis(0.0, 1.0);
        double random_value = dis(gen);

        if (random_value < epsilon) {
            auto result=select_random_valid_action(action_mask);
     
            return result; //select_random_valid_action(action_mask);
        }
        else {
           
            torch::NoGradGuard no_grad;
            auto q_values = policy_net->forward(state_tensor);
            auto masked_q_values = q_values.clone();

            for (int i = 0; i < static_cast<int>(AIAction::TOTAL_ACTIONS); ++i) {
                if (!action_mask[i]) {
                    masked_q_values[0][i] = -1000.0f;
                }
            }
            auto result = static_cast<AIAction>(masked_q_values.argmax(1).item<int>());

            return result;
        }
    }
    catch (const c10::Error& e) {
        std::cerr << "[ERROR] c10::Error in select_action: " << e.what() << std::endl;
        std::vector<bool> action_mask = get_action_mask(game_state);
        return select_random_valid_action(action_mask);
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Standard exception: " << e.what() << std::endl;
        std::vector<bool> action_mask = get_action_mask(game_state);
        return select_random_valid_action(action_mask);
    }

}

AIAction RLAgent::select_greedy_action(const GameStateInfo& game_state) {
    auto state_tensor = game_state.to_tensor().unsqueeze(0).to(device);
    std::vector<bool> action_mask = get_action_mask(game_state);

    torch::NoGradGuard no_grad;
    auto q_values = policy_net->forward(state_tensor);

    auto masked_q_values = q_values.clone();
    for (int i = 0; i < static_cast<int>(AIAction::TOTAL_ACTIONS); ++i) {
        if (!action_mask[i]) {
            masked_q_values[0][i] = -1000.0f;
        }
    }

    return static_cast<AIAction>(masked_q_values.argmax(1).item<int>());
}

void RLAgent::store_experience(const GameStateInfo& state, AIAction action, float reward,
    const GameStateInfo& next_state, bool done) {
    auto state_tensor = state.to_tensor();
    auto next_state_tensor = next_state.to_tensor();

    replay_buffer.push(state_tensor, static_cast<int>(action), reward, next_state_tensor, done);

    total_reward += reward;
    episode_length++;

    if (done) {
        episode_rewards.push_back(total_reward);
        episode_lengths.push_back(episode_length);
        episodes_done++;

        check_performance_improvement();

        if (episodes_done % 10 == 0) {
            print_training_stats();
        }

        total_reward = 0.0f;
        episode_length = 0;
    }
}

void RLAgent::learn() {
    if (replay_buffer.size() < batch_size) return;

    auto batch = replay_buffer.priority_sample(batch_size);
    if (batch.empty()) return;

    std::vector<torch::Tensor> states, next_states;
    std::vector<int> actions;
    std::vector<float> rewards;
    std::vector<bool> dones;

    for (const auto& exp : batch) {
        states.push_back(exp.state);
        actions.push_back(exp.action);
        rewards.push_back(exp.reward);
        next_states.push_back(exp.next_state);
        dones.push_back(exp.done);
    }

    auto state_batch = torch::stack(states).to(device);
    auto next_state_batch = torch::stack(next_states).to(device);
    auto action_batch = torch::tensor(actions, torch::kLong).to(device);
    auto reward_batch = torch::tensor(rewards, torch::kFloat32).to(device);

    auto q_values = policy_net->forward(state_batch);
    auto state_action_values = q_values.gather(1, action_batch.unsqueeze(1));

    torch::Tensor next_state_values = torch::zeros({ batch_size }, device);
    {
        torch::NoGradGuard no_grad;
        auto next_q_values_policy = policy_net->forward(next_state_batch);
        auto next_actions = next_q_values_policy.argmax(1);
        auto next_q_values_target = target_net->forward(next_state_batch);
        auto selected_next_q_values = next_q_values_target.gather(1, next_actions.unsqueeze(1));

        for (int i = 0; i < batch_size; ++i) {
            if (!dones[i]) {
                next_state_values[i] = selected_next_q_values[i][0];
            }
        }
    }

    auto expected_q_values = reward_batch + (gamma * next_state_values);
    auto loss = torch::smooth_l1_loss(state_action_values.squeeze(), expected_q_values);

    optimizer->zero_grad();
    loss.backward();
    torch::nn::utils::clip_grad_norm_(policy_net->parameters(), 1.0);
    optimizer->step();

    running_loss = running_loss * 0.99f + loss.item<float>() * 0.01f;
    steps_done++;

    if (steps_done % target_update_freq == 0) {
        update_target_network();
    }

    if (epsilon > epsilon_min) {
        epsilon *= epsilon_decay;
    }
}

void RLAgent::save_model(const std::string& path) {
    
    try {
        torch::serialize::OutputArchive archive;

        auto params = policy_net->named_parameters();
        for (const auto& param : params) {
            archive.write(param.key(), param.value().cpu());
        }
        archive.write("_meta_epsilon", torch::tensor(epsilon, torch::kFloat64));      // double
        archive.write("_meta_steps", torch::tensor(steps_done, torch::kInt32));       // int
        archive.write("_meta_episodes", torch::tensor(episodes_done, torch::kInt32)); // int
        archive.write("_meta_best_avg_reward", torch::tensor(best_avg_reward, torch::kFloat32)); // float
        archive.write("_meta_state_size", torch::tensor(GameStateInfo::STATE_SIZE, torch::kInt32)); // int
        archive.write("_meta_action_size", torch::tensor(static_cast<int>(AIAction::TOTAL_ACTIONS), torch::kInt32));

        archive.save_to(path);
        // save_training_stats(path);

        std::cout << "[RL] Model saved to " << path << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "[RL] Error saving model: " << e.what() << std::endl;
    }
}

void RLAgent::load_model(const std::string& path) {
    try {
        torch::serialize::InputArchive archive;
        archive.load_from(path);

        // 임시 변수 선언
        at::Tensor temp_tensor;

        // 메타 정보 읽기
        archive.read("_meta_state_size", temp_tensor);
        int saved_state_size = temp_tensor.item<int>();

        archive.read("_meta_action_size", temp_tensor);
        int saved_action_size = temp_tensor.item<int>();

        if (saved_state_size != GameStateInfo::STATE_SIZE ||
            saved_action_size != static_cast<int>(AIAction::TOTAL_ACTIONS)) {
            std::cerr << "[RL] Model dimension mismatch!" << std::endl;
            return;
        }

        archive.read("_meta_epsilon", temp_tensor);
        epsilon = temp_tensor.item<double>();

        archive.read("_meta_steps", temp_tensor);
        steps_done = temp_tensor.item<int>();

        archive.read("_meta_episodes", temp_tensor);
        episodes_done = temp_tensor.item<int>();

        archive.read("_meta_best_avg_reward", temp_tensor);
        best_avg_reward = temp_tensor.item<float>();

        // 네트워크 파라미터 로드
        auto params = policy_net->named_parameters();
        for (auto& param : params) {
            at::Tensor t;
            // 키가 존재하면 읽기 시도 (예외처리 추가 가능)
            try {
                archive.read(param.key(), t);
                param.value().copy_(t.to(device));
            }
            catch (const c10::Error& e) {
                std::cerr << "[RL] Parameter " << param.key() << " not found in archive." << std::endl;
            }
        }

        update_target_network();

        std::cout << "[RL] Model loaded from " << path << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "[RL] Error loading model: " << e.what() << std::endl;
    }
}


double RLAgent::get_epsilon() const { return epsilon; }
int RLAgent::get_episodes_done() const { return episodes_done; }
int RLAgent::get_steps_done() const { return steps_done; }
float RLAgent::get_best_avg_reward() const { return best_avg_reward; }
float RLAgent::get_running_loss() const { return running_loss; }

void RLAgent::set_training_mode(bool training) {
    if (training) {
        policy_net->train();
        target_net->train();
    }
    else {
        policy_net->eval();
        target_net->eval();
    }
}

void RLAgent::clear_replay_buffer()
{
    replay_buffer.clear();

}

void RLAgent::force_save_final_model(const std::string& path)
{
    try {
        save_model(path);
        std::cout << "[RL] Final model saved to: " << path << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[RL] Failed to save final model: " << e.what() << std::endl;
    }
}



void RLAgent::update_target_network() {
    torch::NoGradGuard no_grad;
    auto target_params = target_net->named_parameters();
    auto policy_params = policy_net->named_parameters();

    const float tau = 0.005f;

    for (auto& target_param : target_params) {
        for (auto& policy_param : policy_params) {
            if (target_param.key() == policy_param.key()) {
                target_param.value().copy_(
                    tau * policy_param.value() + (1.0f - tau) * target_param.value()
                );
            }
        }
    }
}

std::vector<bool> RLAgent::get_action_mask(const GameStateInfo& game_state) {
    std::vector<bool> mask(static_cast<int>(AIAction::TOTAL_ACTIONS), true);

    // === 1단계: 물리적 제약 ===

    // 스킬 쿨다운 및 마나 체크
    mask[static_cast<int>(AIAction::SKILL_Q)] = game_state.skill_status.q_ready &&
        game_state.skill_status.has_enough_mana_q;
    mask[static_cast<int>(AIAction::SKILL_E)] = game_state.skill_status.e_ready &&
        game_state.skill_status.has_enough_mana_e && !game_state.skill_status.forblink[2].is_valid();
    mask[static_cast<int>(AIAction::SKILL_R)] = game_state.skill_status.r_ready &&
        game_state.skill_status.has_enough_mana_r;

    // 내비게이션 경로 존재 여부
    mask[static_cast<int>(AIAction::FOLLOW_NAVIGATION)] = game_state.navigation_status.has_navigation_path;
    mask[static_cast<int>(AIAction::CANCEL_NAVIGATION)] = game_state.navigation_status.has_navigation_path;

    // === 2단계: 방향 막힘 체크 ===

    // 막힌 방향 차단
    for (int i = 0; i < 8; ++i) {
        if (game_state.direction_blocked[i]) {
            mask[i] = false;
        }
    }

    // === 3단계: 적군 존재 여부에 따른 제약 ===

    bool has_valid_enemy = game_state.has_enemy_target && game_state.enemy_hp_ratio > 0.0f;

    // 스킬은 적군이 있을 때만 (E스킬은 예외 - 이동기)
    if (!has_valid_enemy) {
        mask[static_cast<int>(AIAction::SKILL_Q)] = false;
        mask[static_cast<int>(AIAction::SKILL_R)] = false;
        // E스킬은 이동용이므로 적군 없어도 허용
    }

    // 공격 관련
    mask[static_cast<int>(AIAction::ATTACK_TARGET)] = has_valid_enemy && game_state.can_attack_enemy;
    mask[static_cast<int>(AIAction::MOVE_TO_ENEMY)] = has_valid_enemy;

    // === 4단계: 후퇴 가능 여부 ===

    // 후퇴는 안전한 위치가 있을 때만 가능
    mask[static_cast<int>(AIAction::RETREAT_SAFE)] = game_state.safePos.is_valid();

    // === 5단계:  응급 상황 처리 (모든 방향이 막혔을 때) ===

    bool all_directions_blocked = true;
    for (int i = 0; i < 8; ++i) {
        if (!game_state.direction_blocked[i]) {
            all_directions_blocked = false;
            break;
        }
    }

    if (all_directions_blocked) {
        // 가장 안전한 방향 하나는 강제로 허용
        float best_safety = -1.0f;
        int best_direction = 0;
        for (int i = 0; i < 8; ++i) {
            if (game_state.best_direction_safety[i] > best_safety) {
                best_safety = game_state.best_direction_safety[i];
                best_direction = i;
            }
        }
        mask[best_direction] = true;

        //  응급 액션들 강제 활성화
        mask[static_cast<int>(AIAction::STAY_POSITION)] = true;

        // 안전한 위치가 있다면 후퇴도 허용
        if (game_state.safePos.is_valid()) {
            mask[static_cast<int>(AIAction::RETREAT_SAFE)] = true;
        }

        // E스킬(순간이동)이 가능하다면 강제 허용 (탈출용)
        if (game_state.skill_status.e_ready &&
            game_state.skill_status.has_enough_mana_e &&
            game_state.skill_status.forblink[2].is_valid()) {
            mask[static_cast<int>(AIAction::SKILL_E)] = true;
        }
    }
    return mask;
}

AIAction RLAgent::select_random_valid_action(const std::vector<bool>& action_mask) {
    std::vector<int> valid_actions;
    for (int i = 0; i < static_cast<int>(AIAction::TOTAL_ACTIONS); ++i) {
        if (action_mask[i]) {
            valid_actions.push_back(i);
        }
    }

    if (valid_actions.empty()) {
        return AIAction::STAY_POSITION;
    }

    std::uniform_int_distribution<> dis(0, valid_actions.size() - 1);
    return static_cast<AIAction>(valid_actions[dis(gen)]);
}

void RLAgent::check_performance_improvement() {
    if (episode_rewards.size() >= 100) {
        float recent_avg = 0.0f;
        for (int i = episode_rewards.size() - 100; i < episode_rewards.size(); ++i) {
            recent_avg += episode_rewards[i];
        }
        recent_avg /= 100.0f;

        if (recent_avg > best_avg_reward) {
            best_avg_reward = recent_avg;
            episodes_since_improvement = 0;
            save_model("./models/best_model.pt");
        }
        else {
            episodes_since_improvement++;
        }
    }
}

void RLAgent::print_training_stats() {
    if (episode_rewards.empty()) return;

    float recent_avg = 0.0f;
    int recent_count = (std::min)(100, (int)episode_rewards.size());
    int start_idx = episode_rewards.size() - recent_count;

    for (int i = start_idx; i < episode_rewards.size(); ++i) {
        recent_avg += episode_rewards[i];
    }
    recent_avg /= recent_count;

    std::cout << "[RL] Episode " << episodes_done
        << " | Avg Reward: " << recent_avg
        << " | Epsilon: " << epsilon
        << " | Loss: " << running_loss << std::endl;
}
/*
void RLAgent::save_training_stats(const std::string& model_path) {
    std::string stats_path = model_path.substr(0, model_path.find_last_of('.')) + "_stats.csv";
    std::ofstream stats_file(stats_path);

    stats_file << "Episode,Reward,Length,Epsilon,Loss\n";
   
    for (size_t i = 0; i < episode_rewards.size(); ++i) {
        float eps = 1.0 * std::pow(epsilon_decay, i);
        stats_file << i + 1 << ","
            << episode_rewards[i] << ","
            << (i < episode_lengths.size() ? episode_lengths[i] : 0) << ","
            << eps << ","
            << running_loss << "\n";
    }

    stats_file.close();
}*/