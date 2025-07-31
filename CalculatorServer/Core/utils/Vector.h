#pragma once


#include <cstdint>
#include <string>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

namespace math
{
	/*  See
		https://randomascii.wordpress.com/2012/01/11/tricks-with-the-floating-point-format/
		for the potential portability problems with the union and bit-fields below.
	*/
	union Float_t
	{
		Float_t(float num = 0.0f) : f(num) {}
		// Portable extraction of components.
		[[nodiscard]] bool Negative() const { return i < 0; }
		[[nodiscard]] int32_t RawMantissa() const { return i & ((1 << 23) - 1); }
		[[nodiscard]] int32_t RawExponent() const { return (i >> 23) & 0xFF; }

		int32_t i;
		float f;
#ifdef _DEBUG
		struct
		{   // Bitfields for exploration. Do not use in production code.
			uint32_t mantissa : 23;
			uint32_t exponent : 8;
			uint32_t sign : 1;
		} parts;
#endif
	};

	bool IsZero(float A);
	bool NearEqual(float A, float B, int maxUlpsDiff = 4);
}

struct IntersectionResultVec2;
struct IntersectionResultVec3;
struct ProjectionInfo;
class Vec3;

class Vec2
{
public:
	float x, y;
	Vec2() { x = y = 0.f; };
	Vec2(float x, float y) : x(x), y(y) {};

	[[nodiscard]] Vec2					 operator+(const Vec2& v) const;
	[[nodiscard]] Vec2					 operator+(float f) const;
	[[nodiscard]] Vec2					 operator+=(const Vec2& v) const;
	[[nodiscard]] Vec2					 operator+=(float f) const;
	[[nodiscard]] Vec2					 operator-(const Vec2& v) const;
	[[nodiscard]] Vec2					 operator-(float f) const;
	[[nodiscard]] Vec2					 operator-=(const Vec2& v) const;
	[[nodiscard]] Vec2					 operator-=(float f) const;
	[[nodiscard]] Vec2					 operator*(float f) const;
	[[nodiscard]] Vec2					 operator*(const Vec2& v) const;
	[[nodiscard]] Vec2					 operator*=(const Vec2& v) const;
	[[nodiscard]] Vec2					 operator*=(float f) const;
	[[nodiscard]] Vec2					 operator/(const Vec2& v) const;
	[[nodiscard]] Vec2					 operator/(float f) const;
	[[nodiscard]] Vec2					 operator/=(const Vec2& v) const;
	[[nodiscard]] Vec2					 operator/=(float f) const;
	[[nodiscard]] bool					 operator==(const Vec2& v) const;
	[[nodiscard]] bool					 operator!=(const Vec2& v) const;

	[[nodiscard]] bool					 is_valid() const;
	[[nodiscard]] bool					 is_visible_on_screen() const;
	[[nodiscard]] float					 length() const;
	[[nodiscard]] float					 length_squared() const;
	[[nodiscard]] float					 distance(const Vec2& o) const;
	[[nodiscard]] float					 distance_squared(const Vec2& o) const;
	[[nodiscard]] Vec2					 v_scale(const Vec2& s) const;
	[[nodiscard]] Vec2					 scale(float s) const;
	[[nodiscard]] Vec2					 normalized() const;
	[[nodiscard]] Vec2					 append(const Vec2& pos1, Vec2& pos2, float dist) const;
	[[nodiscard]] Vec2					 rotated(float angle) const;
	[[nodiscard]] Vec2					 clone() const;
	[[nodiscard]] float					 polar() const;
	[[nodiscard]] float					 cross_product(const Vec2& other) const;
	[[nodiscard]] float					 angle_between(const Vec2& o) const;
	[[nodiscard]] IntersectionResultVec2 intersection(const Vec2& line_segment1_end, Vec2& line_segment2_start, Vec2& line_segment2_end) const;
	[[nodiscard]] ProjectionInfo		 project_on(const Vec2& segment_start, Vec2& segment_end) const;
	[[nodiscard]] Vec2					 extend(const Vec2& to, float distance) const;
	[[nodiscard]] Vec3					 to_3d(float height = 0) const;
	[[nodiscard]] std::string			 to_string() const;
};

class Vec3
{
public:
	float x, y, z;
	Vec3() { x = y = z=0.0f ; };
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {};

	[[nodiscard]] Vec3					operator+(const Vec3& v) const;
	[[nodiscard]] Vec3					operator+(float f) const;
	[[nodiscard]] Vec3					operator+=(const Vec3& v) const;
	[[nodiscard]] Vec3					operator+=(float f) const;
	[[nodiscard]] Vec3					operator-(const Vec3& v) const;
	[[nodiscard]] Vec3					operator-(float f) const;
	[[nodiscard]] Vec3					operator-=(const Vec3& v) const;
	[[nodiscard]] Vec3					operator-=(float f) const;
	[[nodiscard]] Vec3					operator*(const Vec3& v) const;
	[[nodiscard]] Vec3					operator*(float f) const;
	[[nodiscard]] Vec3					operator*=(const Vec3& v) const;
	[[nodiscard]] Vec3					operator*=(float f) const;
	[[nodiscard]] Vec3					operator/(const Vec3& v) const;
	[[nodiscard]] Vec3					operator/(float f) const;
	[[nodiscard]] Vec3					operator/=(const Vec3& v) const;
	[[nodiscard]] Vec3					operator/=(float f) const;
	[[nodiscard]] bool					operator!=(const Vec3& v) const;
	[[nodiscard]] bool					operator==(const Vec3& v) const;

	[[nodiscard]] bool					is_valid() const;
	[[nodiscard]] Vec3					switch_yz() const;
	[[nodiscard]] Vec3					negate() const;
	[[nodiscard]] float					dot_product(const Vec3& other) const;
	[[nodiscard]] float					cross_product(const Vec3& other) const;
	[[nodiscard]] float					polar() const;
	[[nodiscard]] float					angle_between(const Vec3& other) const;
	[[nodiscard]] bool					close(float a, float b, float eps) const;
	[[nodiscard]] Vec3					rotated(float angle) const;
	[[nodiscard]] Vec3					perpendicular() const;
	[[nodiscard]] Vec3					append(const Vec3& pos1, Vec3& pos2, float dist) const;
	[[nodiscard]] Vec3					clone() const;
	[[nodiscard]] float					length() const;
	[[nodiscard]] float					length_2D() const;
	[[nodiscard]] float					length_squared() const;
	[[nodiscard]] float					distance(const Vec3& to ,bool use_height = false) const;
	[[nodiscard]] float					Nav_distance(const Vec3& to, bool use_height = false) const;
	[[nodiscard]] float					distance_squared(const Vec3& to) const;
	[[nodiscard]] Vec3					extend(const Vec3& to, float distance) const;
	[[nodiscard]] Vec3					scale(float s) const;
	[[nodiscard]] Vec3					rotate(const Vec3& startPos, float theta) const;
	[[nodiscard]] Vec3					rotate_x(float angle) const;
	[[nodiscard]] Vec3					rotate_y(float angle) const;
	[[nodiscard]] Vec3					rotate_z(float angle) const;
	[[nodiscard]] Vec3					normalized() const;
	[[nodiscard]] Vec3					set_height(float val = FLT_MIN) const;
	[[nodiscard]] Vec2					to_2d() const;
	[[nodiscard]] Vec3					to_Nav_POS() const;
	[[nodiscard]] Vec3					to_Unreal_POS() const;
	[[nodiscard]] std::string			to_string() const;
	[[nodiscard]] Vec3					moveTowards(const Vec3& to, float dist)const;
	[[nodiscard]] Vec3                  clamp_range(const Vec3& target, float max_range) const;
	[[nodiscard]] Vec3                  clamp_range_2D(const Vec3& target, float max_range) const;
	[[nodiscard]] bool                  is_in_range(const Vec3& target, float range) const;
	[[nodiscard]] bool                  is_in_range_2D(const Vec3& target, float range) const;
	[[nodiscard]] float					GetSignedAngleBetween(const Vec3& to)const;
	[[nodiscard]] Vec3					get_direction_offset(int direction_index, float search_radius_cm);
};

class Vec4
{
public:
	float x, y, z, w;
	Vec4() { x = y = z = w = 0.f; };
	Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
};

struct ProjectionInfo
{
	bool is_on_segment;
	Vec2 line_point;
	Vec2 segment_point;
	ProjectionInfo(bool onSegment, const Vec2& segmentPt, const Vec2& linePt)
		: is_on_segment(onSegment), segment_point(segmentPt), line_point(linePt) {}
};

struct IntersectionResultVec2
{
	int intersects;
	Vec2 intersection_point;
	IntersectionResultVec2(int Input_intersects, Vec2 Input_intersection_point)
		:intersects(Input_intersects), intersection_point(Input_intersection_point) {}
	
};

struct IntersectionResultVec3
{
	bool intersects;
	Vec3 intersection_point;
};