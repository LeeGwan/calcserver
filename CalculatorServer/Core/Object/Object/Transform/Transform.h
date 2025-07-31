#pragma once
#include "../../../utils/Vector.h"
#include <mutex>
#include <shared_mutex>
#include <string>
#include "../../../../RoutineServer/Message/PacketStructure/PacketStructure.h"
enum TransformChangedFlags : uint16_t
{
    PositionChanged = 1 << 0,  // bit 0
    RotationChanged = 1 << 1,
};
#define ADD_TRANSFORM_MEMBER(TYPE, NAME)                                \
private:                                                                \
    TYPE NAME = {};                                                     \
public:                                                                 \
    TYPE Get_##NAME() const { std::shared_lock lock(Transformmutex); return NAME; } \
    Transform& Set_##NAME(const TYPE& value) { std::unique_lock lock(Transformmutex); NAME = value; return *this; }

#define CHECK_TRANSFORM_STATE(TYPE, NAME, FLAG) \
private: \
TYPE NAME = {}; \
public: \
TYPE Get_##NAME() const { std::shared_lock lock(Transformmutex); return NAME; } \
Transform& Set_##NAME(const TYPE& value) { \
std::unique_lock lock(Transformmutex); \
NAME = value; \
ChangedFlags |= FLAG; \
return *this; \
}
class Transform final
{
public:
	void clear();
    void SET(const Vec3 in_vec);
    inline bool HasFlag(const uint16_t& flags)const { return (ChangedFlags & flags) != 0; };
    inline void ClearFlag(const uint16_t& flags) { ChangedFlags &= ~flags; };
    CHECK_TRANSFORM_STATE(Vec3, Position,PositionChanged)//클라이언트
    CHECK_TRANSFORM_STATE(Vec3, Rotation, RotationChanged)//클라이언트
    ADD_TRANSFORM_MEMBER(Vec3, Scale)


    operator Struct_Transform() const 
    {
        Struct_Transform out;
        out.Position = Position;
        out.Rotation = Rotation;
        out.Scale = Scale;
        return out;
    }
   // float 
private:
	mutable std::shared_mutex Transformmutex;
    uint16_t ChangedFlags;
};

