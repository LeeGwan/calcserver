#pragma once
#include <mutex>
class Object;
enum class Slot : unsigned int;

class DamageLib
{
public:
    // 메인 데미지 계산 함수들
    static void compute_damage(Object* source, Object* target);
    static void compute_spell_damage(Object* source, Object* target, const Slot& slot, unsigned int skill_level);
    static void compute_physical_damage(Object* source, Object* target);
    static void compute_magical_damage(Object* source, Object* target);
    static void compute_Raw_damage(Object* source, Object* target);

    // 유틸리티 함수들
    static float Get_respawn_time(const int& LV);

private:
    // 공통 헬퍼 함수들 (내부 사용)
 
    static float calculate_damage_reduction(float armor);
    static void apply_damage_to_target(Object* source, Object* target, float damage);
    static void handle_target_death(Object* source,Object* target);
};
extern std::mutex damage_mutex;