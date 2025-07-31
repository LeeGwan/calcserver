#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <shared_mutex>
class Object;
/**
 * ObjectPool - 오브젝트 풀링을 위한 클래스
 * 메모리 할당/해제 비용을 줄이고 성능을 향상시키기 위해 오브젝트를 재사용
 */
class ObjectPool final
{
private:
    // 오브젝트 풀 아이템 구조체 (내부 사용만)
    struct PoolItem {
        std::unique_ptr<Object> object;

        explicit PoolItem(std::unique_ptr<Object> obj) : object(std::move(obj)) {}
    };

public:
    // 생성자: 초기 풀 크기와 오브젝트 생성 함수를 받음
    explicit ObjectPool(size_t initial_size, std::function<std::unique_ptr<Object>()> create_function);

    // 소멸자
    ~ObjectPool();

    // 복사/이동 생성자 및 대입 연산자 삭제 (안전성 확보)
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
    ObjectPool(ObjectPool&&) = delete;
    ObjectPool& operator=(ObjectPool&&) = delete;

    // 풀에서 사용 가능한 오브젝트를 가져옴 (없으면 새로 생성)
    Object* acquire_object();

    // 사용이 끝난 오브젝트를 풀로 반환
    void release_object(Object* obj);

    // 모든 오브젝트를 해제하고 풀을 정리
    void clear();

    // 현재 풀 크기 반환
    size_t size() const;

    // 사용 가능한 오브젝트 수 반환
    size_t available_count() const;

private:
    std::vector<PoolItem> pool_;                              // 오브젝트 풀
    std::function<std::unique_ptr<Object>()> create_func_;    // 오브젝트 생성 함수
    mutable std::shared_mutex mutex_;                         // 스레드 안전성을 위한 뮤텍스
};


