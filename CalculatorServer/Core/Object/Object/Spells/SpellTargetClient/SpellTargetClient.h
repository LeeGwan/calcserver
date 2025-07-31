#pragma once
#include <mutex>
#include <shared_mutex>
#include "../../../../utils/Vector.h"
class Object;

class SpellTargetClient final
{
public:
	SpellTargetClient();
	virtual ~SpellTargetClient() = default;
	Object* Get_CasterObject()const;
	Object* Get_TargetObject()const;
	Vec3	Get_Start_Position()const;
	Vec3	Get_End_Position()const;

	void	Set_CasterObject(Object* in_Caster);
	void	Set_TargetObject(Object* in_Target);
	void	Set_Start_Position(const Vec3& in_Start_Position);
	void	Set_End_Position(const Vec3& in_End_Position);
	void	Release();
private:
	mutable std::shared_mutex SpellTargetClientmutex;
	Object* CasterObject;
	Object* TargetObject;
	Vec3 Start_Position;
	Vec3 End_Position;
};

