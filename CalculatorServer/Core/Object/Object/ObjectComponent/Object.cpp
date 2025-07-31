#include "Object.h"
#include "../DB/ObjectDB/ObjectLib.h"
#include "../DB/SpellDB/SpellLib.h"
#include "../Navgation/Navigation.h"
#include "../VMT/VMT.h"
#include "../TargetSelector/TargetSelector.h"
#include "../Spells/SpellBook/SpellBook.h"
#include <iostream>
#include <string>
#include <vector>
#include"../../../Maps/MapSchedule.h"
#include"../DB/ObjectDB/Struct_ObjectDB.h"
#include "../../../utils/VectorUtils.h"
Object::Object()
{
    // ������Ʈ �ʱ�ȭ
    P_SpellBook = std::make_unique<SpellBook>();
    P_Navigation = std::make_unique<Navigation>();
    P_VMT_For_OBJ = std::make_unique<VMT_For_OBJ>(this);
    P_TargetSelector = std::make_unique<TargetSelector>();
    m_VectorUtils=std::make_unique<VectorUtils>();

    // ��ü ��� ���� ���·� ����
    use_obj = true;
}
Object::~Object() = default;
bool Object::can_use_obj() const
{
    std::shared_lock<std::shared_mutex> readlock(objectmutex);
    return use_obj;
}

void Object::Set_use_obj(bool in_use_obj)
{
    std::unique_lock<std::shared_mutex> writelock(objectmutex);
    use_obj = in_use_obj;
}

void Object::init_Object(const std::string& in_Name, const std::string& in_Hash, int in_team, unsigned long object_type)
{
    std::unique_lock<std::shared_mutex> writelock(objectmutex);

    // �⺻ ���� ����
    Name = in_Name;
    Hash = in_Hash;

    // �����ͺ��̽����� ��ü ���� �ε�
    Struct_ObjectDB init_ObjectDB;
    std::vector<Struct_SpellDB> init_SpellDB;
    init_ObjectDB.heroname = in_Name;
    if (!ObjectDB::Get_ObjectDB(init_ObjectDB, in_Name))
    {
        std::cout << "��ü �ʱ�ȭ ����: " << in_Name << std::endl;
       
    }

    // �����ͺ��̽����� ������Ʈ ������ ����
    init_ObjectDB.O_CharacterMesh.CopyTo(O_CharacterMesh);
    init_ObjectDB.O_CombatStats.CopyTo(O_CombatStats);
    init_ObjectDB.O_objectStats.CopyTo(O_objectStats);
    init_ObjectDB.O_objectState.CopyTo(O_objectState);

    // �� ����
    O_objectState.Set_Team(in_team);

   O_objectState.Set_ObjectTypeFlag(object_type);
 

    
    // ������ ��� ��ų ���� �ε�
    if (P_VMT_For_OBJ->is_hero())
    {
        SpellDB::Get_SpellDB(init_SpellDB, in_Name);
        O_objectState.Set_Level(6);  // ���� �⺻ ����
        P_SpellBook->import_from_database(init_SpellDB);
    }
    else
    {
        O_objectState.Set_Level(1);  // �Ϲ� ��ü �⺻ ����
    }
}

void Object::Release()
{
    std::unique_lock<std::shared_mutex> writelock(objectmutex);

    // ��ü ��� ����
    use_obj = false;

    // ������Ʈ ����
    if (P_Navigation)
    {
        P_Navigation->clear();
        P_Navigation.reset();
    }

    // ������ ������Ʈ ����
    O_Transform.clear();
    O_CombatStats.clear();
    O_objectStats.clear();
    O_objectState.clear();
    O_CharacterMesh.clear();

    // ����Ʈ ������ ����
    P_VMT_For_OBJ.reset();
    P_SpellBook.reset();
    P_TargetSelector.reset();
}

std::string Object::Get_Name() const
{
    std::shared_lock<std::shared_mutex> readlock(objectmutex);
    return Name;
}

void Object::Set_Name(const std::string& in_Name)
{
    std::unique_lock<std::shared_mutex> writelock(objectmutex);
    Name = in_Name;
}

std::string Object::Get_Hash() const
{
    std::shared_lock<std::shared_mutex> readlock(objectmutex);
    return Hash;
}

void Object::Set_Hash(const std::string& in_Hash)
{
    std::unique_lock<std::shared_mutex> writelock(objectmutex);
    Hash = in_Hash;
}

int Object::Get_Team() const
{
    std::shared_lock<std::shared_mutex> readlock(objectmutex);
    return O_objectState.Get_Team();
}


// ������Ʈ ������ �޼���� (�б� �����̹Ƿ� shared_lock ���)
Transform& Object::Get_Transform_Component()
{

    return O_Transform;
}

CombatStats& Object::Get_CombatStats_Component()
{
    return O_CombatStats;
}

ObjectStats& Object::Get_ObjectStats_Component()
{
    return O_objectStats;
}

objectState& Object::Get_objectState_Component()
{
    return O_objectState;
}

CharacterMesh& Object::Get_CharacterMesh_Component()
{
    return O_CharacterMesh;
}

Navigation* Object::Get_Navigation_Component()
{
    std::shared_lock<std::shared_mutex> readlock(objectmutex);
    return P_Navigation.get();
}

VMT_For_OBJ* Object::Get_VMT_Component()
{
    std::shared_lock<std::shared_mutex> readlock(objectmutex);
    return P_VMT_For_OBJ.get();
}

TargetSelector* Object::Get_TargetSelector_Component()
{
    std::shared_lock<std::shared_mutex> readlock(objectmutex);
    return P_TargetSelector.get();
}

SpellBook* Object::Get_P_SpellBook()
{
    std::shared_lock<std::shared_mutex> readlock(objectmutex);
    return P_SpellBook.get();
}

VectorUtils* Object::Get_m_VectorUtils()
{
    std::shared_lock<std::shared_mutex> readlock(objectmutex);
    return m_VectorUtils.get();
}
