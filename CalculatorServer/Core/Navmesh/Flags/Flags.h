#pragma once
#include "../../Object/Object/Spells/SpellFlags/SpellFlags.h"
#include <cstdint> 
enum PolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP
};
/**
 * 폴리곤 플래그 열거형
 * AI 네비게이션 시스템에서 이동 가능한 영역과 위험도를 나타냄
 */
enum PolyFlags
{
    WALK = 0x01,//안전한곳
    Team1 = 0x02,//내가1팀이면 아군 스펠 경로
    Team2 = 0x04,//내가 1팀이면 적군 스펠 경로 
    //만약 내가 1팀이고 내 타일의 flags 를 보니 PolyFlags::Team2 와 DANGEROUS_Medium 두개 같이 타일의 flag 에 있다 그럼 위험한거
    DANGEROUS_Low = 0x08,//위험도 
    DANGEROUS_Medium = 0x10,//위험도
    Missile_pos = 0x20,//만약 내가 1팀이고 내 타일의 flags 를 보니 PolyFlags::Team2 와 Missile_pos 두개 같이 타일의 flag 에 있다 그럼 이미 충돌했거나 충돌직전 못피함
    Structure = 0x40,                   // 0100 0000 - 구조물/건물
    SAMPLE_POLYFLAGS_DISABLED = 0x100, //이동불가지역
    SAMPLE_POLYFLAGS_ALL = 0xFFFF //모든플래그
};

/**
 * PolyFlags 비트 OR 연산자 오버로드
 * 두 플래그를 결합할 때 사용
 */

inline PolyFlags operator|(PolyFlags a, PolyFlags b)
{
    return static_cast<PolyFlags>(
        static_cast<uint16_t>(a) | static_cast<uint16_t>(b)
        );
}

/**
 * PolyFlags 비트 AND 연산자 오버로드
 * 플래그와 uint16_t 값의 AND 연산
 */
inline PolyFlags operator&(PolyFlags a, uint16_t b)
{
    return static_cast<PolyFlags>(static_cast<uint16_t>(a) & b);
}

/**
 * PolyFlags 비트 AND 할당 연산자 오버로드
 * 플래그 값을 AND 연산 결과로 업데이트
 */
inline PolyFlags& operator&=(PolyFlags& a, uint16_t b)
{
    a = a & b;
    return a;
}

/**
 * DangerLevel 열거형을 PolyFlags로 변환하는 함수
 * @param danger 위험도 레벨
 * @return 해당하는 PolyFlags 값
 */
static PolyFlags FromDangerLevel(DangerLevel danger)
{
    switch (danger)
    {
    case DangerLevel::Low:       return DANGEROUS_Low;
    case DangerLevel::Medium:    return DANGEROUS_Medium;
  //  case DangerLevel::High:      return DANGEROUS_High;
   // case DangerLevel::VeryHigh:  return DANGEROUS_VeryHigh;
   // case DangerLevel::Extreme:   return DANGEROUS_Extreme;
    case DangerLevel::Never:     return WALK;
    default:
        return WALK;
    }
}