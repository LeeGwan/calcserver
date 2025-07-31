#pragma once
#include<unordered_map>
#include <string>
class Object;
class OutputComponent;
class SkillDetected;
class NavMesh;
class Vec3;
class SkillAbility;
class MissileClient
{
public:
	bool InitSpell_Missile(NavMesh* Nav_Mesh,
		SkillDetected*& m_SkillDetected,const Vec3& startPos,
		const Vec3& endPos, float radius,
		float missilespeed, float cast_delay,
		unsigned short dangerousLevel,
		unsigned int in_skillid
		, unsigned long incollisionobject);
	void OnProcessSpell_Missile(const std::unordered_map<std::string, Object*>& objects, 
		OutputComponent* m_outputComponent, 
		int m_mapID,
		NavMesh* Nav_Mesh,
		SkillDetected*& m_SkillDetected,
		SkillAbility* castInfo,
		const float& Deltatime,
		const float& Maptime);


};

