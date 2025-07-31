#pragma once
#include <unordered_map>
#include <string>
#include <memory>

namespace ErrorCode { enum Input_ErrorType : uint8_t; };
namespace Enums { enum CurrentOrderType : uint8_t; };
class VectorUtils;
class DamageLib;
class Object;
class dtNavMeshQuery;
class NavMesh;
class VMT_For_OBJ;
class Vec3;
struct MoveOrder;

class MoveAndAttackComponent final
{
public:
    MoveAndAttackComponent();
    ~MoveAndAttackComponent();

    // ��� �ʱ�ȭ - ��� ���� �� ���� �ʱ�ȭ
    ErrorCode::Input_ErrorType InitializeOrderPath(NavMesh* nav_mesh, Object* hero, VMT_For_OBJ* vmt,
        const Enums::CurrentOrderType& order_type,
        const MoveOrder& task, const Vec3& start_pos,
        const Vec3& end_pos, Object* target,
        const float& map_time);

    // ��� ó�� ��ƾ - �� �����Ӹ��� ȣ��Ǿ� ��� ����
    ErrorCode::Input_ErrorType ProcessOrderRoutine(NavMesh* nav_mesh, Object* hero, VMT_For_OBJ* vmt,
        const Enums::CurrentOrderType& order_type,
        const std::unordered_map<std::string, Object*>& objects,
        const float& delta_time, const float& map_time);

private:
    std::unique_ptr<VectorUtils> vector_utils_;


    // ��� ã�� �� �׺���̼� ����
    ErrorCode::Input_ErrorType SetupNavigation(NavMesh* nav_mesh, const Enums::CurrentOrderType& order_type,
        Object* hero, Object* target, const std::string& target_hash,
        VMT_For_OBJ* vmt, const Vec3& start_pos, const Vec3& end_pos);

    // �̵� ��� ó��
    ErrorCode::Input_ErrorType ProcessMovement(Object* hero, VMT_For_OBJ* vmt, const float& delta_time);

    // ���� ��� ó��
    ErrorCode::Input_ErrorType ProcessAttack(NavMesh* nav_mesh,Object* hero, VMT_For_OBJ* vmt, const float& delta_time,
        const float& map_time);

    // ���� �̵� ��� ó��
    ErrorCode::Input_ErrorType ProcessAttackMove(NavMesh* nav_mesh, Object* hero, VMT_For_OBJ* vmt,
        const float& delta_time, const float& map_time);

    // ��ü�� ���� ���·� ����
    void SetObjectToIdle(Object* hero);

    // Ÿ���� ��ȿ���� Ȯ��
    bool IsTargetValid(Object* target);
};