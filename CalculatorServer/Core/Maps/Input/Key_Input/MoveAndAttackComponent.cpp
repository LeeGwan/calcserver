#include "MoveAndAttackComponent.h"
#include "../../../Object/Object/ObjectComponent/Object.h"
#include "../../../Object/Object/struct_pos/struct_pos.h"
#include "../../../utils/Vector.h"
#include "../../../Object/Object/VMT/VMT.h"
#include "../../../Navmesh/NavMesh.h"
#include "../../../Maps/MapSchedule.h"
#include "../../../Object/Object/objectState/objectState.h"
#include "../../../Object/Object/TargetSelector/TargetSelector.h"
#include "../../../utils/VectorUtils.h"
#include "../../../DamageLib/DamageLib.h"
#include "../../../Error/ErrorClass.h"
#include "../../../Object/Object/Flags.h"
#include "../../../utils/Find_OBJ/Find_OBJ.h"
#include "../../../Algorithm/NavigationService.h"
#include "../../../Object/Object/Navgation/Navigation.h"
#include "../../../Error/is_valid.h"
#include <Windows.h>
#include <cmath> 
MoveAndAttackComponent::MoveAndAttackComponent()
    : vector_utils_(std::make_unique<VectorUtils>())
    
{
}

MoveAndAttackComponent::~MoveAndAttackComponent()= default;


ErrorCode::Input_ErrorType MoveAndAttackComponent::InitializeOrderPath(NavMesh* nav_mesh, Object* hero, VMT_For_OBJ* vmt,
    const Enums::CurrentOrderType& order_type,
    const MoveOrder& task, const Vec3& start_pos,
    const Vec3& end_pos, Object* target,
    const float& map_time)
{
    if (hero->Get_objectState_Component().Get_CurrentOrderType() == Enums::CurrentOrderType::Dead)return ErrorCode::Input_ErrorType::Error_Is_dead_or_cant_use;
    
    TargetSelector* target_selector = hero->Get_TargetSelector_Component();

    switch (order_type) {
    case Enums::CurrentOrderType::MoveTo:
        return SetupNavigation(nav_mesh, order_type, hero, target, task.TargetObjectHash,
            vmt, start_pos, end_pos);

    case Enums::CurrentOrderType::AttackMove:
        return SetupNavigation(nav_mesh, order_type, hero, target, task.TargetObjectHash,
            vmt, start_pos, end_pos);

    case Enums::CurrentOrderType::Attack:
    {
        
        // 공격 상태 설정
        float attack_speed = hero->Get_ObjectStats_Component().Get_total_Attackspeed();
        
        hero->Get_objectState_Component()
            .Set_CurrentOrderType(Enums::CurrentOrderType::Attack);
        hero->Get_objectState_Component().Set_AttackTime((1.0f / attack_speed) + map_time);

            // 타겟 설정
            Vec3 target_pos = target->Get_Transform_Component().Get_Position();
            target_selector->Set_Target_Pos(target_pos)
                .Set_TargetHash(task.TargetObjectHash)
                .Set_Taret_obj(target);
        
        return ErrorCode::Input_ErrorType::Successed;
    }

    case Enums::CurrentOrderType::Stop:
        // 모든 상태 초기화
        target_selector->clear();
        hero->Get_objectState_Component()
            .Set_CurrentOrderType(Enums::CurrentOrderType::Idle);
        hero->Get_objectState_Component().Set_AttackTime(0.0f);
        hero->Get_Navigation_Component()->clear();
        return ErrorCode::Input_ErrorType::Successed;

    default:
        return ErrorCode::Input_ErrorType::UniversalError;
    }
}

ErrorCode::Input_ErrorType MoveAndAttackComponent::SetupNavigation(NavMesh* nav_mesh, const Enums::CurrentOrderType& order_type,
    Object* hero, Object* target, const std::string& target_hash,
    VMT_For_OBJ* vmt, const Vec3& start_pos, const Vec3& end_pos)
{

    // 이동 가능 여부 확인
    PolyFlags movementProps= PolyFlags::SAMPLE_POLYFLAGS_DISABLED;
  
    // 네비게이션 경로 계산
    dtNavMeshQuery* nav_query = nav_mesh->Get_navQuery();
    std::vector<Vec3> navigation_path;
    Vec3 character_size = hero->Get_CharacterMesh_Component().Get_CapsuleSIze();

    if (nav_mesh->Get_NavigationService()->FindPath(nav_query, start_pos, end_pos,
        character_size, navigation_path,
        movementProps)) {
        // 경로 설정 성공
        hero->Get_Navigation_Component()->Set_NaviPath(navigation_path)
            .Set_Current_NaviPath_Index(0);

        hero->Get_objectState_Component().Set_CurrentOrderType(order_type);
        hero->Get_objectState_Component().Set_AttackTime(0.0f);

        // 공격 이동인 경우 타겟 설정
        if (order_type == Enums::CurrentOrderType::AttackMove && target) {
            TargetSelector* target_selector = hero->Get_TargetSelector_Component();
            target_selector->clear();
            target_selector->Set_Taret_obj(target);
            target_selector->Set_Target_Pos(target->Get_Transform_Component().Get_Position())
                .Set_TargetHash(target_hash);
        }

        return ErrorCode::Input_ErrorType::Successed;
    }

    // 경로 찾기 실패
    SetObjectToIdle(hero);
    return ErrorCode::Input_ErrorType::Error_Navi;
}

ErrorCode::Input_ErrorType MoveAndAttackComponent::ProcessOrderRoutine(NavMesh* nav_mesh, Object* hero, VMT_For_OBJ* vmt,
    const Enums::CurrentOrderType& order_type,
    const std::unordered_map<std::string, Object*>& objects,
    const float& delta_time, const float& map_time)
{
    switch (order_type) {
    case Enums::CurrentOrderType::MoveTo:
        return ProcessMovement(hero, vmt, delta_time);

    case Enums::CurrentOrderType::AttackMove:
        return ProcessAttackMove(nav_mesh, hero, vmt, delta_time, map_time);

    case Enums::CurrentOrderType::Attack:
        return ProcessAttack(nav_mesh, hero, vmt, delta_time, map_time);

    case Enums::CurrentOrderType::Idle:
    case Enums::CurrentOrderType::Stop:
        return ErrorCode::Input_ErrorType::Successed;

    default:
        return ErrorCode::Input_ErrorType::UniversalError;
    }
}

ErrorCode::Input_ErrorType MoveAndAttackComponent::ProcessMovement(Object* hero, VMT_For_OBJ* vmt, const float& delta_time)
{


    Navigation* navigation = hero->Get_Navigation_Component();
    std::vector<Vec3> navigation_path = navigation->Get_NaviPath();

    // 경로가 없으면 유휴 상태로 전환
    if (navigation_path.empty()) {
        SetObjectToIdle(hero);
        return ErrorCode::Input_ErrorType::Error_Dont_Have_Navi;
    }

    int current_index = navigation->Get_Current_NaviPath_Index();

    // 경로 끝에 도달했으면 완료 처리
    if (current_index >= static_cast<int>(navigation_path.size())) {
        if (hero->Get_objectState_Component().Get_CurrentOrderType() == Enums::CurrentOrderType::MoveTo) {
            hero->Get_objectState_Component().Set_CurrentOrderType(Enums::CurrentOrderType::Idle);
        }
        navigation->clear();
        return ErrorCode::Input_ErrorType::Successed;
    }

    // 현재 위치와 목표 위치 계산
    Vec3 current_pos = hero->Get_Transform_Component().Get_Position();
    Vec3 target_pos = navigation_path[current_index];
    float distance = target_pos.distance(current_pos);

    // 목표 지점에 도달했으면 다음 지점으로 이동
    if (distance <= 0.1f) {
        navigation->Add_Current_NaviPath_Index(1);
        return ErrorCode::Input_ErrorType::Successed;
    }

    // 이동 처리
    float movement_speed = hero->Get_ObjectStats_Component().Get_total_MovementSpeed();
    Vec3 direction = (target_pos - current_pos).normalized();
    Vec3 move_delta = direction * movement_speed * delta_time;

    // 회전 및 이동 적용
  
    float angle = hero->Get_m_VectorUtils()->GetLerpForAngle(current_pos, target_pos, delta_time);
    hero->Get_Transform_Component().Set_Rotation(Vec3(0.0f, 0.0f, angle));
    hero->Get_Transform_Component().Set_Position(current_pos + move_delta);

    return ErrorCode::Input_ErrorType::Successed;
}

ErrorCode::Input_ErrorType MoveAndAttackComponent::ProcessAttackMove(NavMesh* nav_mesh, Object* hero, VMT_For_OBJ* vmt,
    const float& delta_time, const float& map_time)
{
    TargetSelector* target_selector = hero->Get_TargetSelector_Component();
    Object* target = target_selector->Get_Taret_obj();

    // 타겟 유효성 체크
    if (!IsTargetValid(target)) {
    
        SetObjectToIdle(hero);
        return ErrorCode::Input_ErrorType::Error_Is_dead_or_cant_use;
    }

    Vec3 hero_pos = hero->Get_Transform_Component().Get_Position();
    Vec3 target_pos = target->Get_Transform_Component().Get_Position();
    float attack_range = hero->Get_ObjectStats_Component().Get_total_Attack_range();

    // 공격 범위 내에 있으면 공격 모드로 전환
    if (vmt->is_in_attack_range(hero_pos, target_pos, attack_range)) {
        float attack_speed = hero->Get_ObjectStats_Component().Get_total_Attackspeed();
        hero->Get_objectState_Component()
            .Set_CurrentOrderType(Enums::CurrentOrderType::Attack);
        hero->Get_objectState_Component().Set_AttackTime((1.0f / attack_speed) + map_time);
        hero->Get_Navigation_Component()->clear();
        return ErrorCode::Input_ErrorType::Successed;
    }

    // ⭐ 수정: 네비게이션이 이미 설정되어 있는지 확인
    Navigation* navigation = hero->Get_Navigation_Component();
    std::vector<Vec3> current_path = navigation->Get_NaviPath();

    // 네비게이션 경로가 없거나 타겟이 움직였을 때만 새로운 경로 설정
    bool need_new_navigation = false;

    if (current_path.empty()) {

        need_new_navigation = true;
    }
    else if (vmt->is_moving()) {

        need_new_navigation = true;
    }


    // 새로운 네비게이션이 필요한 경우에만 설정
    if (need_new_navigation) {
     

        // ⭐ 정적 뮤텍스로 네비게이션 보호
        static std::mutex nav_setup_mutex;
        std::lock_guard<std::mutex> lock(nav_setup_mutex);

        // 안전성 검사
        if (!nav_mesh) {
         
            SetObjectToIdle(hero);
            return ErrorCode::Input_ErrorType::Error_Navi;
        }

        if (!nav_mesh->Get_NavigationService()) {
         
            SetObjectToIdle(hero);
            return ErrorCode::Input_ErrorType::Error_Navi;
        }

        dtNavMeshQuery* nav_query = nav_mesh->Get_navQuery();
        if (!nav_query) {
        
            SetObjectToIdle(hero);
            return ErrorCode::Input_ErrorType::Error_Navi;
        }

        Vec3 attack_pos = vmt->get_attack_position(target);
        attack_pos.z = 0.0f;

        // 위치 유효성 검사
        if (std::isnan(attack_pos.x) || std::isnan(attack_pos.y) || std::isnan(attack_pos.z) ||
            std::isinf(attack_pos.x) || std::isinf(attack_pos.y) || std::isinf(attack_pos.z)) {
         
            SetObjectToIdle(hero);
            return ErrorCode::Input_ErrorType::Error_Navi;
        }

        if (std::isnan(hero_pos.x) || std::isnan(hero_pos.y) || std::isnan(hero_pos.z) ||
            std::isinf(hero_pos.x) || std::isinf(hero_pos.y) || std::isinf(hero_pos.z)) {
           
            SetObjectToIdle(hero);
            return ErrorCode::Input_ErrorType::Error_Navi;
        }

 

        // try-catch로 예외 처리
        try {
            auto result = SetupNavigation(nav_mesh, Enums::CurrentOrderType::AttackMove, hero, target,
                target->Get_Hash(), vmt, hero_pos, attack_pos);

            if (result != ErrorCode::Input_ErrorType::Successed) {
                
                return result;
            }
 
        }
        catch (...) {

            SetObjectToIdle(hero);
            return ErrorCode::Input_ErrorType::Error_Navi;
        }
    }

    // 이동 처리
    return ProcessMovement(hero, vmt, delta_time);
}

ErrorCode::Input_ErrorType MoveAndAttackComponent::ProcessAttack(NavMesh* nav_mesh, Object* hero, VMT_For_OBJ* vmt,
    const float& delta_time, const float& map_time)
{
    TargetSelector* target_selector = hero->Get_TargetSelector_Component();
    Object* target = target_selector->Get_Taret_obj();
    if (!MemoryCheck::is_valid(target))return ErrorCode::Input_ErrorType::Error_Cant_Find_OBJ_Dont_Have_OBJ;
    // 타겟이 죽었으면 유휴 상태로 전환
    if (target->Get_VMT_Component()->is_dead()) {
    
        SetObjectToIdle(hero);
        return ErrorCode::Input_ErrorType::Successed;
    }

    Vec3 hero_pos = hero->Get_Transform_Component().Get_Position();
    Vec3 target_pos = target->Get_Transform_Component().Get_Position();
    float attack_range = hero->Get_ObjectStats_Component().Get_total_Attack_range();

    // 타겟 방향으로 회전
    float angle = hero->Get_m_VectorUtils()->GetLerpForAngle(hero_pos, target_pos, delta_time);
    hero->Get_Transform_Component().Set_Rotation(Vec3(0.0f, 0.0f, angle));

    // 공격 범위 밖으로 벗어났으면 공격 이동 모드로 전환 (5% 버퍼)
    if (!vmt->is_in_attack_range(hero_pos, target_pos, attack_range * 1.05f)) {


        target_selector->Set_Target_Pos(target_pos)
            .Set_TargetHash(target->Get_Hash())
            .Set_Taret_obj(target);

        hero->Get_objectState_Component()
            .Set_CurrentOrderType(Enums::CurrentOrderType::AttackMove);
        hero->Get_objectState_Component().Set_AttackTime(0.0f);

        return ErrorCode::Input_ErrorType::Successed;
    }

    // 공격 쿨타임 체크
    float current_attack_time = hero->Get_objectState_Component().Get_AttackTime();
    bool can_attack = (current_attack_time <= map_time);

    // 공격 실행
    if (can_attack) {
        
        DamageLib::compute_damage(hero, target);
        target->Get_CombatStats_Component().Set_last_combat_time(map_time);
        // 다음 공격 시간 설정
        float attack_speed = hero->Get_ObjectStats_Component().Get_total_Attackspeed();
        hero->Get_objectState_Component().Set_AttackTime((1.0f / attack_speed) + map_time);
    }

    return ErrorCode::Input_ErrorType::Successed;
}

void MoveAndAttackComponent::SetObjectToIdle(Object* hero)
{
    hero->Get_objectState_Component()
        .Set_CurrentOrderType(Enums::CurrentOrderType::Idle);
        hero->Get_objectState_Component().Set_AttackTime(0.0f);
    hero->Get_TargetSelector_Component()->clear();
    hero->Get_Navigation_Component()->clear();
}

bool MoveAndAttackComponent::IsTargetValid(Object* target)
{
    return target && !target->Get_VMT_Component()->is_dead();
}