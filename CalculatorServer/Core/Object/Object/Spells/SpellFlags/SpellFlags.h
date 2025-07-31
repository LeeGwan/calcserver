#pragma once

enum class Slot : unsigned int
{
	Q,
	W,
	E,
	R,
	D,
	F,
	None
};
enum class SpellType : unsigned int
{
	None,
	Line,
	Missile,
	Circle,//미사일스피드가 있냐없냐 있으면 럭스e 없으면 갱플궁
	Blink,
	Attack
};
enum class CollisionObject : unsigned long
{
	Minions,
	Heroes,
	Allies,
	YasuoWall, // yasuo, samira
	GameWall,
};
enum class DangerLevel
{
	Never,
	Low ,
	Medium,
	High,
	VeryHigh,
	Extreme
};
enum class SpellState : int 
{
	Unknown = -1,
	Ready = 0,
	CantSpell = 4,
	Supressed = 8,
	NotLearned = 12,
	Channeling = 24,
	Cooldown = 32,
	NoMana = 64,
};
inline CollisionObject operator|(CollisionObject lhs, CollisionObject rhs)
{
	return static_cast<CollisionObject>(
		static_cast<unsigned long>(lhs) | static_cast<unsigned long>(rhs)
		);
}

inline CollisionObject operator&(CollisionObject lhs, CollisionObject rhs)
{
	return static_cast<CollisionObject>(
		static_cast<unsigned long>(lhs) & static_cast<unsigned long>(rhs)
		);
}
inline unsigned long To_unsignedlong(const CollisionObject& in)
{
	return static_cast<unsigned long>(in);
}
inline Slot ToSlot(const unsigned int& in)
{

	return static_cast<Slot>(in);
}
inline unsigned int ToSlotindex(Slot in)
{

	return static_cast<unsigned int>(in);
}




