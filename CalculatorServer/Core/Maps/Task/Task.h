#pragma once

#include <memory>
#include <type_traits>
#include "../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
/**
 * @brief 맵 태스크의 기본 인터페이스
 * 모든 태스크 타입의 다형성을 지원하기 위한 순수 가상 기본 클래스
 */
struct P_MapTask
{
    virtual ~P_MapTask() = default;

    /**
     * @brief 태스크 복제 (깊은 복사)
     * @return 복제된 태스크의 unique_ptr
     */
    virtual std::unique_ptr<P_MapTask> Clone() const = 0;

    /**
     * @brief 태스크 타입 정보 반환 (RTTI 대신 사용)
     * @return 태스크 타입을 나타내는 문자열
     */
    virtual const char* GetTaskType() const = 0;
};

/**
 * @brief 템플릿 기반 맵 태스크 구현체
 * 특정 태스크 타입(T)에 대한 구체적인 구현을 제공
 *
 * @tparam T 태스크 타입 (ObjectMoveOrderType, ObjectCastOrderType, ObjectSpellOrderLevellUp 중 하나)
 */
template <typename T>
struct MapTask : public P_MapTask
{
    // 허용된 타입만 사용할 수 있도록 컴파일 타임 검증
    static_assert(
        std::is_same_v<T, ObjectMoveOrderType> ||
        std::is_same_v<T, ObjectCastOrderType> ,
        "MapTask: Only ObjectMoveOrderType, ObjectCastOrderType, or ObjectSpellOrderLevellUp are allowed"
        );

public:
    T Task; // 실제 태스크 데이터

    /**
     * @brief 생성자
     * @param task 초기화할 태스크 데이터
     */
    explicit MapTask(const T& task) : Task(task) {}

    /**
     * @brief 복사 생성자
     * @param other 복사할 MapTask 객체
     */
    MapTask(const MapTask& other) : Task(other.Task) {}
    /**
     * @brief 대입 연산자 (태스크 데이터)
     * @param in_Task 대입할 태스크 데이터
     * @return 자기 자신의 참조
     */
    MapTask& operator=(const T& in_Task)
    {
        Task = in_Task;
        return *this;
    }

    /**
     * @brief 복사 대입 연산자
     * @param other 복사할 MapTask 객체
     * @return 자기 자신의 참조
     */
    MapTask& operator=(const MapTask& other)
    {
        if (this != &other)
        {
            Task = other.Task;
        }
        return *this;
    }

    /**
     * @brief 태스크 복제 (깊은 복사)
     * @return 복제된 태스크의 unique_ptr
     */
    std::unique_ptr<P_MapTask> Clone() const override
    {
        return std::make_unique<MapTask<T>>(*this);
    }

    /**
     * @brief 태스크 타입 정보 반환
     * @return 태스크 타입을 나타내는 문자열
     */
    const char* GetTaskType() const override
    {
        if constexpr (std::is_same_v<T, ObjectMoveOrderType>)
            return "ObjectMoveOrderType";
        else if constexpr (std::is_same_v<T, ObjectCastOrderType>)
            return "ObjectCastOrderType";

        else
            return "Unknown";
    }

    /**
     * @brief 태스크 데이터 접근 (읽기 전용)
     * @return 태스크 데이터의 const 참조
     */
    const T& GetTask() const { return Task; }

    /**
     * @brief 태스크 데이터 접근 (읽기/쓰기)
     * @return 태스크 데이터의 참조
     */
    T& GetTask() { return Task; }
};

/**
 * @brief 편의를 위한 타입 별칭들
 */
using MoveTask = MapTask<ObjectMoveOrderType>;
using CastTask = MapTask<ObjectCastOrderType>;


/**
 * @brief 태스크 생성 헬퍼 함수들
 */
namespace TaskFactory
{
    /**
     * @brief 이동 태스크 생성
     * @param moveOrder 이동 명령 데이터
     * @return 생성된 이동 태스크의 unique_ptr
     */
    inline std::unique_ptr<P_MapTask> CreateMoveTask(const ObjectMoveOrderType& moveOrder)
    {
        return std::make_unique<MoveTask>(moveOrder);
    }

    /**
     * @brief 캐스트 태스크 생성
     * @param castOrder 캐스트 명령 데이터
     * @return 생성된 캐스트 태스크의 unique_ptr
     */
    inline std::unique_ptr<P_MapTask> CreateCastTask(const ObjectCastOrderType& castOrder)
    {
        return std::make_unique<CastTask>(castOrder);
    }

    /**
     * @brief 스펠 레벨업 태스크 생성
     * @param levelUpOrder 레벨업 명령 데이터
     * @return 생성된 레벨업 태스크의 unique_ptr
     */
 
}

/**
 * @brief 태스크 타입 캐스팅 헬퍼 함수들
 */
namespace TaskCast
{
    /**
     * @brief P_MapTask를 특정 타입으로 안전하게 캐스팅
     * @tparam T 캐스팅할 태스크 타입
     * @param task 캐스팅할 태스크 포인터
     * @return 캐스팅된 태스크 포인터 (실패시 nullptr)
     */
    template<typename T>
    MapTask<T>* SafeCast(P_MapTask* task)
    {
        return dynamic_cast<MapTask<T>*>(task);
    }

    /**
     * @brief P_MapTask를 특정 타입으로 안전하게 캐스팅 (const 버전)
     * @tparam T 캐스팅할 태스크 타입
     * @param task 캐스팅할 태스크 포인터
     * @return 캐스팅된 태스크 포인터 (실패시 nullptr)
     */
    template<typename T>
    const MapTask<T>* SafeCast(const P_MapTask* task)
    {
        return dynamic_cast<const MapTask<T>*>(task);
    }
}


