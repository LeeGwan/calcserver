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

    // 주 진입점 - 명령 초기화 및 실행
    ErrorCode::Input_ErrorType ProcessCommand(NavMesh* nav_mesh, const MoveOrder& task,
        const std::unordered_map<std::string, Object*>& objects,
        const float& map_time);

    // 업데이트 루틴 - 지속적인 명령 처리
    ErrorCode::Input_ErrorType UpdateCommand(NavMesh* nav_mesh, const Enums::CurrentOrderType& order_type,
        Object* hero, VMT_For_OBJ* vmt,
        const std::unordered_map<std::string, Object*>& objects,
        const float& delta_time, const float& map_time);

private:
    std::unique_ptr<MoveAndAttackComponent> move_attack_component_;

    // 객체 검증 및 가져오기
    ErrorCode::Input_ErrorType ValidateAndGetObjects(const MoveOrder& task,
        const std::unordered_map<std::string, Object*>& objects,
        Object*& hero, Object*& target);

    // 명령 타입 결정
    Enums::CurrentOrderType DetermineOrderType(const MoveOrder& task, Object* hero,
        Object* target, VMT_For_OBJ* vmt);
};