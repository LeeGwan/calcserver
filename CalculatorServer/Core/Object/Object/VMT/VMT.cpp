#include "VMT.h"
#include "../../../../Core/Navmesh/Flags/Flags.h"
#include "../ObjectComponent/Object.h"
#include "../../../utils/Vector.h"
#include "../../../../Core/Maps/MapSchedule.h"
#include "../../../../Core/Navmesh/NavMesh.h"
#include "../struct_pos/struct_pos.h"
#include "../objectState/objectState.h"
#include "../Transform/Transform.h"
#include"../TargetSelector/TargetSelector.h"

VMT_For_OBJ::VMT_For_OBJ(Object* in_self_obj) : self_obj(in_self_obj)
{
    // 생성자에서 nullptr 체크는 호출하는 쪽에서 책임지도록 함
}

// 상태 확인 메서드들
bool VMT_For_OBJ::is_alive() const
{
    return self_obj->Get_objectState_Component().Get_CurrentOrderType() != Enums::CurrentOrderType::Dead;
}

bool VMT_For_OBJ::is_dead() const
{
    return self_obj->Get_objectState_Component().Get_CurrentOrderType() == Enums::CurrentOrderType::Dead;
}

bool VMT_For_OBJ::is_me(Object* other_obj) const
{
    return self_obj->Get_Hash() == other_obj->Get_Hash();
}

bool VMT_For_OBJ::is_team(Object* other_obj) const
{
    return self_obj->Get_objectState_Component().Get_Team() ==
        other_obj->Get_objectState_Component().Get_Team();
}

bool VMT_For_OBJ::is_enemy(Object* other_obj) const
{
    return !is_me(other_obj) && self_obj->Get_objectState_Component().Get_Team() !=
        other_obj->Get_objectState_Component().Get_Team();
}

bool VMT_For_OBJ::is_immovable() const
{
    const unsigned long obj_state = self_obj->Get_objectState_Component().Get_GameObjectActionState();
    return obj_state & static_cast<unsigned long>(Enums::GameObjectActionState::Immovable);
}

bool VMT_For_OBJ::is_moving() const
{
    TargetSelector* target_selector = self_obj->Get_TargetSelector_Component();
    Object* target_obj = target_selector->Get_Taret_obj();

    if (!target_obj) {
        return false;
    }

    const Vec3 target_obj_pos = target_obj->Get_Transform_Component().Get_Position();
    const Vec3 stored_target_pos = target_selector->Get_Target_Pos();

    float position_diff = target_obj_pos.distance(stored_target_pos);

    // ⭐ 수정: 이동 임계값을 더 크게 설정 (너무 민감하게 반응하지 않도록)
    if (position_diff < 5.0f) // 기존 0.1f에서 5.0f로 증가
    {
        return false;
    }

    // 타겟 위치 업데이트
    target_selector->Set_Target_Pos(target_obj_pos);
    return true;
}

// 오브젝트 타입 확인 메서드들 (헬퍼 메서드로 중복 제거)
bool VMT_For_OBJ::is_turret() const
{
    const unsigned long obj_state = self_obj->Get_objectState_Component().Get_ObjectTypeFlag();
    return obj_state & static_cast<unsigned long>(Enums::ObjectTypeFlag::Turret);
}

bool VMT_For_OBJ::is_nexus() const
{
    const unsigned long obj_state = self_obj->Get_objectState_Component().Get_ObjectTypeFlag();
    return obj_state & static_cast<unsigned long>(Enums::ObjectTypeFlag::Nexus);
}

bool VMT_For_OBJ::is_structure() const
{
    return is_turret() || is_nexus();
}

bool VMT_For_OBJ::is_hero() const
{

    const unsigned long obj_state = self_obj->Get_objectState_Component().Get_ObjectTypeFlag();
    return (obj_state & static_cast<unsigned long>(Enums::ObjectTypeFlag::Hero))!=0;
}

bool VMT_For_OBJ::is_ai() const
{
    const unsigned long obj_state = self_obj->Get_objectState_Component().Get_ObjectTypeFlag();
    return (obj_state & static_cast<unsigned long>(Enums::ObjectTypeFlag::AI)) != 0;

}

bool VMT_For_OBJ::is_melee_minion() const
{
    const unsigned long obj_state = self_obj->Get_objectState_Component().Get_ObjectTypeFlag();
    return obj_state & static_cast<unsigned long>(Enums::ObjectTypeFlag::Melee_Minion);
}

bool VMT_For_OBJ::is_ranged_minion() const
{
    const unsigned long obj_state = self_obj->Get_objectState_Component().Get_ObjectTypeFlag();
    return obj_state & static_cast<unsigned long>(Enums::ObjectTypeFlag::Ranged_Minion);
}

bool VMT_For_OBJ::is_minion() const
{
    const unsigned long obj_state = self_obj->Get_objectState_Component().Get_ObjectTypeFlag();
    return obj_state & static_cast<unsigned long>(Enums::ObjectTypeFlag::Minion);
}

// 공격 관련 메서드들
Vec3 VMT_For_OBJ::get_attack_position(Object* other_obj) const
{
   Vec3 this_pos = self_obj->Get_Transform_Component().Get_Position();
   Vec3 target_pos = other_obj->Get_Transform_Component().Get_Position();
    const float distance = this_pos.distance(target_pos);
    const float attack_range = self_obj->Get_ObjectStats_Component().Get_total_Attack_range();
    const float target_size = other_obj->Get_CharacterMesh_Component().Get_CapsuleSIze().x;

    // 공격 가능한 위치 계산
    return this_pos.append(this_pos, target_pos, (distance + target_size) - attack_range);
}

bool VMT_For_OBJ::is_in_attack_range(const Vec3& self_pos, const Vec3& target_pos, float attack_range) const
{
    const float distance = target_pos.distance(self_pos);
    return distance < attack_range;
}