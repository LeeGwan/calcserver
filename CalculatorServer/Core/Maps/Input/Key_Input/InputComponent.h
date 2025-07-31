#pragma once
#include <memory>
#include <unordered_map>
#include <string>
namespace ErrorCode { enum Input_ErrorType : uint8_t; };
namespace Enums { enum CurrentOrderType : uint8_t; };
class Object;
class VMT_For_OBJ;
struct MoveOrder;
class NavMesh;
class MoveAndAttackComponent;

class InputComponent final
{
public:
    InputComponent();
    ~InputComponent();

    // �� ������ - ��� �ʱ�ȭ �� ����
    ErrorCode::Input_ErrorType ProcessCommand(NavMesh* nav_mesh, const MoveOrder& task,
        const std::unordered_map<std::string, Object*>& objects,
        const float& map_time);

    // ������Ʈ ��ƾ - �������� ��� ó��
    ErrorCode::Input_ErrorType UpdateCommand(NavMesh* nav_mesh, const Enums::CurrentOrderType& order_type,
        Object* hero, VMT_For_OBJ* vmt,
        const std::unordered_map<std::string, Object*>& objects,
        const float& delta_time, const float& map_time);

private:
    std::unique_ptr<MoveAndAttackComponent> move_attack_component_;

    // ��ü ���� �� ��������
    ErrorCode::Input_ErrorType ValidateAndGetObjects(const MoveOrder& task,
        const std::unordered_map<std::string, Object*>& objects,
        Object*& hero, Object*& target);

    // ��� Ÿ�� ����
    Enums::CurrentOrderType DetermineOrderType(const MoveOrder& task, Object* hero,
        Object* target, VMT_For_OBJ* vmt);
};