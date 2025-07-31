#include "MissileClient.h"
#include "../../../../../Navmesh/Flags/Flags.h"
#include "../../../../../Navmesh/NavMesh.h"
#include "../../../../../Algorithm/NavigationService.h"
#include "../../../ObjectComponent/Object.h"
#include "../../../VMT/VMT.h"
#include "../../../../../utils/Find_OBJ/Find_OBJ.h"
#include "../../../../../Algorithm/SkillDetected/SkillDetected.h"
#include "../../../../../Maps/Output/OutputComponent.h"
#include "../../../Spells/SpellData/SpellData.h"
#include "../../../../../Ability/SkillAbility/SkillAbility.h"
#include "../../Spellslot/Spellslot.h"
#include "../../SpellTargetClient/SpellTargetClient.h"
#include "../../../CombatStats/CombatStats.h"
#include"../../SpellDataResource/SpellDataResource.h"
#include "../../../../../Error/is_valid.h"
#include <Windows.h>
#include"../../../../../DamageLib/DamageLib.h"
bool MissileClient::InitSpell_Missile(NavMesh* Nav_Mesh, SkillDetected*& m_SkillDetected, const Vec3& startPos, const Vec3& endPos, float radius, float missilespeed,float cast_delay ,unsigned short dangerousLevel,unsigned int in_skillid, unsigned long incollisionobject)
{
    if (missilespeed == 0)return false;
    float duration = (endPos.distance(startPos) / missilespeed)+ cast_delay;
    Vec3 N_startPos = startPos.to_Nav_POS();
    float f_startPos[3] = { N_startPos.x,N_startPos.y,N_startPos.z };
    Vec3 N_endPos = endPos.to_Nav_POS();
    float f_endPos[3] = { N_endPos.x,N_endPos.y,N_endPos.z };
   
    return m_SkillDetected->CreateMovingSkill(Nav_Mesh->Get_LeagueNavMesh(), Nav_Mesh->Get_navQuery(), f_startPos, f_endPos, radius*0.01f, dangerousLevel, duration,in_skillid,incollisionobject);
}

void MissileClient::OnProcessSpell_Missile(const std::unordered_map<std::string, Object*>& objects, 
    OutputComponent* m_outputComponent, int m_mapID, NavMesh* Nav_Mesh, SkillDetected*& m_SkillDetected, SkillAbility* castInfo
    , const float& Deltatime,
    const float& Maptime)
{

    unsigned int skillid = castInfo->Get_skillid();
    Spellslot* caster_Spellslot = castInfo->Get_P_Spell_Slot();
    unsigned int SpellLv = caster_Spellslot->Get_SpellLv();
    Slot skillslot = caster_Spellslot->Get_Spell_Slot_id();
    Object* caster= caster_Spellslot->Get_Target_Client()->Get_CasterObject();
    Object* target = nullptr;
  
    SpellDataResource* Spell_data = castInfo->Get_P_Spell_Slot()->Get_SpellData()->Get_Spell_Data_Resource();
    
    float radius = Spell_data->Get_Spell_Base_CastRadius();
    float missile_speed = Spell_data->Get_Spell_Base_missile_speed();
    Vec3 startPOS = castInfo->Get_StartPosion();
    Vec3 endPOS = castInfo->Get_EndPosion();
    // startPOS와 endPOS가 같은지 체크
    if (startPOS == endPOS) {
        m_outputComponent->DeleteSpell(m_mapID, skillid);
        m_SkillDetected->RemoveSkillEffect(Nav_Mesh->Get_LeagueNavMesh(), skillid);
        castInfo->Release();
        return;
    }
    Vec3 direction = (endPOS - startPOS).normalized();
    Vec3 movement = (direction * missile_speed) * Deltatime;
    Vec3 NewstartPOS = startPOS + movement;

    // 목표 지점을 넘어가지 않도록 체크
    float remainingDistance = (endPOS - startPOS).length();
    float moveDistance = movement.length();

    if (moveDistance >= remainingDistance)
    {
      // 목표 지점에 도달했으므로 스펠 삭제
        m_outputComponent->DeleteSpell(m_mapID, skillid);
        m_SkillDetected->RemoveSkillEffect(Nav_Mesh->Get_LeagueNavMesh(), skillid);
       castInfo->Release();
       return;

    }
    Vec3 N_NewstartPOS = NewstartPOS.to_Nav_POS();
    float f_NewstartPOS[3] = { N_NewstartPOS.x,N_NewstartPOS.y,N_NewstartPOS.z };
    castInfo->Set_StartPosion(NewstartPOS);

    if (m_SkillDetected->UpdateMovingSkillPosition(caster, target, objects, Nav_Mesh->Get_LeagueNavMesh(), Nav_Mesh->Get_navQuery(), skillid, f_NewstartPOS, radius * 0.01f))
    {
        if (MemoryCheck::is_valid<Object>(target))
        {
            if (!target->Get_VMT_Component()->is_dead())
            {
                DamageLib::compute_spell_damage(caster, target, skillslot, SpellLv);
                target->Get_CombatStats_Component().Set_last_combat_time(Maptime);
            }
        }
        m_outputComponent->DeleteSpell(m_mapID, skillid);
        m_SkillDetected->RemoveSkillEffect(Nav_Mesh->Get_LeagueNavMesh(), skillid);
     
    }
    
     return;
}


