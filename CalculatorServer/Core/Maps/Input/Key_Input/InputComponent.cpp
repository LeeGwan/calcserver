#include "InputComponent.h"
#include "../../../Object/Object/ObjectComponent/Object.h"
#include "../../../Navmesh/NavMesh.h"
#include "../../../Error/ErrorClass.h"
#include "MoveAndAttackComponent.h"
#include "../../../Object/Object/Flags.h"
#include"../../../utils/Find_OBJ/Find_OBJ.h"
#include"../../../Object/Object/VMT/VMT.h"
#include <Windows.h>
InputComponent::InputComponent()
    : move_attack_component_(std::make_unique<MoveAndAttackComponent>())
{
}

InputComponent::~InputComponent() = default;


ErrorCode::Input_ErrorType InputComponent::ProcessCommand(NavMesh* nav_mesh, const MoveOrder& task,
    const std::unordered_map<std::string, Object*>& objects,
    const float& map_time)
{
    Object* hero = nullptr;
    Object* target = nullptr;

    // 객체 검증 및 가져오기
    auto result = ValidateAndGetObjects(task, objects, hero, target);
    if (result != ErrorCode::Input_ErrorType::Successed) {
        return result;
    }

    VMT_For_OBJ* vmt = hero->Get_VMT_Component();

    // 죽었거나 사용할 수 없는 객체 체크
    if (vmt->is_dead() || hero->can_use_obj()) {
        return ErrorCode::Input_ErrorType::Error_Is_dead_or_cant_use;
    }

    // 명령 타입 결정
    Enums::CurrentOrderType order_type = DetermineOrderType(task, hero, target, vmt);

    // 명령 실행을 위한 데이터 준비
    MoveOrder processed_task = task;
    processed_task.CurrentOrderType = order_type;

    Vec3 start_pos = hero->Get_Transform_Component().Get_Position();
    Vec3 end_pos = task.EndPos;

    // 이동 및 공격 컴포넌트로 명령 전달
    return move_attack_component_->InitializeOrderPath(nav_mesh, hero, vmt, order_type,
        processed_task, start_pos, end_pos,
        target, map_time);
}

ErrorCode::Input_ErrorType InputComponent::UpdateCommand(NavMesh* nav_mesh, const Enums::CurrentOrderType& order_type,
    Object* hero, VMT_For_OBJ* vmt,
    const std::unordered_map<std::string, Object*>& objects,
    const float& delta_time, const float& map_time)
{


    auto result = move_attack_component_->ProcessOrderRoutine(nav_mesh, hero, vmt, order_type,
        objects, delta_time, map_time);


    return result;
}

ErrorCode::Input_ErrorType InputComponent::ValidateAndGetObjects(const MoveOrder& task,
    const std::unordered_map<std::string, Object*>& objects,
    Object*& hero, Object*& target)
{
    // 명령을 수행할 객체 검증
    if (task.ObjectHash.empty()) {
        return ErrorCode::Input_ErrorType::Error_Cant_Find_OBJ_For_Use_Hash;
    }

    if (!Find_OBJ::Get_OBJ_To_Use_Hash(objects, task.ObjectHash, hero)) {
        return ErrorCode::Input_ErrorType::Error_Cant_Find_OBJ_Dont_Have_OBJ;
    }

    // 타겟 객체 검증 (타겟이 있는 경우만)
    if (!task.TargetObjectHash.empty()) {
        if (!Find_OBJ::Get_OBJ_To_Use_Hash(objects, task.TargetObjectHash, target)) {
            return ErrorCode::Input_ErrorType::Error_Cant_Find_OBJ_Dont_Have_OBJ;
        }
    }

    return ErrorCode::Input_ErrorType::Successed;
}

Enums::CurrentOrderType InputComponent::DetermineOrderType(const MoveOrder& task, Object* hero,
    Object* target, VMT_For_OBJ* vmt)
{
    // 타겟이 없으면 단순 이동
    if (!target || task.TargetObjectHash.empty()) {
        return Enums::CurrentOrderType::MoveTo;
    }

    // 타겟이 적이 아니면 단순 이동
    if (!vmt->is_enemy(target)) {
        return Enums::CurrentOrderType::MoveTo;
    }

    // 적이지만 공격 범위 밖이면 공격 이동
    Vec3 hero_pos = hero->Get_Transform_Component().Get_Position();
    Vec3 target_pos = target->Get_Transform_Component().Get_Position();

    float attack_range = hero->Get_ObjectStats_Component().Get_total_Attack_range();

    if (!vmt->is_in_attack_range(hero_pos, target_pos, attack_range)) {
        return Enums::CurrentOrderType::AttackMove;
    }

    // 공격 범위 내의 적이면 직접 공격
    return Enums::CurrentOrderType::Attack;
}