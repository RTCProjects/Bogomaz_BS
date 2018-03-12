#ifndef _MAIN_H
#define _MAIN_H

#include <K1887VE3T.h>
#include <Intrins.h>
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>  
#include <math.h>

#define DEVICE_NUMBER	0

extern const uint32	SerialNumberBD;

//режим рассчёта круговой диаграммы
typedef enum
{
	SPEC_MODE = 0x01,	//рассчёт по данным спектрометра
	CNTR_MODE = 0x02	//рассчёт по данным 'гранаты'
}eAngDiagrammMode;
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
#define CLEAR_BIT(REG, BIT) REG &= (~(1<<BIT))
#define SET_BIT(REG, BIT)	REG |= (1<<BIT)



sfr CAN_IC0  = 0xF128;
sfr CAN_IC1  = 0xF12A;
sfr CAN_IC2  = 0xF12C;
sfr CAN_IC3  = 0xF12E;
sfr CAN_IC4  = 0xF132;
sfr CAN_IC5  = 0xF134;
sfr CAN_IC6  = 0xF136;
sfr CAN_IC7  = 0xF138;
sfr CAN_IC8  = 0xF140;
sfr CAN_IC9  = 0xF142;
sfr CAN_IC10 = 0xF144;
sfr CAN_IC11 = 0xF146;
sfr CAN_IC12 = 0xF148;
sfr CAN_IC13 = 0xF14A;
sfr CAN_IC14 = 0xF14C;
sfr CAN_IC15 = 0xF14E;

#define NCR0 (*((unsigned long volatile sdata *) 0xEA00))														   
#define NCR1 (*((unsigned long volatile sdata *) 0xEB00))
#define NSR0 (*((unsigned long volatile sdata *) 0xEA04))
#define NSR1 (*((unsigned long volatile sdata *) 0xEB04))
#define NIPR0 (*((unsigned long volatile sdata *) 0xEA08))
#define NIPR1 (*((unsigned long volatile sdata *) 0xEB08))
#define NPCR0 (*((unsigned long volatile sdata *) 0xEA0C))
#define NPCR1 (*((unsigned long volatile sdata *) 0xEB0C))
#define NBTR0 (*((unsigned long volatile sdata *) 0xEA10))
#define NBTR1 (*((unsigned long volatile sdata *) 0xEB10))
#define NECNT0   (*((unsigned long volatile sdata *) 0xEA14))
#define NECNT0_H (*((unsigned int volatile sdata *) 0xEA16))
#define NECNT1   (*((unsigned long volatile sdata *) 0xEB14))
#define NECNT1_H (*((unsigned int volatile sdata *) 0xEB16))
#define NFCR0   (*((unsigned long volatile sdata *) 0xEA18))
#define NFCR0_H (*((unsigned int volatile sdata *) 0xEA1A))
#define NFCR1   (*((unsigned long volatile sdata *) 0xEB18))
#define NFCR1_H (*((unsigned int volatile sdata *) 0xEB1A))
#define CLC   (*((unsigned long volatile sdata *) 0xE800))
#define ID   (*((unsigned long volatile sdata *) 0xE808))
#define ID_H (*((unsigned int volatile sdata *) 0xE80A))
#define FDR   (*((unsigned long volatile sdata *) 0xE80C))
#define FDR_H (*((unsigned int volatile sdata *) 0xE80E))
#define PANCTR   (*((unsigned long volatile sdata *) 0xE9C4))
#define PANCTR_H (*((unsigned int volatile sdata *) 0xE9C6))
#define MCR   (*((unsigned long volatile sdata *) 0xE9C8))
#define MITR   (*((unsigned long volatile sdata *) 0xE9CC))
#define MSPND0   (*((unsigned long volatile sdata *) 0xE940))
#define MSPND0_H (*((unsigned int volatile sdata *) 0xE942))
#define MSPND1   (*((unsigned long volatile sdata *) 0xE944))
#define MSPND1_H (*((unsigned int volatile sdata *) 0xE946))
#define MSID0   (*((unsigned long volatile sdata *) 0xE980))
#define MSID1   (*((unsigned long volatile sdata *) 0xE984))
#define MSIMASK   (*((unsigned long volatile sdata *) 0xE9C0))
#define MSIMASK_H (*((unsigned long volatile sdata *) 0xE9C2))
#define LIST0   (*((unsigned long volatile sdata *) 0xE900))
#define LIST0_H  (*((unsigned int volatile sdata *) 0xE902))
#define LIST1   (*((unsigned long volatile sdata *) 0xE904))
#define LIST1_H  (*((unsigned int volatile sdata *) 0xE906))
#define LIST2   (*((unsigned long volatile sdata *) 0xE908))
#define LIST2_H  (*((unsigned int volatile sdata *) 0xE90A))
#define LIST3   (*((unsigned long volatile sdata *) 0xE90C))
#define LIST3_H  (*((unsigned int volatile sdata *) 0xE90E))
#define LIST4   (*((unsigned long volatile sdata *) 0xE910))
#define LIST4_H  (*((unsigned int volatile sdata *) 0xE912))
#define LIST5   (*((unsigned long volatile sdata *) 0xE914))
#define LIST5_H  (*((unsigned int volatile sdata *) 0xE916))
#define LIST6   (*((unsigned long volatile sdata *) 0xE918))
#define LIST6_H  (*((unsigned int volatile sdata *) 0xE91A))
#define LIST7   (*((unsigned long volatile sdata *) 0xE91C))
#define LIST7_H  (*((unsigned int volatile sdata *) 0xE91E))


struct stCanMessage_32 {
  unsigned long MOFCR;
  unsigned long MOFGPR;
  unsigned long MOIPR;
  unsigned long MOAMR;
  unsigned long MODATAL;
  unsigned long MODATAH;
  unsigned long MOAR;
  unsigned long MOCTR;
};

#define CAN_Message_32 ((struct stCanMessage_32 volatile near *) 0x00EC00)

struct stCanMessage_16 {
  unsigned int MOFCRL;		
  unsigned int MOFCRH;		
  unsigned int MOFGPRL;
  unsigned int MOFGPRH;
  unsigned int MOIPRL;		
  unsigned int MOIPRH;
  unsigned int MOAMRL;
  unsigned int MOAMRH;		
  unsigned int MODATALL;	
  unsigned int MODATALH;	
  unsigned int MODATAHL;	
  unsigned int MODATAHH;	
  unsigned int MOARL;
  unsigned int MOARH;			
  unsigned int MOCTRL;		
  unsigned int MOCTRH;		
};

#define CAN_Message_16 ((struct stCanMessage_16 volatile near *) 0x00EC00)


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void delay(unsigned long z);

extern uint8	reProgBit;

#endif
