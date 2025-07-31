#pragma once
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <string>
#include "../Flags.h"
#include "../Transform/Transform.h"
#include "../objectState/objectState.h"
#include "../CombatStats/CombatStats.h"
#include "../objectStats/objectStats.h"
#include "../CharacterMesh/CharacterMesh.h"



struct Create_Map_data;
enum class PacketType : uint8_t;
class Navigation;
class VMT_For_OBJ;
class TargetSelector;
class SpellBook;
class VectorUtils;
/**
 * @brief 게임 내 모든 객체(영웅, 몬스터, 구조물 등)의 기본 클래스
 *
 * 이 클래스는 게임 내 모든 객체가 공통으로 가져야 할 데이터와 기능을 제공합니다.
 * 컴포넌트 기반 설계를 사용하여 각 기능을 모듈화했습니다.
 * 멀티스레딩 환경에서 안전하게 동작하도록 shared_mutex를 사용합니다.
 */
class Object
{
public:
    Object();
    ~Object();

    // 객체 상태 관리
    bool can_use_obj() const;
    void Set_use_obj(bool in_use_obj);

    // 객체 초기화 및 정리
    void init_Object(const std::string& in_Name, const std::string& in_Hash, int in_team,unsigned long object_type);
    void Release();  // 객체 반환 시 사용

    // 기본 정보 접근자 (스레드 안전)
    std::string Get_Name() const;
    void Set_Name(const std::string& in_Name);
    std::string Get_Hash() const;
    void Set_Hash(const std::string& in_Hash);
    int Get_Team()const;

    // 컴포넌트 접근자들
    // 주의: 참조 반환 메서드들은 호출자가 스레드 안전성을 보장해야 함
    Transform& Get_Transform_Component();
    CombatStats& Get_CombatStats_Component();
    ObjectStats& Get_ObjectStats_Component();
    objectState& Get_objectState_Component();
    CharacterMesh& Get_CharacterMesh_Component();

    // 포인터 반환 메서드들 (스레드 안전)
    Navigation* Get_Navigation_Component();
    VMT_For_OBJ* Get_VMT_Component();
    TargetSelector* Get_TargetSelector_Component();
    SpellBook* Get_P_SpellBook();
    VectorUtils* Get_m_VectorUtils();
private:
    // 스레드 동기화용 뮤텍스 (읽기/쓰기 락 지원)
    mutable std::shared_mutex objectmutex;

    // 기본 정보
    std::string Name;
    std::string Hash;
    bool use_obj;

    // 데이터 컴포넌트들 (값 타입)
    Transform O_Transform;
    CombatStats O_CombatStats;
    ObjectStats O_objectStats;
    objectState O_objectState;
    CharacterMesh O_CharacterMesh;

    // 포인터 컴포넌트들 (동적 할당)
    std::unique_ptr<SpellBook> P_SpellBook;
    std::unique_ptr<Navigation> P_Navigation;
    std::unique_ptr<VMT_For_OBJ> P_VMT_For_OBJ;
    std::unique_ptr<TargetSelector> P_TargetSelector;
    std::unique_ptr<VectorUtils> m_VectorUtils;
};