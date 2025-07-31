#pragma once
#include <memory>
#include <unordered_map>
#include <string>
enum class AIAction : int;
class NavigationAnalyzer;
struct GameStateInfo;
class Object;
class NavMesh;
class SpellBook;
class Vec3;
class GameStateAnalyzer {
public:
	GameStateAnalyzer();
	~GameStateAnalyzer();
	GameStateInfo extract_game_state(NavMesh* navmesh,Object* ai_object, const std::unordered_map<std::string, Object*>& all_objects,float deltatime, float maptime);
	float calculate_reward(Object* ai_object, AIAction action, const GameStateInfo& prev_state,
		const GameStateInfo& current_state, bool episode_done);
private:
	std::unique_ptr<NavigationAnalyzer> navigation_analyzer;
	void analyze_navmesh_state(NavMesh* navmesh,GameStateInfo& state, Object* ai_object, float search_radius);

	float calculate_mana_usage_reward(AIAction action, float mp_used, const GameStateInfo& current_state);
	float calculate_skill_usage_reward(SpellBook* spellbook,AIAction action, const GameStateInfo& prev_state,
		const GameStateInfo& current_state);
	float calculate_navigation_reward(AIAction action, const GameStateInfo& prev_state,
		const GameStateInfo& current_state);
	float calculate_safety_reward(const GameStateInfo& prev_state,
		const GameStateInfo& current_state, AIAction action);
	float calculate_combat_reward(Object* ai_object, AIAction action,
		const GameStateInfo& prev_state, const GameStateInfo& current_state);
	float calculate_strategic_reward(const GameStateInfo& prev_state,
		const GameStateInfo& current_state, AIAction action);


	


};