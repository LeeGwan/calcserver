#include "Spellslot.h"
#include "../SpellFlags/SpellFlags.h"
#include "../SpellTargetClient/SpellTargetClient.h"
#include "../SpellData/SpellData.h"
#include "../SpellDataResource/SpellDataResource.h"
Spellslot::Spellslot()
{
    Target_Client = std::make_unique<SpellTargetClient>();
    Spell_Data = std::make_unique<SpellData>();
    SpellLv = 0;
    CoolDownTime = 0.0f;
    ChangedFlags &= 0;
}

Spellslot::~Spellslot() = default;


SpellTargetClient* Spellslot::Get_Target_Client() const
{
    std::shared_lock lock(Spellslotmutex);
    return Target_Client.get();
}

SpellData* Spellslot::Get_SpellData() const
{
    std::shared_lock lock(Spellslotmutex);
    return Spell_Data.get();
}

Slot Spellslot::Get_Spell_Slot_id() const
{
    std::shared_lock lock(Spellslotmutex);
    return Spell_Slot_id;
}

unsigned int Spellslot::Get_SpellLv() const
{
    std::shared_lock lock(Spellslotmutex);
    return SpellLv;
}

bool Spellslot::Can_use() const
{
    if(Get_SpellLv()==0)
    //½ºÆç »óÅÂ 1,·¹¹ëÀÌ 0ÀÌ³Ä 
    return false;
}

float Spellslot::Get_CoolDownTime() const
{
    std::unique_lock lock(Spellslotmutex);
    return CoolDownTime;
}

SpellType Spellslot::Get_spelltype()const
{
    std::shared_lock lock(Spellslotmutex);
    return Spell_Data->Get_Spell_Data_Resource()->Get_SpellType();
}

void Spellslot::Set_Spell_Slot_id(const Slot& in_Slot)
{
    std::unique_lock lock(Spellslotmutex);
    Spell_Slot_id = in_Slot;
}

void Spellslot::Set_SpellLv(const unsigned int& in_SpellLv)
{
    std::unique_lock lock(Spellslotmutex);
    SpellLv = in_SpellLv;
}

void Spellslot::Set_CoolDownTime(const float& in_CoolDownTime)
{
    std::unique_lock lock(Spellslotmutex);
    ChangedFlags |= CoolDownTimeChanged;
    CoolDownTime = in_CoolDownTime;
}
void Spellslot::Release()
{
    std::unique_lock lock(Spellslotmutex);
    
}
