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
 * ������ �÷��� ������
 * AI �׺���̼� �ý��ۿ��� �̵� ������ ������ ���赵�� ��Ÿ��
 */
enum PolyFlags
{
    WALK = 0x01,//�����Ѱ�
    Team1 = 0x02,//����1���̸� �Ʊ� ���� ���
    Team2 = 0x04,//���� 1���̸� ���� ���� ��� 
    //���� ���� 1���̰� �� Ÿ���� flags �� ���� PolyFlags::Team2 �� DANGEROUS_Medium �ΰ� ���� Ÿ���� flag �� �ִ� �׷� �����Ѱ�
    DANGEROUS_Low = 0x08,//���赵 
    DANGEROUS_Medium = 0x10,//���赵
    Missile_pos = 0x20,//���� ���� 1���̰� �� Ÿ���� flags �� ���� PolyFlags::Team2 �� Missile_pos �ΰ� ���� Ÿ���� flag �� �ִ� �׷� �̹� �浹�߰ų� �浹���� ������
    Structure = 0x40,                   // 0100 0000 - ������/�ǹ�
    SAMPLE_POLYFLAGS_DISABLED = 0x100, //�̵��Ұ�����
    SAMPLE_POLYFLAGS_ALL = 0xFFFF //����÷���
};

/**
 * PolyFlags ��Ʈ OR ������ �����ε�
 * �� �÷��׸� ������ �� ���
 */

inline PolyFlags operator|(PolyFlags a, PolyFlags b)
{
    return static_cast<PolyFlags>(
        static_cast<uint16_t>(a) | static_cast<uint16_t>(b)
        );
}

/**
 * PolyFlags ��Ʈ AND ������ �����ε�
 * �÷��׿� uint16_t ���� AND ����
 */
inline PolyFlags operator&(PolyFlags a, uint16_t b)
{
    return static_cast<PolyFlags>(static_cast<uint16_t>(a) & b);
}

/**
 * PolyFlags ��Ʈ AND �Ҵ� ������ �����ε�
 * �÷��� ���� AND ���� ����� ������Ʈ
 */
inline PolyFlags& operator&=(PolyFlags& a, uint16_t b)
{
    a = a & b;
    return a;
}

/**
 * DangerLevel �������� PolyFlags�� ��ȯ�ϴ� �Լ�
 * @param danger ���赵 ����
 * @return �ش��ϴ� PolyFlags ��
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