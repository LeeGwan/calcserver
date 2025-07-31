#pragma once

#include <memory>
#include <type_traits>
#include "../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
/**
 * @brief �� �½�ũ�� �⺻ �������̽�
 * ��� �½�ũ Ÿ���� �������� �����ϱ� ���� ���� ���� �⺻ Ŭ����
 */
struct P_MapTask
{
    virtual ~P_MapTask() = default;

    /**
     * @brief �½�ũ ���� (���� ����)
     * @return ������ �½�ũ�� unique_ptr
     */
    virtual std::unique_ptr<P_MapTask> Clone() const = 0;

    /**
     * @brief �½�ũ Ÿ�� ���� ��ȯ (RTTI ��� ���)
     * @return �½�ũ Ÿ���� ��Ÿ���� ���ڿ�
     */
    virtual const char* GetTaskType() const = 0;
};

/**
 * @brief ���ø� ��� �� �½�ũ ����ü
 * Ư�� �½�ũ Ÿ��(T)�� ���� ��ü���� ������ ����
 *
 * @tparam T �½�ũ Ÿ�� (ObjectMoveOrderType, ObjectCastOrderType, ObjectSpellOrderLevellUp �� �ϳ�)
 */
template <typename T>
struct MapTask : public P_MapTask
{
    // ���� Ÿ�Ը� ����� �� �ֵ��� ������ Ÿ�� ����
    static_assert(
        std::is_same_v<T, ObjectMoveOrderType> ||
        std::is_same_v<T, ObjectCastOrderType> ,
        "MapTask: Only ObjectMoveOrderType, ObjectCastOrderType, or ObjectSpellOrderLevellUp are allowed"
        );

public:
    T Task; // ���� �½�ũ ������

    /**
     * @brief ������
     * @param task �ʱ�ȭ�� �½�ũ ������
     */
    explicit MapTask(const T& task) : Task(task) {}

    /**
     * @brief ���� ������
     * @param other ������ MapTask ��ü
     */
    MapTask(const MapTask& other) : Task(other.Task) {}
    /**
     * @brief ���� ������ (�½�ũ ������)
     * @param in_Task ������ �½�ũ ������
     * @return �ڱ� �ڽ��� ����
     */
    MapTask& operator=(const T& in_Task)
    {
        Task = in_Task;
        return *this;
    }

    /**
     * @brief ���� ���� ������
     * @param other ������ MapTask ��ü
     * @return �ڱ� �ڽ��� ����
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
     * @brief �½�ũ ���� (���� ����)
     * @return ������ �½�ũ�� unique_ptr
     */
    std::unique_ptr<P_MapTask> Clone() const override
    {
        return std::make_unique<MapTask<T>>(*this);
    }

    /**
     * @brief �½�ũ Ÿ�� ���� ��ȯ
     * @return �½�ũ Ÿ���� ��Ÿ���� ���ڿ�
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
     * @brief �½�ũ ������ ���� (�б� ����)
     * @return �½�ũ �������� const ����
     */
    const T& GetTask() const { return Task; }

    /**
     * @brief �½�ũ ������ ���� (�б�/����)
     * @return �½�ũ �������� ����
     */
    T& GetTask() { return Task; }
};

/**
 * @brief ���Ǹ� ���� Ÿ�� ��Ī��
 */
using MoveTask = MapTask<ObjectMoveOrderType>;
using CastTask = MapTask<ObjectCastOrderType>;


/**
 * @brief �½�ũ ���� ���� �Լ���
 */
namespace TaskFactory
{
    /**
     * @brief �̵� �½�ũ ����
     * @param moveOrder �̵� ��� ������
     * @return ������ �̵� �½�ũ�� unique_ptr
     */
    inline std::unique_ptr<P_MapTask> CreateMoveTask(const ObjectMoveOrderType& moveOrder)
    {
        return std::make_unique<MoveTask>(moveOrder);
    }

    /**
     * @brief ĳ��Ʈ �½�ũ ����
     * @param castOrder ĳ��Ʈ ��� ������
     * @return ������ ĳ��Ʈ �½�ũ�� unique_ptr
     */
    inline std::unique_ptr<P_MapTask> CreateCastTask(const ObjectCastOrderType& castOrder)
    {
        return std::make_unique<CastTask>(castOrder);
    }

    /**
     * @brief ���� ������ �½�ũ ����
     * @param levelUpOrder ������ ��� ������
     * @return ������ ������ �½�ũ�� unique_ptr
     */
 
}

/**
 * @brief �½�ũ Ÿ�� ĳ���� ���� �Լ���
 */
namespace TaskCast
{
    /**
     * @brief P_MapTask�� Ư�� Ÿ������ �����ϰ� ĳ����
     * @tparam T ĳ������ �½�ũ Ÿ��
     * @param task ĳ������ �½�ũ ������
     * @return ĳ���õ� �½�ũ ������ (���н� nullptr)
     */
    template<typename T>
    MapTask<T>* SafeCast(P_MapTask* task)
    {
        return dynamic_cast<MapTask<T>*>(task);
    }

    /**
     * @brief P_MapTask�� Ư�� Ÿ������ �����ϰ� ĳ���� (const ����)
     * @tparam T ĳ������ �½�ũ Ÿ��
     * @param task ĳ������ �½�ũ ������
     * @return ĳ���õ� �½�ũ ������ (���н� nullptr)
     */
    template<typename T>
    const MapTask<T>* SafeCast(const P_MapTask* task)
    {
        return dynamic_cast<const MapTask<T>*>(task);
    }
}


