#pragma once
#include <mutex>
#include <shared_mutex>
#include<memory>
enum spellslotChangedFlags : uint16_t
{
	CoolDownTimeChanged = 1 << 0,  // bit 0
	//RotationChanged = 1 << 1,
}; 

class SpellTargetClient;
class SpellData;
enum class SpellType : unsigned int;
enum class Slot : unsigned int;
class Spellslot final
{
public:
	Spellslot();
	~Spellslot();
	SpellTargetClient* Get_Target_Client()const;
	SpellData* Get_SpellData()const;
	Slot Get_Spell_Slot_id()const;
	unsigned int Get_SpellLv()const;
	bool Can_use()const;
	float Get_CoolDownTime()const;
	SpellType Get_spelltype()const;
	void Set_Spell_Slot_id(const Slot& in_Slot);
	void Set_SpellLv(const unsigned int& in_SpellLv);
	void Set_CoolDownTime(const float& in_CoolDownTime);
	inline bool HasFlag(const uint16_t& flags)const { return (ChangedFlags & flags) != 0; };
	inline void ClearFlag(const uint16_t& flags) { ChangedFlags &= ~flags; };
	void Release();


private:
	mutable std::shared_mutex Spellslotmutex;
	std::unique_ptr<SpellTargetClient> Target_Client;
	std::unique_ptr<SpellData> Spell_Data;
	Slot Spell_Slot_id;
	unsigned int SpellLv;
	float CoolDownTime;//실제 스펠의 쿨타임
	uint16_t ChangedFlags;
	//아이콘 가져오는거 클라에 만들자
};

