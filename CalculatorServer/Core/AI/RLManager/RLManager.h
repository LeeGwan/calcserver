#pragma once
#include <memory>
#include <unordered_map>
#include <string>

enum class AIAction : int;
class Map;
class RLAgent;
struct GameStateInfo;
class GameStateAnalyzer;
class ActionExecutor;
class Object;
class NavMesh;
class Vec3;

class RLManager
{
public:
	RLManager(Map* map);
	~RLManager();
	void RLManager_init();
	void Release();
	void register_ai_object(const std::string& object_hash);
	void unregister_ai_object(const std::string& object_hash);
	void update_ai_agents(float deltatime);

	void save_all_models();
	void load_all_models();
	void set_training_mode(bool training);
	void print_statistics();
private:
	Map* map;
	unsigned int mapid;
	bool is_shutdown = false;
	bool training_mode;
	std::unordered_map<std::string, std::unique_ptr<RLAgent>> rl_agents;
	std::unordered_map<std::string, GameStateInfo> last_states;
	std::unordered_map<std::string, AIAction> last_action;
	std::unique_ptr<GameStateAnalyzer> state_analyzer;
	std::unique_ptr<ActionExecutor> action_executor;

	void update_individual_agent(const std::string& hash, Object* object, RLAgent* agent, float deltatime);
	Object* find_enemy(const std::unordered_map<std::string, Object*>& all_objects,const std::string& enemyhash ,Object* object);


};