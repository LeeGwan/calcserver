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

    // 명령 초기화 - 경로 설정 및 상태 초기화
    ErrorCode::Input_ErrorType InitializeOrderPath(NavMesh* nav_mesh, Object* hero, VMT_For_OBJ* vmt,
        const Enums::CurrentOrderType& order_type,
        const MoveOrder& task, const Vec3& start_pos,
        const Vec3& end_pos, Object* target,
        const float& map_time);

    // 명령 처리 루틴 - 각 프레임마다 호출되어 명령 실행
    ErrorCode::Input_ErrorType ProcessOrderRoutine(NavMesh* nav_mesh, Object* hero, VMT_For_OBJ* vmt,
        const Enums::CurrentOrderType& order_type,
        const std::unordered_map<std::string, Object*>& objects,
        const float& delta_time, const float& map_time);

private:
    std::unique_ptr<VectorUtils> vector_utils_;


    // 경로 찾기 및 네비게이션 설정
    ErrorCode::Input_ErrorType SetupNavigation(NavMesh* nav_mesh, const Enums::CurrentOrderType& order_type,
        Object* hero, Object* target, const std::string& target_hash,
        VMT_For_OBJ* vmt, const Vec3& start_pos, const Vec3& end_pos);

    // 이동 명령 처리
    ErrorCode::Input_ErrorType ProcessMovement(Object* hero, VMT_For_OBJ* vmt, const float& delta_time);

    // 공격 명령 처리
    ErrorCode::Input_ErrorType ProcessAttack(NavMesh* nav_mesh,Object* hero, VMT_For_OBJ* vmt, const float& delta_time,
        const float& map_time);

    // 공격 이동 명령 처리
    ErrorCode::Input_ErrorType ProcessAttackMove(NavMesh* nav_mesh, Object* hero, VMT_For_OBJ* vmt,
        const float& delta_time, const float& map_time);

    // 객체를 유휴 상태로 설정
    void SetObjectToIdle(Object* hero);

    // 타겟이 유효한지 확인
    bool IsTargetValid(Object* target);
};