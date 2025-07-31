#pragma once
#include <vector>
#include <cstdint>
#include<string>
#include <iostream>
// ============ 압축된 바이너리 리더 클래스 ============
class CompactBinaryReader {
private:
    std::vector<uint8_t> owned_data_;
    const uint8_t* data_;
    size_t size_;
    size_t offset_;

public:
    CompactBinaryReader(const std::vector<uint8_t>& data);
    CompactBinaryReader(const uint8_t* data, size_t size);

    bool HasData(size_t bytes) const;
    //------------------테스트코드
    void Reset() { offset_ = 0; }
    size_t GetOffset() const { return offset_; }
    size_t GetSize() const { return size_; }
    void PrintData(int count = 10) const {
        std::cout << "Reader data from offset " << offset_ << ": ";
        for (int i = 0; i < count && offset_ + i < size_; i++) {
            std::cout << std::hex << (int)data_[offset_ + i] << " ";
        }
        std::cout << std::dec << std::endl;
    }

    //--------------------------
    std::string ReadDirectString();
    uint8_t ReadUInt8();
    int16_t ReadInt16();
    uint16_t ReadUInt16();
    uint32_t ReadUInt32();
    int32_t ReadInt32();
    float ReadCompactFloat(float precision = 0.01f);
    class Vec3 ReadCompactVec3();
    uint8_t ReadBitFlags();
};