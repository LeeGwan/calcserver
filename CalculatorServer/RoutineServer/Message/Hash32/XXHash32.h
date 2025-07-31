#pragma once
#include <cstdint>
class XXHash32 
{
private:
    static const uint32_t PRIME32_1 = 0x9E3779B1U;
    static const uint32_t PRIME32_2 = 0x85EBCA77U;
    static const uint32_t PRIME32_3 = 0xC2B2AE3DU;
    static const uint32_t PRIME32_4 = 0x27D4EB2FU;
    static const uint32_t PRIME32_5 = 0x165667B1U;

    static uint32_t rotl32(uint32_t x, int r);
    static uint32_t read32(const uint8_t* ptr);
public:
    static uint32_t hash(const void* input, size_t len, uint32_t seed = 0);
};

