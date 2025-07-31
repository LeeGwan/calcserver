#pragma once
#include <memory>
#include <random>
#include <torch/torch.h>
#include "../ReplayBuffer/ReplayBuffer.h"
#include "../RLStructures/RLStructures.h"
enum class AIAction : int;
class RLAgent {
private:
    DQN policy_net;
    DQN target_net;
    std::unique_ptr<torch::optim::Adam> optimizer;
    ReplayBuffer replay_buffer;
    torch::Device device;
    
    // 하이퍼파라미터
    double epsilon;
    const double epsilon_decay = 0.9995;
    const double epsilon_min = 0.1;
    const double gamma = 0.99;
    const int batch_size = 64;
    const int target_update_freq = 100;
    const double learning_rate = 0.001;

    std::mt19937 gen;
    int steps_done = 0;
    int episodes_done = 0;

    // 학습 통계
    std::vector<float> episode_rewards;
    std::vector<int> episode_lengths;
    std::vector<float> episode_losses;
    float total_reward = 0.0f;
    int episode_length = 0;
    float running_loss = 0.0f;

    // 성능 모니터링
    float best_avg_reward = -std::numeric_limits<float>::infinity();
    int episodes_since_improvement = 0;
    const int patience = 500;

public:
    RLAgent();
    ~RLAgent();
    // 행동 선택
    AIAction select_action(const std::string& hash, const GameStateInfo& game_state);
    AIAction select_greedy_action(const GameStateInfo& game_state);

    // 경험 저장 및 학습
    void store_experience(const GameStateInfo& state, AIAction action, float reward,
        const GameStateInfo& next_state, bool done);
    void learn();

    // 모델 저장/로드
    void save_model(const std::string& path);
    void load_model(const std::string& path);

    // Getter 메서드들
    double get_epsilon() const;
    int get_episodes_done() const;
    int get_steps_done() const;
    float get_best_avg_reward() const;
    float get_running_loss() const;

    // 모드 제어
    void set_training_mode(bool training);
    void clear_replay_buffer(); // 리플레이 버퍼 정리
    void force_save_final_model(const std::string& path); // 강제 최종 모델 저장

private:
    // 내부 메서드들
    void update_target_network();
    std::vector<bool> get_action_mask(const GameStateInfo& game_state);
    AIAction select_random_valid_action(const std::vector<bool>& action_mask);
    void check_performance_improvement();
    void print_training_stats();
   // void save_training_stats(const std::string& model_path);
};