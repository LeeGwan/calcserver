#include "Map.h"
#include "Spellinput/SpellinputComponent.h"
#include <chrono>
#include <thread>
#include "../Object/Object/Flags.h"
#include "../../RoutineServer/UDP/UdpManager.h"
#include "MapSchedule.h"
#include "../Navmesh/Flags/Flags.h"
#include "../Object/Object/Navgation/Navigation.h"
#include "Timer/Timer.h"
#include "../Ability/SkillAbility/SkillAbility.h"
#include "../Error/is_valid.h"
#include "../utils/Vector.h"
#include "../../RoutineServer/Message/PacketStructure/PacketStructure.h"
#include "../Error/ErrorClass.h"
#include "../Object/ObjectPool.h"
#include "../Ability/AbilityPool/AbilityPool.h"
#include "Input/Key_Input/InputComponent.h"
#include "Output/OutputComponent.h"
#include "../Navmesh/NavMesh.h"
#include "../Algorithm/SkillDetected/SkillDetected.h"
#include"../Object/Object/VMT/VMT.h"
#include "../Object/Object/ObjectComponent/Object.h"
#include "../AI/RLManager/RLManager.h"
// === ��� ���� ===
namespace MapConstants
{
	constexpr int OBJECT_POOL_SIZE = 312;
	constexpr int ABILITY_POOL_SIZE = 500;
	constexpr int TURRET_COUNT_PER_TEAM = 4;
	constexpr int NEXUS_COUNT_PER_TEAM = 1;
	constexpr int CLIENT_SEND_FPS = 60;
	constexpr int CLIENT_SEND_INTERVAL_MS = 1000 / CLIENT_SEND_FPS;
	constexpr int AI_FPS = 30;
	constexpr int AI_INTERVAL_MS = 1000 / AI_FPS;
	// �� 1 (�ϴ�) �ͷ� ��ġ
	const Vec3 TEAM1_TURRET_POSITIONS[4] = {
		Vec3(-450.f, -7600.f, 0.f),
		Vec3(-450.f, -9770.f, 0.f),
		Vec3(-90.f, -11790.f, 0.f),
		Vec3(-850.f, -11790.f, 0.f)
	};

	// �� 2 (���) �ͷ� ��ġ
	const Vec3 TEAM2_TURRET_POSITIONS[4] = {
		Vec3(-450.f, -3330.f, 0.f),
		Vec3(-450.f, -1160.f, 0.f),
		Vec3(-850.f, 860.f, 0.f),
		Vec3(-90.f, 860.f, 0.f)
	};

	// �ؼ��� ��ġ
	const Vec3 TEAM1_NEXUS_POSITION = Vec3(-450.f, -13040.f, 0.f);
	const Vec3 TEAM2_NEXUS_POSITION = Vec3(-450.f, 2090.f, 0.f);

	// �÷��̾� �ʱ� ��ġ
	const Vec3 LOCALPALAYER= Vec3(-370.4572f, -13913.5898f, 0.f);
	const Vec3 TEAM1_START_POSITION = Vec3(-419.981689f, -7283.589844f,0.0f);// Vec3(-370.4572f, -13913.5898f, 0.f);
	const Vec3 TEAM2_START_POSITION = Vec3(-419.981689f, -3603.589844f, 0.0f);// Vec3(-419.981689f, 3140.0f, 0.f);
	const Vec3 HERO_START_ROTATION = Vec3(0.f, 0.f, 0.0f);
	const Vec3 DEFAULT_SCALE = Vec3(1.0f, 1.0f, 1.0f);
	const Vec3 TURRET_SCALE = Vec3(0.3f, 0.3f, 0.3f);
}

// === ������ ===
Map::Map(int mapID, std::vector<std::string> playerNames, std::vector<std::string> playerHashes)
	: m_netID(1)
	, m_mapID(mapID)
	, m_gameTime(0.0f)
	, m_AITime(0.0f)
	, m_isGameActive(true)
{


	// ���ҽ� ������ �ʱ�ȭ

	m_objectPool = std::make_unique<ObjectPool>(MapConstants::OBJECT_POOL_SIZE,
		[]() { return std::make_unique<Object>(); });
	m_abilityPool = std::make_unique<AbilityPool>(MapConstants::ABILITY_POOL_SIZE);

	// ������Ʈ �ʱ�ȭ
	m_inputComponent = std::make_unique<InputComponent>();
	m_spellInputComponent = std::make_unique<SpellinputComponent>();
	m_outputComponent = std::make_unique<OutputComponent>();
	m_navMeshComponent = std::make_unique<NavMesh>();
	m_SkillDetected = std::make_unique<SkillDetected>();
	//rl_manager = std::make_unique<RLManager>(this);
	// ���� ������Ʈ ����
	CreatePlayerObjects(playerNames, playerHashes);

	CreateTurrets(1);  // �� 1 �ͷ�
	CreateNexus(1);    // �� 1 �ؼ���
	CreateTurrets(2);  // �� 2 �ͷ�
	CreateNexus(2);    // �� 2 �ؼ���

	// ��� ������Ʈ�� Ŭ���̾�Ʈ�� ����
	SendInitialObjectsToClients();
	//initRLSystem();
	
}

// === Getter/Setter �޼��� ===
int Map::GetMapID() const
{
	return m_mapID;
}

bool Map::IsGameActive() const
{
	return m_isGameActive;
}

void Map::SetGameState(bool isActive)
{
	m_isGameActive = isActive;
	m_inputConditionVariable.notify_all();
}

float Map::GetMapTime() const
{
	std::shared_lock lock(m_timeMutex);
	return m_gameTime;
}

// === ��Ƽ������ ó�� �޼��� ===
void Map::ProcessInputEvents()
{
	std::unique_ptr<P_MapTask> taskData;

	while (m_isGameActive)
	{
		// �۾� ť���� �۾� ��������
		{
			std::unique_lock<std::mutex> lock(m_taskQueueMutex);
			m_inputConditionVariable.wait(lock, [this] {
				return !m_taskQueue.empty() || !m_isGameActive;
				});

			if (!m_isGameActive) break;

			taskData = std::move(m_taskQueue.front());
			m_taskQueue.pop();
		}

		// �۾� Ÿ�Կ� ���� ó��
		ProcessTask(taskData.get());
	}
}
void Map::SendToClientsFromobject()
{
	for (const auto& [hash, object] : m_allObjects)
	{
		if (IsObjectValid(object))
		{
			m_outputComponent->ProcessObjectUpdates(m_mapID, object);
		}
	}
}
void Map::SendToClientsFromSkillObject()
{
	std::shared_lock lock(m_skillAbilitiesMutex);
	{
		for (SkillAbility* skill : m_skillAbilities)
		{

			if (MemoryCheck::is_valid<SkillAbility>(skill))
			{
				if (skill->Get_Spell_Type() == SpellType::Blink)continue;

				unsigned int skillid = skill->Get_skillid();
				Vec3 startPos = skill->Get_StartPosion();
				m_outputComponent->ProcessSpellUpdates(m_mapID, skillid, startPos);
			}
		}
	}
}
void Map::SendToClients()
{
	auto lastSentTime = std::chrono::steady_clock::now();

	while (m_isGameActive)
	{
		auto currentTime = std::chrono::steady_clock::now();

		// 40FPS�� Ŭ���̾�Ʈ�� ����
		if (currentTime - lastSentTime >= std::chrono::milliseconds(MapConstants::CLIENT_SEND_INTERVAL_MS))
		{
			lastSentTime = currentTime;

			if (!m_isGameActive) return;

			// ��� ����ִ� ������Ʈ ���� ����
			SendToClientsFromobject();
			SendToClientsFromSkillObject();

		}

		// CPU ��뷮 ����ȭ�� ���� ª�� ���
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Map::CalculateGameLogic()
{
	Timer calcTimer;
	float deltaTime = 0.0f;

	while (m_isGameActive)
	{
		deltaTime = calcTimer.GetDeltaTime();

		// �ð� ������Ʈ
		AddMapTime(deltaTime);

		// ���� ���� ������Ʈ
		UpdateMap();
		UpdateObjectInputs(deltaTime);


		calcTimer.Update();
	}
}

void Map::ProcessAIEvents()
{
	auto lastSentTime = std::chrono::steady_clock::now();
	Timer calcTimer;
	float deltaTime = 0.0f;
	float maptime=0.0f;

	SetRLTrainingMode(true);

	while (m_isGameActive)
	{
		deltaTime = calcTimer.GetDeltaTime();
		
			// AI �̺�Ʈ ó�� ����
			
		UpdateRLSystem(deltaTime);
		
		
		
		calcTimer.Update();
		maptime += deltaTime;
		if (maptime - m_AITime>600.0f)
		{
			SaveRLModels();
			m_AITime = maptime;
		}
	
	}
	SaveRLModels();
}

void Map::UpdataCollision()
{

}

void Map::CalculateAI()
{
	while (m_isGameActive)
	{
		// AI ��� ����
		// TODO: AI ��� ����

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

void Map::ProcessSkill()
{
	Timer calcTimer;

	float deltaTime = 0.0f;
	while (m_isGameActive)
	{
		deltaTime = calcTimer.GetDeltaTime();
		UpdateSkillAbilities(deltaTime);
		UpdateTile(deltaTime);
		calcTimer.Update();


	}
}

// === ���ҽ� ���� �޼��� ===
void Map::ShutdownServer()
{
	CleanupResources();
}

void Map::CleanupResources()
{
	// ���ҽ� ����
	if (m_objectPool)
	{
		m_objectPool->clear();
		m_objectPool.reset();
	}

	if (m_abilityPool)
	{
		m_abilityPool->Release();
		m_abilityPool.reset();
	}
	if (m_inputComponent)
		m_inputComponent.reset();
	
	if (m_outputComponent)
		m_outputComponent.reset();
	if (m_SkillDetected)
	{
		m_SkillDetected->RemoveAllSkillEffects(GetNavMeshComponent()->Get_LeagueNavMesh());
		m_SkillDetected.reset();
	}
	if (m_navMeshComponent)
	{
		m_navMeshComponent->Release();
		m_navMeshComponent.reset();
	}
	if (rl_manager)
	{
		rl_manager->Release();
		rl_manager.reset();
	}

}

// === ������Ʈ ���� �޼��� ===
NavMesh* Map::GetNavMeshComponent()
{
	std::unique_lock lock(m_mapMutex);
	return m_navMeshComponent.get();
}

AbilityPool* Map::GetAbilityPool()
{
	std::unique_lock lock(m_mapMutex);
	return m_abilityPool.get();
}

ObjectPool* Map::GetObjectPool()
{
	std::unique_lock lock(m_mapMutex);
	return m_objectPool.get();
}

std::condition_variable& Map::GetInputConditionVariable()
{
	return m_inputConditionVariable;
}
void Map::UpdateTile(float deltaTime)
{
	m_SkillDetected->Update(GetNavMeshComponent()->Get_LeagueNavMesh(), GetNavMeshComponent()->Get_navQuery(), deltaTime);
}
std::unordered_map<std::string, Object*> Map::GetAllObjects()const
{
	return m_allObjects;
}
// === ���� �޼��� ===
void Map::UpdateSkillAbilities(float deltaTime)
{
	for (auto it = m_skillAbilities.begin(); it != m_skillAbilities.end();)
	{


		if (!MemoryCheck::is_valid<SkillAbility>(*it))
		{
			std::unique_lock lock(m_skillAbilitiesMutex);
			{
				it = m_skillAbilities.erase(it);
			}
		}
		else
		{
			// ��ų ������Ʈ ó��
			SkillAbility* skill = *it;
			SkillDetected* p__SkillDetected = m_SkillDetected.get();
			ErrorCode::Spell_ErrorType result = m_spellInputComponent->UpdateSpellCasting(
				m_allObjects,
				m_outputComponent.get(),
				m_mapID,
				GetNavMeshComponent(),
				p__SkillDetected,
				skill,
				deltaTime,Get_Map_time());

			++it;
		}
	}
}

void Map::UpdateMap()
{
	if (!m_isGameActive) return;

	// �� ��ü ������Ʈ ����
	// TODO: �� Ư�� ������Ʈ ���� ����
}

void Map::UpdateObjectInputs(float deltaTime)
{
	for (const auto& [hash, object] : m_allObjects)
	{
		if (!MemoryCheck::is_valid<Object>(object)) continue;
		//��Ÿ�� ó��
		m_spellInputComponent->UpdateCooldown(object, deltaTime);
		// ���� ������Ʈ ó��
		if (object->Get_VMT_Component()->is_dead())
		{
			if (object->Get_VMT_Component()->is_hero())
			{
				HandleDeadObject(object, deltaTime);
			}
			continue;
		}
		else
		{
			if (object->Get_VMT_Component()->is_hero())
			{
				if (object->Get_objectState_Component().Get_DeadTime() > 0.1f)
				{
					OutputDebugStringA((object->Get_Hash() + "currenttype�� dead������ �����߻�").c_str());
				}
			}

		}
	
		// Ȱ�� ������Ʈ ó��
		ProcessActiveObject(object, deltaTime);
		
	}
}

void Map::AddMapTime(float deltaTime)
{
	std::unique_lock lock(m_timeMutex);
	m_gameTime += deltaTime;
}

void Map::CreatePlayerObjects(const std::vector<std::string>& playerNames,
	const std::vector<std::string>& playerHashes)
{

	for (size_t i = 0; i < playerNames.size(); ++i)
	{
		Object* playerObject = m_objectPool->acquire_object();
		if (!playerObject) continue;

		const std::string& playerHash = playerHashes[i];
		if (playerHash.rfind("BOT_", 0) == 0)
		{

			playerObject->init_Object(playerNames[i], playerHash, 2, static_cast<unsigned long>(Enums::ObjectTypeFlag::Hero | Enums::ObjectTypeFlag::AI));
			playerObject->Get_Transform_Component().Set_Position(MapConstants::TEAM2_START_POSITION);
		}
		else
		{
			playerObject->init_Object(playerNames[i], playerHash, 1, static_cast<unsigned long>(Enums::ObjectTypeFlag::Hero));
			playerObject->Get_Transform_Component().Set_Position(MapConstants::LOCALPALAYER); 
		}
		// �÷��̾� ������Ʈ �ʱ�ȭ
		playerObject->Get_Transform_Component().Set_Rotation(MapConstants::HERO_START_ROTATION);
		playerObject->Get_Transform_Component().Set_Scale(MapConstants::DEFAULT_SCALE);


		m_allObjects.insert({ playerHash, playerObject });
	}
	/*
	std::string aiHash= std::to_string (m_mapID) + "_AITEST";
	int teamid;
	Vec3 spwanpos;
	for (int i = 0; i < 4; i++)
	{
		aiHash += std::to_string(i);
		if (i == 0 || i == 1)
		{
			teamid = 1;
			spwanpos = MapConstants::TEAM1_START_POSITION;

		}
		else
		{
			teamid = 2;
			spwanpos = MapConstants::TEAM2_START_POSITION;
		}
		Test_create_ai_object("ezreal", aiHash, teamid, spwanpos);
	}*/
		
		

}

void Map::CreateTurrets(int team)
{
	const Vec3* positions = (team == 1) ? MapConstants::TEAM1_TURRET_POSITIONS
		: MapConstants::TEAM2_TURRET_POSITIONS;
	const Vec3 rotation = (team == 1) ? Vec3(0.f, 0.f, 0.f) : Vec3(0.f, 0.f, -180.f);

	for (int i = 0; i < MapConstants::TURRET_COUNT_PER_TEAM; ++i, ++m_netID)
	{
		Object* turret = m_objectPool->acquire_object();
		if (!turret) continue;

		std::string turretHash = "TURRET_" + std::to_string(m_netID) + "_" + std::to_string(m_netID);
		
		// �ͷ� �ʱ�ȭ
		turret->init_Object("turret", turretHash, team, static_cast<unsigned long>(Enums::ObjectTypeFlag::Turret));
		turret->Get_Transform_Component().Set_Position(positions[i]);
		turret->Get_Transform_Component().Set_Rotation(rotation);
		turret->Get_Transform_Component().Set_Scale(MapConstants::TURRET_SCALE);

		m_allObjects.insert({ turretHash, turret });
	}
}

void Map::CreateNexus(int team)
{
	++m_netID;
	Object* nexus = m_objectPool->acquire_object();
	if (!nexus) return;

	std::string nexusHash = "NEXUS" + std::to_string(m_netID) + "_" + std::to_string(m_netID);
	Vec3 position = (team == 1) ? MapConstants::TEAM1_NEXUS_POSITION
		: MapConstants::TEAM2_NEXUS_POSITION;

	// �ؼ��� �ʱ�ȭ
	nexus->init_Object("nexus", nexusHash, team, static_cast<unsigned long>(Enums::ObjectTypeFlag::Nexus));
	nexus->Get_Transform_Component().Set_Position(position);
	nexus->Get_Transform_Component().Set_Rotation(Vec3(0.f, 0.f, 0.f));
	nexus->Get_Transform_Component().Set_Scale(MapConstants::DEFAULT_SCALE);

	m_allObjects.insert({ nexusHash, nexus });
}

void Map::SendInitialObjectsToClients()
{
	// ��� ������Ʈ�� Ŭ���̾�Ʈ�� ����
	for (const auto& [hash, object] : m_allObjects)
	{
		if (IsObjectValid(object))
		{
			m_outputComponent->SendInitData(m_mapID, object, PacketType::CreateMap);
		}
	}
}

void Map::ProcessTask(P_MapTask* task)
{
	// �̵� ��� ó��
	if (MapTask<ObjectMoveOrderType>* moveTask = dynamic_cast<MapTask<ObjectMoveOrderType>*>(task))
	{
		MoveOrder moveOrder = moveTask->Task.Move_INFOR;
		ErrorCode::Input_ErrorType result = m_inputComponent->ProcessCommand(GetNavMeshComponent(), moveOrder, m_allObjects, GetMapTime());
		return;
	}

	// ��ų ���� ��� ó��

	if (MapTask<ObjectCastOrderType>* castTask = dynamic_cast<MapTask<ObjectCastOrderType>*>(task))
	{
		CastSpellINFOR castOrder = castTask->Task.CastSpell_INFOR;
		SkillAbility* outSkillAbility = nullptr;
		SkillDetected* p__SkillDetected = m_SkillDetected.get();
		ErrorCode::Spell_ErrorType result = m_spellInputComponent->InitializeSpellCasting(
			GetNavMeshComponent(), p__SkillDetected, m_abilityPool.get(), outSkillAbility, castOrder, m_allObjects, GetMapTime());

		if (result == ErrorCode::Spell_ErrorType::Spell_Successed)
		{
			std::unique_lock lock(m_skillAbilitiesMutex);
			{
				m_skillAbilities.insert(outSkillAbility);
			}
			m_outputComponent->ProcessSpellInit(m_mapID, outSkillAbility);//�����̺�Ʈ
		}
	}
	return;

}

void Map::HandleDeadObject(Object* object, float deltaTime)
{
	float currentDeadTime = object->Get_objectState_Component().Get_DeadTime() - deltaTime;

	if (currentDeadTime <= 0.0f)
	{
		// ��Ȱ ó��
		//std::string name = object->Get_Hash() + "��Ȱ!!!\n";
		//OutputDebugStringA(name.c_str());
		if (object->Get_VMT_Component()->is_ai())
		{
			object->Get_Transform_Component().Set_Position(MapConstants::TEAM2_START_POSITION);
		}
		else
		{
			object->Get_Transform_Component().Set_Position(MapConstants::TEAM1_START_POSITION);
		}
		float health = object->Get_ObjectStats_Component().Get_maxhelth();
		float mana = object->Get_ObjectStats_Component().Get_maxmana();
		object->Get_ObjectStats_Component().Set_health(health);
		object->Get_ObjectStats_Component().Set_mana(mana);
		object->Get_Transform_Component().Set_Rotation(MapConstants::HERO_START_ROTATION);
		object->Get_objectState_Component().Set_DeadTime(0.0f);
		object->Get_objectState_Component().Set_CurrentOrderType(Enums::CurrentOrderType::Idle);
		
	}
	else
	{
		object->Get_objectState_Component().Set_DeadTime(currentDeadTime);
	}
}

void Map::ProcessActiveObject(Object* object, float deltaTime)
{
	Enums::CurrentOrderType currentOrderType =
		static_cast<Enums::CurrentOrderType>(object->Get_objectState_Component().Get_CurrentOrderType());

	if (currentOrderType == Enums::CurrentOrderType::Idle) return;

	// ������Ʈ �Է� ó��
	ErrorCode::Input_ErrorType result = m_inputComponent->UpdateCommand(
		GetNavMeshComponent(), currentOrderType, object, object->Get_VMT_Component(),
		m_allObjects, deltaTime, GetMapTime());
}

bool Map::IsObjectValid(Object* object)
{
	return MemoryCheck::is_valid<Object>(object) && !object->Get_VMT_Component()->is_dead();
}



Map::~Map()
{
	
}


void Map::Tick_In()
{
	ProcessInputEvents();
}

void Map::Tick_Out()
{
	SendToClients();
}

void Map::Tick_In_CALC()
{
	CalculateGameLogic();
}

void Map::Tick_AI()
{
	ProcessAIEvents();
}

void Map::Tick_AI_CALC()
{
	CalculateAI();
}

void Map::Tick_SkillCalc()
{
	ProcessSkill();
}

bool Map::Get_Gamestate() const
{
	return IsGameActive();
}

float Map::Get_Map_time() const
{
	return GetMapTime();
}

void Map::UpdateRLSystem(float deltatime)
{
	if (rl_manager) {
		rl_manager->update_ai_agents(deltatime);
	}
}

void Map::SaveRLModels()
{
	if (rl_manager) {
		rl_manager->save_all_models();
	}
}

void Map::SetRLTrainingMode(bool training)
{
	if (rl_manager) {
		rl_manager->set_training_mode(training);
	}
}
void Map::initRLSystem()
{
	if (rl_manager)
	{
		rl_manager->RLManager_init();
	}
}

void Map::Test_create_ai_object(const std::string& champion_name, const std::string& hash_id, int team_id, const Vec3& position)
{
	Object* aiObject = m_objectPool->acquire_object();

	// AI �÷��� ����
	unsigned long flags = static_cast<unsigned long>(Enums::ObjectTypeFlag::Hero | Enums::ObjectTypeFlag::AI);
	aiObject->init_Object(champion_name, hash_id, team_id, flags);

	// ��ġ ���� (�⺻���� ������ �װ� ���, �ƴϸ� ���� ���� ��ġ)
	Vec3 spawn_position = position;
	if (!position.is_valid()) {
		spawn_position = (team_id == 1) ? MapConstants::TEAM1_START_POSITION : MapConstants::TEAM2_START_POSITION;
	}

	// Transform ����
	aiObject->Get_Transform_Component().Set_Position(spawn_position);
	aiObject->Get_Transform_Component().Set_Rotation(MapConstants::HERO_START_ROTATION);
	aiObject->Get_Transform_Component().Set_Scale(MapConstants::DEFAULT_SCALE);

	// ������Ʈ �ʿ� �߰�
	m_allObjects.insert({ hash_id, aiObject });

}
