#pragma once

// Forward declarations
class Object;
class Vec3;

/**
 * VMT_For_OBJ - ���� ������Ʈ�� ���� ��ƿ��Ƽ Ŭ����
 * ������Ʈ�� ���� �˻�, Ÿ�� Ȯ��, ���� ���� ����� ����
 */
class VMT_For_OBJ final
{
public:
    explicit VMT_For_OBJ(Object* in_self_obj);
    ~VMT_For_OBJ() = default;

    // ���� �����ڿ� ���� ������ ���� (������ Ȯ��)
    VMT_For_OBJ(const VMT_For_OBJ&) = delete;
    VMT_For_OBJ& operator=(const VMT_For_OBJ&) = delete;

    // ���� Ȯ�� �޼���
    bool is_alive() const;
    bool is_dead() const;
    bool is_me(Object* other_obj) const;
    bool is_team( Object* other_obj) const;
    bool is_enemy( Object* other_obj) const;
    bool is_immovable() const;
    bool is_moving() const;

    // ������Ʈ Ÿ�� Ȯ�� �޼���
    bool is_turret() const;
    bool is_nexus() const;
    bool is_structure() const;  // turret �Ǵ� nexus
    bool is_hero() const;
    bool is_ai() const;
    bool is_melee_minion() const;
    bool is_ranged_minion() const;
    bool is_minion() const;

    // ���� ���� �޼���
    Vec3 get_attack_position(Object* other_obj) const;
    bool is_in_attack_range(const Vec3& self_pos, const Vec3& target_pos, float attack_range) const;

private:
    Object* self_obj;  // nullptr �ʱ�ȭ ���� (�����ڿ��� �ʱ�ȭ)
};