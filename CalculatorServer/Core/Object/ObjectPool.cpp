#include "ObjectPool.h"
#include <algorithm>
#include "Object/ObjectComponent/Object.h"
ObjectPool::ObjectPool(size_t initial_size, std::function<std::unique_ptr<Object>()> create_function)
    : create_func_(std::move(create_function))
{
    // �ʱ� Ǯ ũ�⸸ŭ ������Ʈ �̸� ����
    pool_.reserve(initial_size);
    for (size_t i = 0; i < initial_size; ++i)
    {
        pool_.emplace_back(create_func_());
    }
}

ObjectPool::~ObjectPool() = default;


Object* ObjectPool::acquire_object()
{
    std::unique_lock lock(mutex_);

    // ��� ������ ������Ʈ ã��
    auto it = std::find_if(pool_.begin(), pool_.end(),
        [](const PoolItem& item) {
            return item.object && item.object->can_use_obj();
        });

    if (it != pool_.end())
    {
        // ��� ������ ������Ʈ�� ã���� ���
        it->object->Set_use_obj(false);  // ��� ������ ��ŷ
        return it->object.get();
    }

    // ��� ������ ������Ʈ�� ������ ���� ����
    pool_.emplace_back(create_func_());
    pool_.back().object->Set_use_obj(false);  // ��� ������ ��ŷ
    return pool_.back().object.get();
}

void ObjectPool::release_object(Object* obj)
{
    if (!obj) return;  // nullptr üũ

    std::unique_lock lock(mutex_);

    // �ش� ������Ʈ�� Ǯ���� ã�Ƽ� ����
    auto it = std::find_if(pool_.begin(), pool_.end(),
        [obj](const PoolItem& item) {
            return item.object.get() == obj;
        });

    if (it != pool_.end())
    {
        it->object->Release();  // ������Ʈ ���� �ʱ�ȭ
    }
}

void ObjectPool::clear()
{
    std::unique_lock lock(mutex_);

    // ��� ������Ʈ ����
    for (PoolItem& item : pool_)
    {
        if (item.object)
        {
            // ��� ���� �ƴ� ������Ʈ�� ���� (������ Ȯ��)
            if (item.object->can_use_obj())
            {
                item.object->Release();
            }
            item.object.reset();  // unique_ptr ����
        }
    }

    pool_.clear();  // ���� ����
    pool_.shrink_to_fit();  // �޸� ��ȯ
}

size_t ObjectPool::size() const
{
    std::shared_lock lock(mutex_);
    return pool_.size();
}

size_t ObjectPool::available_count() const
{
    std::shared_lock lock(mutex_);

    return std::count_if(pool_.begin(), pool_.end(),
        [](const PoolItem& item) {
            return item.object && item.object->can_use_obj();
        });
}