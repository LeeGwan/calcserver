#include "CompactBinaryReader.h"
#include "../../../Core/utils/Vector.h"

#include "../OptimizedBinaryPacketSerializer/OptimizedBinaryPacketSerializer.h"
// ============ CompactBinaryReader ±¸Çö ============

CompactBinaryReader::CompactBinaryReader(const std::vector<uint8_t>& data)
    : owned_data_(data), data_(owned_data_.data()), size_(owned_data_.size()), offset_(0) {
}

CompactBinaryReader::CompactBinaryReader(const uint8_t* data, size_t size)
    : data_(data), size_(size), offset_(0) {
}

bool CompactBinaryReader::HasData(size_t bytes) const {
    return offset_ + bytes <= size_;
}

std::string CompactBinaryReader::ReadDirectString()
{
    uint16_t length = ReadUInt16();
    std::string result;
    result.reserve(length);
    for (uint16_t i = 0; i < length; ++i) {
        result.push_back(static_cast<char>(ReadUInt8()));
    }
    return result;
}

uint8_t CompactBinaryReader::ReadUInt8() {
    if (!HasData(1)) return 0;
    return data_[offset_++];
}

int16_t CompactBinaryReader::ReadInt16() {
    if (!HasData(2)) return 0;
    int16_t value = data_[offset_] | (data_[offset_ + 1] << 8);
    offset_ += 2;
    return value;
}

uint16_t CompactBinaryReader::ReadUInt16() {
    if (!HasData(2)) return 0;
    uint16_t value = data_[offset_] | (data_[offset_ + 1] << 8);
    offset_ += 2;
    return value;
}

uint32_t CompactBinaryReader::ReadUInt32() {
    if (!HasData(4)) return 0;
    uint32_t value = data_[offset_] | (data_[offset_ + 1] << 8) |
        (data_[offset_ + 2] << 16) | (data_[offset_ + 3] << 24);
    offset_ += 4;
    return value;
}

int32_t CompactBinaryReader::ReadInt32() {
    return static_cast<int32_t>(ReadUInt32());
}

float CompactBinaryReader::ReadCompactFloat(float precision) {
    uint32_t compact_value = ReadInt32();
    return static_cast<float>(compact_value) * precision;
}

Vec3 CompactBinaryReader::ReadCompactVec3() {
    Vec3 vec;
    vec.x = static_cast<float>(ReadInt32()) / 100.0f;
    vec.y = static_cast<float>(ReadInt32()) / 100.0f;
    vec.z = static_cast<float>(ReadInt32()) / 100.0f;
    return vec;
}



uint8_t CompactBinaryReader::ReadBitFlags() {
    return ReadUInt8();
}