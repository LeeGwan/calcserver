#pragma once
#include <cstdint> 
class MemoryCheck
{
public:
    template<typename T>
    static bool is_valid(T* in_check);
    static bool check_memory(uint64_t* ptr);
};