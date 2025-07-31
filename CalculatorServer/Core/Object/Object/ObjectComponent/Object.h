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
 * @brief ���� �� ��� ��ü(����, ����, ������ ��)�� �⺻ Ŭ����
 *
 * �� Ŭ������ ���� �� ��� ��ü�� �������� ������ �� �����Ϳ� ����� �����մϴ�.
 * ������Ʈ ��� ���踦 ����Ͽ� �� ����� ���ȭ�߽��ϴ�.
 * ��Ƽ������ ȯ�濡�� �����ϰ� �����ϵ��� shared_mutex�� ����մϴ�.
 */
class Object
{
public:
    Object();
    ~Object();

    // ��ü ���� ����
    bool can_use_obj() const;
    void Set_use_obj(bool in_use_obj);

    // ��ü �ʱ�ȭ �� ����
    void init_Object(const std::string& in_Name, const std::string& in_Hash, int in_team,unsigned long object_type);
    void Release();  // ��ü ��ȯ �� ���

    // �⺻ ���� ������ (������ ����)
    std::string Get_Name() const;
    void Set_Name(const std::string& in_Name);
    std::string Get_Hash() const;
    void Set_Hash(const std::string& in_Hash);
    int Get_Team()const;

    // ������Ʈ �����ڵ�
    // ����: ���� ��ȯ �޼������ ȣ���ڰ� ������ �������� �����ؾ� ��
    Transform& Get_Transform_Component();
    CombatStats& Get_CombatStats_Component();
    ObjectStats& Get_ObjectStats_Component();
    objectState& Get_objectState_Component();
    CharacterMesh& Get_CharacterMesh_Component();

    // ������ ��ȯ �޼���� (������ ����)
    Navigation* Get_Navigation_Component();
    VMT_For_OBJ* Get_VMT_Component();
    TargetSelector* Get_TargetSelector_Component();
    SpellBook* Get_P_SpellBook();
    VectorUtils* Get_m_VectorUtils();
private:
    // ������ ����ȭ�� ���ؽ� (�б�/���� �� ����)
    mutable std::shared_mutex objectmutex;

    // �⺻ ����
    std::string Name;
    std::string Hash;
    bool use_obj;

    // ������ ������Ʈ�� (�� Ÿ��)
    Transform O_Transform;
    CombatStats O_CombatStats;
    ObjectStats O_objectStats;
    objectState O_objectState;
    CharacterMesh O_CharacterMesh;

    // ������ ������Ʈ�� (���� �Ҵ�)
    std::unique_ptr<SpellBook> P_SpellBook;
    std::unique_ptr<Navigation> P_Navigation;
    std::unique_ptr<VMT_For_OBJ> P_VMT_For_OBJ;
    std::unique_ptr<TargetSelector> P_TargetSelector;
    std::unique_ptr<VectorUtils> m_VectorUtils;
};