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
 * @brief 게임 맵을 관리하는 메인 클래스
 *
 * 게임 맵의 생명주기, 오브젝트 관리, 멀티스레드 처리를 담당합니다.
 * 여러 스레드에서 동시에 실행되는 다양한 Tick 함수들을 통해 게임 로직을 처리합니다.
 */
class Map final
{
public:
    /**
     * @brief Map 생성자
     * @param mapID 맵의 고유 ID
     * @param playerNames 플레이어 이름 목록
     * @param playerHashes 플레이어 해시 목록
     */
    Map(int mapID, std::vector<std::string> playerNames, std::vector<std::string> playerHashes);

    /**
     * @brief Map 소멸자
     */
    ~Map();
    /**
    * @brief 구 API 호환 - 입력 처리
    */
    void Tick_In();

    /**
     * @brief 구 API 호환 - 출력 처리
     */
    void Tick_Out();

    /**
     * @brief 구 API 호환 - 계산 처리
     */
    void Tick_In_CALC();

    /**
     * @brief 구 API 호환 - AI 처리
     */
    void Tick_AI();

    /**
     * @brief 구 API 호환 - AI 계산
     */
    void Tick_AI_CALC();
    // === Getter/Setter 메서드 ===

    /**
     * @brief 맵 ID 반환
     * @return 맵의 고유 ID
     */
    int GetMapID() const;

    /**
     * @brief 게임 상태 반환
     * @return 게임이 실행 중인지 여부
     */
    bool IsGameActive() const;

    /**
     * @brief 게임 상태 설정
     * @param isActive 게임 활성화 여부
     */
    void SetGameState(bool isActive);

    /**
     * @brief 맵 시간 반환 (스레드 안전)
     * @return 게임 시작 후 경과 시간
     */
    float GetMapTime() const;

    /**
     * @brief 델타 타임 반환
     * @return 마지막 프레임과의 시간 차이
     */
    float GetDeltaTime() const;

    // === 멀티스레드 처리 메서드 ===

    /**
     * @brief 입력 이벤트 처리 스레드
     * 플레이어 입력과 게임 이벤트를 처리합니다.
     */
    void ProcessInputEvents();

    void SendToClientsFromobject();

    void SendToClientsFromSkillObject();

    /**
     * @brief 클라이언트 출력 스레드
     * 40FPS로 클라이언트에 게임 상태를 전송합니다.
     */
    void SendToClients();

    /**
     * @brief 게임 로직 계산 스레드
     * 오브젝트 업데이트, 스킬 처리 등을 담당합니다.
     */
    void CalculateGameLogic();

    /**
     * @brief AI 이벤트 처리 스레드
     * AI 관련 이벤트를 처리합니다.
     */
    void ProcessAIEvents();
    /**
    * @brief 충돌체크 
    * 스펠과 오브젝트간의 충돌 체크 담당합니다.
    */
    void UpdataCollision();
    /**
     * @brief AI 계산 스레드
     * AI 로직 계산을 담당합니다.
     */
    void CalculateAI();

    /**
     * @brief 미니언 및 구조물 처리 스레드
     * 미니언과 구조물의 로직을 처리합니다.
     */
    void ProcessSkill();

    // === 리소스 관리 메서드 ===

    /**
     * @brief 서버 종료 처리
     * 게임을 안전하게 종료하고 리소스를 정리합니다.
     */
    void ShutdownServer();

    // === 템플릿 메서드 ===

    /**
     * @brief 작업을 큐에 추가
     * @tparam T 작업 타입
     * @param task 추가할 작업
     */
    template <typename T>
    void PushTask(const T& task);

    // === 컴포넌트 접근 메서드 ===

    /**
     * @brief NavMesh 컴포넌트 반환
     * @return NavMesh 포인터 (스레드 안전)
     */
    NavMesh* GetNavMeshComponent();

    /**
     * @brief AbilityPool 반환
     * @return AbilityPool 포인터 (스레드 안전)
     */
    AbilityPool* GetAbilityPool();

    /**
     * @brief ObjectPool 반환
     * @return ObjectPool 포인터 (스레드 안전)
     */
    ObjectPool* GetObjectPool();

    /**
     * @brief 조건 변수 반환
     * @return 입력 처리용 조건 변수
     */
    std::condition_variable& GetInputConditionVariable();
    /**
    * @brief 구 API 호환 - 스킬 처리
    */
    void Tick_SkillCalc();
    void UpdateTile(float deltaTime);
    std::unordered_map<std::string, Object*> GetAllObjects()const;
private:
    // === 멤버 변수 ===

    // 기본 정보
    int m_netID;
    int m_mapID;                    ///< 맵 ID
    bool m_isGameActive;            ///< 게임 활성화 상태
    float m_gameTime;               ///< 게임 시작 후 경과 시간
    float m_AITime;              ///< ai 저장 타임

    // 리소스 관리자
    std::unique_ptr<ObjectPool> m_objectPool;           ///< 오브젝트 풀
    std::unique_ptr<AbilityPool> m_abilityPool;         ///< 능력 풀

    // 컴포넌트
    std::unique_ptr<InputComponent> m_inputComponent;           ///< 입력 처리 컴포넌트
    std::unique_ptr<OutputComponent> m_outputComponent;         ///< 출력 처리 컴포넌트
    std::unique_ptr<SpellinputComponent> m_spellInputComponent; ///< 스킬 입력 컴포넌트
    std::unique_ptr<NavMesh> m_navMeshComponent;               ///< 내비게이션 메시
    std::unique_ptr<SkillDetected> m_SkillDetected;            ///< 스펠 충돌

    // 게임 오브젝트
    std::unordered_map<std::string, Object*> m_allObjects;     ///< 모든 오브젝트 (해시 -> 오브젝트)
    std::unordered_set<SkillAbility*> m_skillAbilities;       ///< 활성 스킬 능력들

    // 멀티스레드 처리
    std::queue<std::unique_ptr<P_MapTask>> m_taskQueue;        ///< 작업 큐
    std::condition_variable m_inputConditionVariable;          ///< 입력 처리 조건 변수
    std::mutex m_taskQueueMutex;                              ///< 작업 큐 뮤텍스
    std::mutex m_mapMutex;                                    ///< 맵 전체 뮤텍스
    mutable std::shared_mutex m_timeMutex;                    ///< 시간 관련 뮤텍스 (읽기 공유)
    mutable std::shared_mutex m_skillAbilitiesMutex;                    ///< 스킬 관련 뮤텍스 (읽기 쓰기)
   // AI 
    std::unique_ptr<RLManager> rl_manager;
    // === 헬퍼 메서드 ===

    /**
     * @brief 게임 종료 시 리소스 정리
     */
    void CleanupResources();

    /**
     * @brief 스킬 능력 업데이트
     * @param deltaTime 델타 타임
     */
    void UpdateSkillAbilities(float deltaTime);

    /**
     * @brief 맵 전체 업데이트
     */
    void UpdateMap();

    /**
     * @brief 오브젝트 입력 업데이트
     * @param deltaTime 델타 타임
     */
    void UpdateObjectInputs(float deltaTime);

    /**
     * @brief 맵 시간 추가 (스레드 안전)
     * @param deltaTime 추가할 시간
     */
    void AddMapTime(float deltaTime);

    /**
     * @brief 터렛 생성
     * @param team 팀 (1 또는 2)
     */
    void CreateTurrets(int team);

    /**
     * @brief 넥서스 생성
     * @param team 팀 (1 또는 2)
     */
    void CreateNexus(int team);

    /**
     * @brief 플레이어 오브젝트 생성
     * @param playerNames 플레이어 이름 목록
     * @param playerHashes 플레이어 해시 목록
     */
    void CreatePlayerObjects(const std::vector<std::string>& playerNames,
        const std::vector<std::string>& playerHashes);

    /**
     * @brief 초기 오브젝트들을 클라이언트에 전송
     */
    void SendInitialObjectsToClients();

    /**
     * @brief 작업 처리
     * @param task 처리할 작업
     */
    void ProcessTask(P_MapTask* task);

    /**
     * @brief 죽은 오브젝트 처리
     * @param object 처리할 오브젝트
     * @param deltaTime 델타 타임
     */
    void HandleDeadObject(Object* object, float deltaTime);

    /**
     * @brief 활성 오브젝트 처리
     * @param object 처리할 오브젝트
     * @param deltaTime 델타 타임
     */
    void ProcessActiveObject(Object* object, float deltaTime);

    /**
     * @brief 오브젝트 유효성 검사
     * @param object 검사할 오브젝트
     * @return 유효한 오브젝트인지 여부
     */
    bool IsObjectValid(Object* object);

    // === 원래 API 호환성 유지 ===

 

   

   

    /**
     * @brief 구 API 호환 - 게임 상태 반환
     */
    bool Get_Gamestate() const;

    /**
     * @brief 구 API 호환 - 맵 시간 반환
     */
    float Get_Map_time() const;

    /**
    * @brief AI 관련 함수
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

// === 템플릿 구현 ===

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