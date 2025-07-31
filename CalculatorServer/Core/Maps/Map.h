#pragma once

#include <optional>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <condition_variable>
#include "Task/Task.h"
// Forward declarations
class RLManager;
class NavMesh;
class ObjectPool;
class AbilityPool;
class InputComponent;
class OutputComponent;
class SpellinputComponent;
class Object;
class SkillAbility;
struct ObjectMoveOrderType;
struct ObjectCastOrderType;
class SkillDetected;
struct MoveOrder;
struct CastSpellINFOR;
class Vec3;

/**
 * @class Map
 * @brief ���� ���� �����ϴ� ���� Ŭ����
 *
 * ���� ���� �����ֱ�, ������Ʈ ����, ��Ƽ������ ó���� ����մϴ�.
 * ���� �����忡�� ���ÿ� ����Ǵ� �پ��� Tick �Լ����� ���� ���� ������ ó���մϴ�.
 */
class Map final
{
public:
    /**
     * @brief Map ������
     * @param mapID ���� ���� ID
     * @param playerNames �÷��̾� �̸� ���
     * @param playerHashes �÷��̾� �ؽ� ���
     */
    Map(int mapID, std::vector<std::string> playerNames, std::vector<std::string> playerHashes);

    /**
     * @brief Map �Ҹ���
     */
    ~Map();
    /**
    * @brief �� API ȣȯ - �Է� ó��
    */
    void Tick_In();

    /**
     * @brief �� API ȣȯ - ��� ó��
     */
    void Tick_Out();

    /**
     * @brief �� API ȣȯ - ��� ó��
     */
    void Tick_In_CALC();

    /**
     * @brief �� API ȣȯ - AI ó��
     */
    void Tick_AI();

    /**
     * @brief �� API ȣȯ - AI ���
     */
    void Tick_AI_CALC();
    // === Getter/Setter �޼��� ===

    /**
     * @brief �� ID ��ȯ
     * @return ���� ���� ID
     */
    int GetMapID() const;

    /**
     * @brief ���� ���� ��ȯ
     * @return ������ ���� ������ ����
     */
    bool IsGameActive() const;

    /**
     * @brief ���� ���� ����
     * @param isActive ���� Ȱ��ȭ ����
     */
    void SetGameState(bool isActive);

    /**
     * @brief �� �ð� ��ȯ (������ ����)
     * @return ���� ���� �� ��� �ð�
     */
    float GetMapTime() const;

    /**
     * @brief ��Ÿ Ÿ�� ��ȯ
     * @return ������ �����Ӱ��� �ð� ����
     */
    float GetDeltaTime() const;

    // === ��Ƽ������ ó�� �޼��� ===

    /**
     * @brief �Է� �̺�Ʈ ó�� ������
     * �÷��̾� �Է°� ���� �̺�Ʈ�� ó���մϴ�.
     */
    void ProcessInputEvents();

    void SendToClientsFromobject();

    void SendToClientsFromSkillObject();

    /**
     * @brief Ŭ���̾�Ʈ ��� ������
     * 40FPS�� Ŭ���̾�Ʈ�� ���� ���¸� �����մϴ�.
     */
    void SendToClients();

    /**
     * @brief ���� ���� ��� ������
     * ������Ʈ ������Ʈ, ��ų ó�� ���� ����մϴ�.
     */
    void CalculateGameLogic();

    /**
     * @brief AI �̺�Ʈ ó�� ������
     * AI ���� �̺�Ʈ�� ó���մϴ�.
     */
    void ProcessAIEvents();
    /**
    * @brief �浹üũ 
    * ����� ������Ʈ���� �浹 üũ ����մϴ�.
    */
    void UpdataCollision();
    /**
     * @brief AI ��� ������
     * AI ���� ����� ����մϴ�.
     */
    void CalculateAI();

    /**
     * @brief �̴Ͼ� �� ������ ó�� ������
     * �̴Ͼ�� �������� ������ ó���մϴ�.
     */
    void ProcessSkill();

    // === ���ҽ� ���� �޼��� ===

    /**
     * @brief ���� ���� ó��
     * ������ �����ϰ� �����ϰ� ���ҽ��� �����մϴ�.
     */
    void ShutdownServer();

    // === ���ø� �޼��� ===

    /**
     * @brief �۾��� ť�� �߰�
     * @tparam T �۾� Ÿ��
     * @param task �߰��� �۾�
     */
    template <typename T>
    void PushTask(const T& task);

    // === ������Ʈ ���� �޼��� ===

    /**
     * @brief NavMesh ������Ʈ ��ȯ
     * @return NavMesh ������ (������ ����)
     */
    NavMesh* GetNavMeshComponent();

    /**
     * @brief AbilityPool ��ȯ
     * @return AbilityPool ������ (������ ����)
     */
    AbilityPool* GetAbilityPool();

    /**
     * @brief ObjectPool ��ȯ
     * @return ObjectPool ������ (������ ����)
     */
    ObjectPool* GetObjectPool();

    /**
     * @brief ���� ���� ��ȯ
     * @return �Է� ó���� ���� ����
     */
    std::condition_variable& GetInputConditionVariable();
    /**
    * @brief �� API ȣȯ - ��ų ó��
    */
    void Tick_SkillCalc();
    void UpdateTile(float deltaTime);
    std::unordered_map<std::string, Object*> GetAllObjects()const;
private:
    // === ��� ���� ===

    // �⺻ ����
    int m_netID;
    int m_mapID;                    ///< �� ID
    bool m_isGameActive;            ///< ���� Ȱ��ȭ ����
    float m_gameTime;               ///< ���� ���� �� ��� �ð�
    float m_AITime;              ///< ai ���� Ÿ��

    // ���ҽ� ������
    std::unique_ptr<ObjectPool> m_objectPool;           ///< ������Ʈ Ǯ
    std::unique_ptr<AbilityPool> m_abilityPool;         ///< �ɷ� Ǯ

    // ������Ʈ
    std::unique_ptr<InputComponent> m_inputComponent;           ///< �Է� ó�� ������Ʈ
    std::unique_ptr<OutputComponent> m_outputComponent;         ///< ��� ó�� ������Ʈ
    std::unique_ptr<SpellinputComponent> m_spellInputComponent; ///< ��ų �Է� ������Ʈ
    std::unique_ptr<NavMesh> m_navMeshComponent;               ///< ������̼� �޽�
    std::unique_ptr<SkillDetected> m_SkillDetected;            ///< ���� �浹

    // ���� ������Ʈ
    std::unordered_map<std::string, Object*> m_allObjects;     ///< ��� ������Ʈ (�ؽ� -> ������Ʈ)
    std::unordered_set<SkillAbility*> m_skillAbilities;       ///< Ȱ�� ��ų �ɷµ�

    // ��Ƽ������ ó��
    std::queue<std::unique_ptr<P_MapTask>> m_taskQueue;        ///< �۾� ť
    std::condition_variable m_inputConditionVariable;          ///< �Է� ó�� ���� ����
    std::mutex m_taskQueueMutex;                              ///< �۾� ť ���ؽ�
    std::mutex m_mapMutex;                                    ///< �� ��ü ���ؽ�
    mutable std::shared_mutex m_timeMutex;                    ///< �ð� ���� ���ؽ� (�б� ����)
    mutable std::shared_mutex m_skillAbilitiesMutex;                    ///< ��ų ���� ���ؽ� (�б� ����)
   // AI 
    std::unique_ptr<RLManager> rl_manager;
    // === ���� �޼��� ===

    /**
     * @brief ���� ���� �� ���ҽ� ����
     */
    void CleanupResources();

    /**
     * @brief ��ų �ɷ� ������Ʈ
     * @param deltaTime ��Ÿ Ÿ��
     */
    void UpdateSkillAbilities(float deltaTime);

    /**
     * @brief �� ��ü ������Ʈ
     */
    void UpdateMap();

    /**
     * @brief ������Ʈ �Է� ������Ʈ
     * @param deltaTime ��Ÿ Ÿ��
     */
    void UpdateObjectInputs(float deltaTime);

    /**
     * @brief �� �ð� �߰� (������ ����)
     * @param deltaTime �߰��� �ð�
     */
    void AddMapTime(float deltaTime);

    /**
     * @brief �ͷ� ����
     * @param team �� (1 �Ǵ� 2)
     */
    void CreateTurrets(int team);

    /**
     * @brief �ؼ��� ����
     * @param team �� (1 �Ǵ� 2)
     */
    void CreateNexus(int team);

    /**
     * @brief �÷��̾� ������Ʈ ����
     * @param playerNames �÷��̾� �̸� ���
     * @param playerHashes �÷��̾� �ؽ� ���
     */
    void CreatePlayerObjects(const std::vector<std::string>& playerNames,
        const std::vector<std::string>& playerHashes);

    /**
     * @brief �ʱ� ������Ʈ���� Ŭ���̾�Ʈ�� ����
     */
    void SendInitialObjectsToClients();

    /**
     * @brief �۾� ó��
     * @param task ó���� �۾�
     */
    void ProcessTask(P_MapTask* task);

    /**
     * @brief ���� ������Ʈ ó��
     * @param object ó���� ������Ʈ
     * @param deltaTime ��Ÿ Ÿ��
     */
    void HandleDeadObject(Object* object, float deltaTime);

    /**
     * @brief Ȱ�� ������Ʈ ó��
     * @param object ó���� ������Ʈ
     * @param deltaTime ��Ÿ Ÿ��
     */
    void ProcessActiveObject(Object* object, float deltaTime);

    /**
     * @brief ������Ʈ ��ȿ�� �˻�
     * @param object �˻��� ������Ʈ
     * @return ��ȿ�� ������Ʈ���� ����
     */
    bool IsObjectValid(Object* object);

    // === ���� API ȣȯ�� ���� ===

 

   

   

    /**
     * @brief �� API ȣȯ - ���� ���� ��ȯ
     */
    bool Get_Gamestate() const;

    /**
     * @brief �� API ȣȯ - �� �ð� ��ȯ
     */
    float Get_Map_time() const;

    /**
    * @brief AI ���� �Լ�
    */
    void UpdateRLSystem(float deltatime);
    void SaveRLModels();
    void SetRLTrainingMode(bool training);
    void initRLSystem();
    void Test_create_ai_object(const std::string& champion_name,
        const std::string& hash_id,
        int team_id,
        const Vec3& position);
};

// === ���ø� ���� ===

template <typename T>
void Map::PushTask(const T& task)
{
    std::unique_lock<std::mutex> lock(m_taskQueueMutex);
    {
        auto typed_task = std::make_unique<MapTask<T>>(task);
        std::unique_ptr<P_MapTask> base_task = std::move(typed_task);
        m_taskQueue.push(std::move(base_task));
        m_inputConditionVariable.notify_one();
    }
}