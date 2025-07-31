#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <shared_mutex>
class Object;
/**
 * ObjectPool - ������Ʈ Ǯ���� ���� Ŭ����
 * �޸� �Ҵ�/���� ����� ���̰� ������ ����Ű�� ���� ������Ʈ�� ����
 */
class ObjectPool final
{
private:
    // ������Ʈ Ǯ ������ ����ü (���� ��븸)
    struct PoolItem {
        std::unique_ptr<Object> object;

        explicit PoolItem(std::unique_ptr<Object> obj) : object(std::move(obj)) {}
    };

public:
    // ������: �ʱ� Ǯ ũ��� ������Ʈ ���� �Լ��� ����
    explicit ObjectPool(size_t initial_size, std::function<std::unique_ptr<Object>()> create_function);

    // �Ҹ���
    ~ObjectPool();

    // ����/�̵� ������ �� ���� ������ ���� (������ Ȯ��)
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
    ObjectPool(ObjectPool&&) = delete;
    ObjectPool& operator=(ObjectPool&&) = delete;

    // Ǯ���� ��� ������ ������Ʈ�� ������ (������ ���� ����)
    Object* acquire_object();

    // ����� ���� ������Ʈ�� Ǯ�� ��ȯ
    void release_object(Object* obj);

    // ��� ������Ʈ�� �����ϰ� Ǯ�� ����
    void clear();

    // ���� Ǯ ũ�� ��ȯ
    size_t size() const;

    // ��� ������ ������Ʈ �� ��ȯ
    size_t available_count() const;

private:
    std::vector<PoolItem> pool_;                              // ������Ʈ Ǯ
    std::function<std::unique_ptr<Object>()> create_func_;    // ������Ʈ ���� �Լ�
    mutable std::shared_mutex mutex_;                         // ������ �������� ���� ���ؽ�
};


