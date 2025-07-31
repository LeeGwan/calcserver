#include "RLManager.h"
#include "../GameStateAnalyzer/GameStateAnalyzer.h"
#include "../../Object/Object/ObjectComponent/Object.h"
#include "../../Object/Object/VMT/VMT.h"
#include "../RLAgent/RLAgent.h"
#include "../../Maps/Map.h"
#include "../../Navmesh/NavMesh.h"
#include "../../Error/is_valid.h"
#include"../RLStructures/RLStructures.h"
#include "../AIflag/AIflags.h"
#include "../ActionExecutor/ActionExecutor.h"
#include "../../utils/Find_OBJ/Find_OBJ.h"
#include "../../Algorithm/NavigationService.h"
#include "../../Object/Object/Spells/SpellBook/SpellBook.h"
#include <iostream>
#include <fstream>
#include <filesystem>
RLManager::RLManager(Map* map):map(map)
{
	if (!map)return;
	training_mode = false;
	mapid= map->GetMapID();
	state_analyzer = std::make_unique<GameStateAnalyzer>();
	action_executor = std::make_unique<ActionExecutor>(mapid);
}

RLManager::~RLManager() {
	if (!is_shutdown) {
		Release();
	}
}

void RLManager::RLManager_init()
{
	const auto& all_objects = map->GetAllObjects();//MemoryCheck
	for (const auto& [hash, object] : all_objects)
	{
		if (!MemoryCheck::is_valid<Object>(object) || !object->Get_VMT_Component()->is_ai())continue;
	
			register_ai_object(hash);
		
		
	}
}

void RLManager::Release()
{
	if (is_shutdown) return;
	std::cout << "[RL] Starting safe shutdown process..." << std::endl;

	try {
		set_training_mode(false);

	// 모든 에이전트 모델 저장
	std::cout << "[RL] Saving all agent models..." << std::endl;
	for (const auto& [hash, agent] : rl_agents) {
		if (agent) {
			try {
				std::string filename = "./models/agent_" + hash + ".pt";
				agent->save_model(filename);
				agent->clear_replay_buffer();
				std::cout << "[RL] Saved model for agent: " << hash << std::endl;
			}
			catch (const std::exception& e) {
				std::cerr << "[RL] Error saving model for " << hash << ": " << e.what() << std::endl;
			}
		}
	}

	print_statistics();

	// 저장 없이 해제만
	std::cout << "[RL] Unregistering all AI objects..." << std::endl;
	last_states.clear();
	last_action.clear();
	rl_agents.clear();  // 직접 정리 (unregister_ai_object 호출 안함)

	state_analyzer.reset();
	action_executor.reset();
	is_shutdown = true;

	std::cout << "[RL] Safe shutdown completed successfully!" << std::endl;
}
catch (const std::exception& e) {
	std::cerr << "[RL] Error during shutdown: " << e.what() << std::endl;
}
}
void RLManager::register_ai_object(const std::string& object_hash)
{
	std::string path = "./models";
	std::ifstream folder(path);
	if (!folder.good())
	{
		std::filesystem::create_directories(path);
		folder.close();
	}
	if (rl_agents.find(object_hash) == rl_agents.end()) {
		rl_agents.insert({ object_hash,std::make_unique<RLAgent>() });
		
		std::string filename = path + "/agent_" + object_hash + ".pt";
		std::ifstream file(filename);

			
		rl_agents[object_hash]->save_model(filename);
		//std::cout << "[RL] Initial model saved: " << filename << std::endl;
		

		//std::cout << "[RL] AI agent registered: " << object_hash << std::endl;
	}
}

void RLManager::unregister_ai_object(const std::string& object_hash)
{
	auto it = rl_agents.find(object_hash);
	if (it != rl_agents.end()) {
		// 모델 저장 후 제거
		it->second->save_model("./models/agent_" + object_hash + ".pt");
		rl_agents.erase(it);
		last_states.erase(object_hash);
		//std::cout << "[RL] AI agent unregistered: " << object_hash << std::endl;
	}
}

void RLManager::update_ai_agents(float deltatime)
{
	const auto& all_objects = map->GetAllObjects();//MemoryCheck
	for (const auto& [hash, object] : all_objects)
	{
		if (!MemoryCheck::is_valid<Object>(object)||!object->Get_VMT_Component()->is_ai())continue;
		auto agent_it = rl_agents.find(hash);
		if (agent_it == rl_agents.end()) {
			register_ai_object(hash);
			agent_it = rl_agents.find(hash);
		}
		if (agent_it != rl_agents.end()) {
			update_individual_agent(hash, object, agent_it->second.get(), deltatime);
		}
	}
}

void RLManager::save_all_models()
{
	for (const auto& [hash, agent] : rl_agents) {
		std::string filename = "./models/agent_" + hash + ".pt";
		agent->save_model(filename);
	}
	//std::cout << "[RL] All models saved" << std::endl;
}
void RLManager::load_all_models()
{
	for (const auto& [hash, agent] : rl_agents) {
		std::string filename = "./models/agent_" + hash + ".pt";
		agent->load_model(filename);
	}
	std::cout << "[RL] All models loaded" << std::endl;
}
void RLManager::set_training_mode(bool training)
{
	training_mode = training;
	for (const auto& [hash, agent] : rl_agents) {
		agent->set_training_mode(training);
	}
	std::cout << "[RL] Training mode: " << (training ? "ON" : "OFF") << std::endl;
}
void RLManager::print_statistics()
{
	std::cout << "\n[RL] === AI Agent Statistics ===" << std::endl;
	for (const auto& [hash, agent] : rl_agents) {
		std::cout << "[RL] Agent " << hash << ": Episodes=" << agent->get_episodes_done()
			<< ", Epsilon=" << agent->get_epsilon()
			<< ", Best Reward=" << agent->get_best_avg_reward() << std::endl;
	}
	std::cout << "[RL] Total Agents: " << rl_agents.size() << std::endl;
}

void RLManager::update_individual_agent(const std::string& hash, Object* object, RLAgent* agent,float deltatime)
{
	float maptime = map->GetMapTime();
	auto m_allObjects = map->GetAllObjects();
	GameStateInfo current_state = state_analyzer->extract_game_state(map->GetNavMeshComponent(), object, m_allObjects,deltatime, maptime);
	// 이전 상태가 있으면 학습 진행
	auto last_state_it = last_states.find(hash);
	if (last_state_it != last_states.end() && training_mode) {
		// 이전 행동의 결과 평가 및 학습
		// (실제로는 이전 행동을 저장해두어야 하지만 여기서는 단순화)

		bool episode_done = object->Get_VMT_Component()->is_dead();
		float reward = state_analyzer->calculate_reward(object, last_action[hash],
			last_state_it->second, current_state, episode_done);

		// 경험 저장 (임시로 STAY_POSITION 사용)
		agent->store_experience(last_state_it->second, last_action[hash], reward, current_state, episode_done);
		agent->learn();
	}
	/*
	* 내일 할거 
	* 액션상태 체크 하기 
	* execute_action 요거 수정하기
	*/
	// 다음 행동 선택
	AIAction selected_action;
	if (training_mode) {
		selected_action = agent->select_action(hash,current_state);
	}
	else {
		selected_action = agent->select_greedy_action(current_state);
	}
	// 행동 실행
	Object* target = nullptr;
	Vec3 SafePos;
	if (selected_action == AIAction::MOVE_TO_ENEMY || selected_action == AIAction::ATTACK_TARGET || selected_action == AIAction::SKILL_Q || selected_action == AIAction::SKILL_R)
	{
		target=find_enemy(m_allObjects, current_state.enemy_hash, object);
	}
	
	action_executor->execute_action(object, target, selected_action, current_state, current_state.search_radius);

	// 현재 상태 저장
	last_states[hash] = current_state;
	last_action[hash] = selected_action;
}

Object* RLManager::find_enemy(const std::unordered_map<std::string, Object*>& all_objects, const std::string& enemyhash, Object* object)
{
	Object* target;
	if (!Find_OBJ::Get_OBJ_To_Use_Hash(all_objects, enemyhash, target))
	{
		if (!Find_OBJ::Find_Pos_is_near_Enemy(object, target, all_objects))
		{
			return nullptr;
		}
	}
	

	return target;
}
