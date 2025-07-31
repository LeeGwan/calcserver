#pragma once
#include <unordered_map>

enum class Slot : unsigned int;
enum class AIAction : int;
class Object;
struct GameStateInfo;
class Vec3;
class ActionExecutor {
public:
	ActionExecutor(unsigned int mapid);
	~ActionExecutor();
	bool execute_action(Object* ai_object, Object* target, AIAction action, const GameStateInfo& state, float search_radius);
private:
	unsigned int mapid;
	bool execute_movement(Object* ai_object, AIAction action, float search_radius);
	bool execute_move_to_enemy(Object* ai_object, Object* target);
	bool execute_attack(Object* ai_object, Object* target);
	bool execute_skill(Object* ai_object, Object* target, Slot skill_slot);
	bool execute_blinkskill(Object* ai_object,const Vec3& pos, Slot skill_slot);
	bool execute_retreat(Object* ai_object,const Vec3& SafePos);
	bool execute_stay(Object* ai_object);
	bool execute_follow_navigation(Object* ai_object);
	bool execute_cancel_navigation(Object* ai_object);
	bool send_move_command(Object* ai_object, const Vec3& target_pos);
	bool send_attack_command(Object* ai_object, Object* target);
	bool send_skill_command(Object* ai_object, Object* target, Slot skill_slot, const Vec3& target_pos);
};