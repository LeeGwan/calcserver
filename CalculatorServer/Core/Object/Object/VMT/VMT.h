#pragma once

// Forward declarations
class Object;
class Vec3;

/**
 * VMT_For_OBJ - 게임 오브젝트를 위한 유틸리티 클래스
 * 오브젝트의 상태 검사, 타입 확인, 공격 관련 기능을 제공
 */
class VMT_For_OBJ final
{
public:
    explicit VMT_For_OBJ(Object* in_self_obj);
    ~VMT_For_OBJ() = default;

    // 복사 생성자와 대입 연산자 삭제 (안전성 확보)
    VMT_For_OBJ(const VMT_For_OBJ&) = delete;
    VMT_For_OBJ& operator=(const VMT_For_OBJ&) = delete;

    // 상태 확인 메서드
    bool is_alive() const;
    bool is_dead() const;
    bool is_me(Object* other_obj) const;
    bool is_team( Object* other_obj) const;
    bool is_enemy( Object* other_obj) const;
    bool is_immovable() const;
    bool is_moving() const;

    // 오브젝트 타입 확인 메서드
    bool is_turret() const;
    bool is_nexus() const;
    bool is_structure() const;  // turret 또는 nexus
    bool is_hero() const;
    bool is_ai() const;
    bool is_melee_minion() const;
    bool is_ranged_minion() const;
    bool is_minion() const;

    // 공격 관련 메서드
    Vec3 get_attack_position(Object* other_obj) const;
    bool is_in_attack_range(const Vec3& self_pos, const Vec3& target_pos, float attack_range) const;

private:
    Object* self_obj;  // nullptr 초기화 제거 (생성자에서 초기화)
};