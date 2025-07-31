#pragma once
#include <deque>
#include <mutex>
#include <cstddef> 
#include <vector>
#include <torch/torch.h>
// 경험 구조체
struct Experience {
    torch::Tensor state, next_state;
    int action;
    float reward;
    bool done;

    Experience(torch::Tensor s, int a, float r, torch::Tensor ns, bool d);
};
class ReplayBuffer {
private:
    std::deque<Experience> buffer;
    size_t capacity;
    mutable std::mutex buffer_mutex;

public:
    explicit ReplayBuffer(size_t cap);
    ~ReplayBuffer();
    // 경험 추가
    void push(torch::Tensor state, int action, float reward,
        torch::Tensor next_state, bool done);

    // 랜덤 배치 샘플링
    std::vector<Experience> sample(size_t batch_size);

    // 우선순위 샘플링
    std::vector<Experience> priority_sample(size_t batch_size, float alpha = 0.6f);

    // 버퍼 크기 반환
    size_t size() const;

    // 버퍼 초기화
    void clear();

    // 최근 경험들의 평균 보상 계산
    float get_recent_avg_reward(size_t recent_count = 100) const;
};
