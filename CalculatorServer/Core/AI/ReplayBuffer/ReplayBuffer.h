#pragma once
#include <deque>
#include <mutex>
#include <cstddef> 
#include <vector>
#include <torch/torch.h>
// ���� ����ü
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
    // ���� �߰�
    void push(torch::Tensor state, int action, float reward,
        torch::Tensor next_state, bool done);

    // ���� ��ġ ���ø�
    std::vector<Experience> sample(size_t batch_size);

    // �켱���� ���ø�
    std::vector<Experience> priority_sample(size_t batch_size, float alpha = 0.6f);

    // ���� ũ�� ��ȯ
    size_t size() const;

    // ���� �ʱ�ȭ
    void clear();

    // �ֱ� ������� ��� ���� ���
    float get_recent_avg_reward(size_t recent_count = 100) const;
};
