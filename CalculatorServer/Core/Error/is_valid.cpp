#include "is_valid.h"
#include"../Object/Object/ObjectComponent/Object.h"
#include "../Ability/SkillAbility/SkillAbility.h"
#include "../Algorithm/SkillDetected/SkillDetected.h"
template<typename T>
bool MemoryCheck::is_valid(T* in_check)
{
    static_assert(std::is_pointer_v<T*>, "T must be a pointer type");
    if (!in_check)return false;
    if (!check_memory(reinterpret_cast<uint64_t*>(in_check)))return false;
    if constexpr (std::is_same<T, Object>::value)
    {
        Object* Check_obj = dynamic_cast<Object*>(in_check);
        return !Check_obj->can_use_obj();
    }
    else if constexpr (std::is_same<T, SkillAbility>::value)
    {
        SkillAbility* Check_obj = dynamic_cast<SkillAbility*>(in_check);
        return !Check_obj->can_use_Ability();
    }
    else if constexpr (std::is_same<T, PolyFlagInfo>::value)
    {
        return true;
    }
    else
        return false;
}
bool MemoryCheck::check_memory(uint64_t* ptr)
{
    std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(ptr);

    // 디버그 패턴 체크
    if (!addr ||
        addr == 0xCCCCCCCCCCCCCCCCULL ||
        addr == 0xCDCDCDCDCDCDCDCDULL ||
        addr == 0xDDDDDDDDDDDDDDDDULL ||
        addr == 0xFEEEFEEEFEEEFEEEULL) {
        return false;
    }

#ifdef _WIN32
    return (addr >= 0x10000ULL && addr <= 0x00007FFFFFFFFFFFULL);
#else
    return (addr >= 0x1000ULL && addr <= 0x00007FFFFFFFFFFFULL);
#endif

}
template bool MemoryCheck::is_valid(Object*);
template bool MemoryCheck::is_valid(SkillAbility*);
template bool MemoryCheck::is_valid(PolyFlagInfo*);