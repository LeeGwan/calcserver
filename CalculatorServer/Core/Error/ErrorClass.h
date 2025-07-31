#pragma once
#include <cstdint>
#include <string>
#include <type_traits>

namespace ErrorCode
{
    enum Input_ErrorType : uint8_t
    {
        Default,
        Successed,
        Error_Cant_Find_OBJ_Dont_Have_OBJ,
        Error_Cant_Find_OBJ_Cant_Use,
        Error_Cant_Find_OBJ_For_Use_Hash,
        Error_Is_dead_or_cant_use,
        Error_Cant_Move,
        Error_Cant_Attack,
        Error_Dont_Have_Navi,
        Error_Navi,
        UniversalError,
        Anti_Hack_Check_Input
    };

    enum Spell_ErrorType : uint8_t
    {
        Unkown,
        Spell_Successed,
        Spell_DeleteOBJ_Successed,
        Spell_Error_Dont_have_hash,
        Spell_Error_Dont_have_OBJ,
        Spell_Error_Is_dead,
        Spell_Error_Is_not_Spell,
        Spell_Error_Unknown,
        Spell_Error_CantSpell,
        Spell_Error_Supressed,
        Spell_Error_NotLearned,
        Spell_Error_Channeling,
        Spell_Error_Cooldown,
        Spell_Error_NoMana,
        Spell_Error_Cant_Acquire_object,
        Spell_Error_Cant_Find_SpellSlotList,
        Anti_Hack_Check_Spell
    };

    enum Level_Input_ErrorType : uint8_t
    {
        Anti_Hack_Check_Level_Input,
        Level_Input_Successed,
        Level_Input_Error_Dont_have_hash,
        Level_Input_Error_Dont_have_OBJ
    };
}

class Error_Class
{
public:
    // Input error 생성자
    Error_Class(ErrorCode::Input_ErrorType code)
        : input_error_code(code), spell_error_set(false), level_input_error_set(false) {
    }

    // Spell error 생성자
    Error_Class(ErrorCode::Spell_ErrorType code)
        : spell_error_code(code), spell_error_set(true), level_input_error_set(false) {
    }

    // Level input error 생성자
    Error_Class(ErrorCode::Level_Input_ErrorType code)
        : level_input_error_code(code), spell_error_set(false), level_input_error_set(true) {
    }

    // operator() : error 메시지 출력용
    std::string operator()() const {
        if (spell_error_set)
            return get_error_message(spell_error_code);
        else if (level_input_error_set)
            return get_error_message(level_input_error_code);
        else
            return get_error_message(input_error_code);
    }

    // 템플릿 기반 에러코드 getter
    template<typename T>
    T Get() const {
        static_assert(
            std::is_same<T, ErrorCode::Input_ErrorType>::value ||
            std::is_same<T, ErrorCode::Spell_ErrorType>::value ||
            std::is_same<T, ErrorCode::Level_Input_ErrorType>::value,
            "Unsupported error type"
            );

        if constexpr (std::is_same<T, ErrorCode::Input_ErrorType>::value) {
            return input_error_code;
        }
        else if constexpr (std::is_same<T, ErrorCode::Spell_ErrorType>::value) {
            return spell_error_code;
        }
        else {
            return level_input_error_code;
        }
    }

private:
    ErrorCode::Input_ErrorType input_error_code = ErrorCode::Input_ErrorType::Default;
    ErrorCode::Spell_ErrorType spell_error_code = ErrorCode::Spell_ErrorType::Unkown;
    ErrorCode::Level_Input_ErrorType level_input_error_code = ErrorCode::Level_Input_ErrorType::Anti_Hack_Check_Level_Input;

    bool spell_error_set = false;
    bool level_input_error_set = false;

    static std::string get_error_message(ErrorCode::Input_ErrorType code)
    {
        switch (code)
        {
        case ErrorCode::Input_ErrorType::Default: return "Default input error";
        case ErrorCode::Input_ErrorType::Successed: return "Input succeeded";
        case ErrorCode::Input_ErrorType::Error_Cant_Find_OBJ_Dont_Have_OBJ: return "Object not found or does not exist";
        case ErrorCode::Input_ErrorType::Error_Cant_Find_OBJ_Cant_Use: return "Object found but cannot be used";
        case ErrorCode::Input_ErrorType::Error_Cant_Find_OBJ_For_Use_Hash: return "Object hash invalid or not found";
        case ErrorCode::Input_ErrorType::Error_Is_dead_or_cant_use: return "Object is dead or cannot be used";
        case ErrorCode::Input_ErrorType::Error_Cant_Move: return "Cannot move to the target location";
        case ErrorCode::Input_ErrorType::Error_Cant_Attack: return "Cannot attack the target";
        case ErrorCode::Input_ErrorType::Error_Dont_Have_Navi: return "Navigation data missing";
        case ErrorCode::Input_ErrorType::Error_Navi: return "Navigation pathfinding error";
        case ErrorCode::Input_ErrorType::UniversalError: return "General error occurred";
        case ErrorCode::Input_ErrorType::Anti_Hack_Check_Input: return "Anti-hack input";
        default: return "Unknown Input Error";
        }
    }

    static std::string get_error_message(ErrorCode::Spell_ErrorType code)
    {
        switch (code)
        {
        case ErrorCode::Spell_ErrorType::Unkown: return "Unknown spell error";
        case ErrorCode::Spell_ErrorType::Spell_Successed: return "Spell succeeded";
        case ErrorCode::Spell_ErrorType::Spell_DeleteOBJ_Successed: return "Spell object deleted successfully";
        case ErrorCode::Spell_ErrorType::Spell_Error_Dont_have_hash: return "Spell cast failed: missing object hash";
        case ErrorCode::Spell_ErrorType::Spell_Error_Dont_have_OBJ: return "Spell cast failed: target object not found";
        case ErrorCode::Spell_ErrorType::Spell_Error_Is_dead: return "Cannot cast spell: object is dead";
        case ErrorCode::Spell_ErrorType::Spell_Error_Is_not_Spell: return "Cast failed: not a valid spell";
        case ErrorCode::Spell_ErrorType::Spell_Error_Unknown: return "Spell failed due to unknown reason";
        case ErrorCode::Spell_ErrorType::Spell_Error_CantSpell: return "Cannot cast spell at this time";
        case ErrorCode::Spell_ErrorType::Spell_Error_Supressed: return "Spell casting is currently suppressed";
        case ErrorCode::Spell_ErrorType::Spell_Error_NotLearned: return "Spell not learned yet";
        case ErrorCode::Spell_ErrorType::Spell_Error_Channeling: return "Spell is being channeled";
        case ErrorCode::Spell_ErrorType::Spell_Error_Cooldown: return "Spell is on cooldown";
        case ErrorCode::Spell_ErrorType::Spell_Error_NoMana: return "Not enough mana to cast spell";
        case ErrorCode::Spell_ErrorType::Spell_Error_Cant_Acquire_object: return "Cannot acquire target object";
        case ErrorCode::Spell_ErrorType::Spell_Error_Cant_Find_SpellSlotList: return "Spell slot list not found";
        case ErrorCode::Spell_ErrorType::Anti_Hack_Check_Spell: return "Anti-hack spell";
        default: return "Unknown Spell Error";
        }
    }

    static std::string get_error_message(ErrorCode::Level_Input_ErrorType code)
    {
        switch (code)
        {
        case ErrorCode::Level_Input_ErrorType::Anti_Hack_Check_Level_Input: return "Anti-hack level input";
        case ErrorCode::Level_Input_ErrorType::Level_Input_Successed: return "Level_Input succeeded";
        case ErrorCode::Level_Input_ErrorType::Level_Input_Error_Dont_have_hash: return "Level_Input failed: missing object hash";
        case ErrorCode::Level_Input_ErrorType::Level_Input_Error_Dont_have_OBJ: return "Level_Input failed: target object not found"; 
        default: return "Unknown Level Input Error";
        }
    }
};
