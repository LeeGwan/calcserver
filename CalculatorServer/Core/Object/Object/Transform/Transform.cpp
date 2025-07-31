#include "Transform.h"

void Transform::clear()
{
    std::unique_lock lock(Transformmutex);
    Position = {};
    Rotation = {};
    Scale = {};
    ChangedFlags = 0;

}

void Transform::SET(const Vec3 in_vec)
{
    std::unique_lock lock(Transformmutex);
    Position = in_vec;
}
