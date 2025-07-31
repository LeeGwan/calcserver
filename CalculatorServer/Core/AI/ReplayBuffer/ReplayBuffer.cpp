#include "ReplayBuffer.h"
#include <random>
#include <algorithm>
// Experience 생성자
Experience::Experience(torch::Tensor s, int a, float r, torch::Tensor ns, bool d)
    : state(s), action(a), reward(r), next_state(ns), done(d) {
}
ReplayBuffer::ReplayBuffer(size_t cap) : capacity(cap) {}

ReplayBuffer::~ReplayBuffer() = default;


void ReplayBuffer::push(torch::Tensor state, int action, float reward,
    torch::Tensor next_state, bool done) {
    std::lock_guard<std::mutex> lock(buffer_mutex);

    if (buffer.size() >= capacity) {
        buffer.pop_front(); // 오래된 경험 제거
    }
    buffer.emplace_back(state, action, reward, next_state, done);
}

std::vector<Experience> ReplayBuffer::sample(size_t batch_size) {
    std::lock_guard<std::mutex> lock(buffer_mutex);

    if (buffer.size() < batch_size) {
        return {}; // 충분한 경험이 없으면 빈 벡터 반환
    }

    std::vector<Experience> batch;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, buffer.size() - 1);

    for (size_t i = 0; i < batch_size; ++i) {
        batch.push_back(buffer[dis(gen)]);
    }
    return batch;
}

std::vector<Experience> ReplayBuffer::priority_sample(size_t batch_size, float alpha) {
    std::lock_guard<std::mutex> lock(buffer_mutex);

    if (buffer.size() < batch_size) {
        return {};
    }

    std::vector<Experience> batch;
    std::vector<float> priorities;

    // 우선순위 계산 (보상의 절댓값을 기준으로)
    for (const auto& exp : buffer) {
        float priority = std::abs(exp.reward) + 0.01f; // 작은 상수 추가
        priorities.push_back(std::pow(priority, alpha));
    }

    // 확률 분포 생성
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> dist(priorities.begin(), priorities.end());

    for (size_t i = 0; i < batch_size; ++i) {
        int selected_idx = dist(gen);
        batch.push_back(buffer[selected_idx]);
    }

    return batch;
}

size_t ReplayBuffer::size() const {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    return buffer.size();
}

void ReplayBuffer::clear() {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    buffer.clear();
}

float ReplayBuffer::get_recent_avg_reward(size_t recent_count) const {
    std::lock_guard<std::mutex> lock(buffer_mutex);

    if (buffer.empty()) return 0.0f;

    size_t count = (std::min)(recent_count, buffer.size());
    float total_reward = 0.0f;

    for (auto it = buffer.end() - count; it != buffer.end(); ++it) {
        total_reward += it->reward;
    }

    return total_reward / count;
}