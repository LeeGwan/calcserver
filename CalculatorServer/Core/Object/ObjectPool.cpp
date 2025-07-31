#include "ObjectPool.h"
#include <algorithm>
#include "Object/ObjectComponent/Object.h"
ObjectPool::ObjectPool(size_t initial_size, std::function<std::unique_ptr<Object>()> create_function)
    : create_func_(std::move(create_function))
{
    // 초기 풀 크기만큼 오브젝트 미리 생성
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

    // 사용 가능한 오브젝트 찾기
    auto it = std::find_if(pool_.begin(), pool_.end(),
        [](const PoolItem& item) {
            return item.object && item.object->can_use_obj();
        });

    if (it != pool_.end())
    {
        // 사용 가능한 오브젝트를 찾았을 경우
        it->object->Set_use_obj(false);  // 사용 중으로 마킹
        return it->object.get();
    }

    // 사용 가능한 오브젝트가 없으면 새로 생성
    pool_.emplace_back(create_func_());
    pool_.back().object->Set_use_obj(false);  // 사용 중으로 마킹
    return pool_.back().object.get();
}

void ObjectPool::release_object(Object* obj)
{
    if (!obj) return;  // nullptr 체크

    std::unique_lock lock(mutex_);

    // 해당 오브젝트를 풀에서 찾아서 해제
    auto it = std::find_if(pool_.begin(), pool_.end(),
        [obj](const PoolItem& item) {
            return item.object.get() == obj;
        });

    if (it != pool_.end())
    {
        it->object->Release();  // 오브젝트 상태 초기화
    }
}

void ObjectPool::clear()
{
    std::unique_lock lock(mutex_);

    // 모든 오브젝트 해제
    for (PoolItem& item : pool_)
    {
        if (item.object)
        {
            // 사용 중이 아닌 오브젝트만 해제 (안전성 확보)
            if (item.object->can_use_obj())
            {
                item.object->Release();
            }
            item.object.reset();  // unique_ptr 해제
        }
    }

    pool_.clear();  // 벡터 정리
    pool_.shrink_to_fit();  // 메모리 반환
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