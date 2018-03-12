#ifndef __K1887VE3T_H__
#define __K1887VE3T_H__

#define uint8		unsigned char
#define uint16	unsigned int
#define uint32	unsigned long
#define int8		char
#define int16		int

#define __IO 		volatile

sfr   CC0      = 0xFE80;
sfr   CC0IC    = 0xFF78;
sfr   CC1      = 0xFE82;
sfr   CC1IC    = 0xFF7A;
sfr   CC2      = 0xFE84;
sfr   CC2IC    = 0xFF7C;
sfr   CC3      = 0xFE86;
sfr   CC3IC    = 0xFF7E;
sfr   CC4      = 0xFE88;
sfr   CC4IC    = 0xFF80;
sfr   CC5      = 0xFE8A;
sfr   CC5IC    = 0xFF82;
sfr   CC6      = 0xFE8C;
sfr   CC6IC    = 0xFF84;
sfr   CC7      = 0xFE8E;
sfr   CC7IC    = 0xFF86;
sfr   CC8      = 0xFE90;
sfr   CC8IC    = 0xFF88;
sfr   CC9      = 0xFE92;
sfr   CC9IC    = 0xFF8A;
sfr   CC10     = 0xFE94;
sfr   CC10IC   = 0xFF8C;
sfr   CC11     = 0xFE96;
sfr   CC11IC   = 0xFF8E;
sfr   CC12     = 0xFE98;
sfr   CC12IC   = 0xFF90;
sfr   CC13     = 0xFE9A;
sfr   CC13IC   = 0xFF92;
sfr   CC14     = 0xFE9C;
sfr   CC14IC   = 0xFF94;
sfr   CC15     = 0xFE9E;
sfr   CC15IC   = 0xFF96;
sfr   CC16     = 0xFE60;
sfr   CC16IC   = 0xF160;
sfr   CC17     = 0xFE62;
sfr   CC17IC   = 0xF162;
sfr   CC18     = 0xFE64;
sfr   CC18IC   = 0xF164;
sfr   CC19     = 0xFE66;
sfr   CC19IC   = 0xF166;
sfr   CC20     = 0xFE68;
sfr   CC20IC   = 0xF168;
sfr   CC21     = 0xFE6A;
sfr   CC21IC   = 0xF16A;
sfr   CC22     = 0xFE6C;
sfr   CC22IC   = 0xF16C;
sfr   CC23     = 0xFE6E;
sfr   CC23IC   = 0xF16E;
sfr   CC24     = 0xFE70;
sfr   CC24IC   = 0xF170;
sfr   CC25     = 0xFE72;
sfr   CC25IC   = 0xF172;
sfr   CC26     = 0xFE74;
sfr   CC26IC   = 0xF174;
sfr   CC27     = 0xFE76;
sfr   CC27IC   = 0xF176;
sfr   CC28     = 0xFE78;
sfr   CC28IC   = 0xF178;
sfr   CC29     = 0xFE7A;
sfr   CC29IC   = 0xF184;
sfr   CC30     = 0xFE7C;
sfr   CC30IC   = 0xF18C;
sfr   CC31     = 0xFE7E;
sfr   CC31IC   = 0xF194;
sfr   CCM0     = 0xFF52;
sfr   CCM1     = 0xFF54;
sfr   CCM2     = 0xFF56;
sfr   CCM3     = 0xFF58;
sfr   CCM4     = 0xFF22;
sfr   CCM5     = 0xFF24;
sfr   CCM6     = 0xFF26;
sfr   CCM7     = 0xFF28;
sfr   T0       = 0xFE50;
sfr   T01CON   = 0xFF50;
sfr   T0IC     = 0xFF9C;
sfr   T0REL    = 0xFE54;
sfr   T1       = 0xFE52;
sfr   T1IC     = 0xFF9E;
sfr   T1REL    = 0xFE56;
sfr   T7       = 0xF050;
sfr   T78CON   = 0xFF20;
sfr   T7IC     = 0xF17A;
sfr   T7REL    = 0xF054;
sfr   T8       = 0xF052;
sfr   T8IC     = 0xF17C;
sfr   T8REL    = 0xF056;
sbit  CC0IR    = CC0IC^7;
sbit  CC0IE    = CC0IC^6;
sbit  CC1IR    = CC1IC^7;
sbit  CC1IE    = CC1IC^6;
sbit  CC2IR    = CC2IC^7;
sbit  CC2IE    = CC2IC^6;
sbit  CC3IR    = CC3IC^7;
sbit  CC3IE    = CC3IC^6;
sbit  CC4IR    = CC4IC^7;
sbit  CC4IE    = CC4IC^6;
sbit  CC5IR    = CC5IC^7;
sbit  CC5IE    = CC5IC^6;
sbit  CC6IR    = CC6IC^7;
sbit  CC6IE    = CC6IC^6;
sbit  CC7IR    = CC7IC^7;
sbit  CC7IE    = CC7IC^6;
sbit  CC8IR    = CC8IC^7;
sbit  CC8IE    = CC8IC^6;
sbit  CC9IR    = CC9IC^7;
sbit  CC9IE    = CC9IC^6;
sbit  CC10IR   = CC10IC^7;
sbit  CC10IE   = CC10IC^6;
sbit  CC11IR   = CC11IC^7;
sbit  CC11IE   = CC11IC^6;
sbit  CC12IR   = CC12IC^7;
sbit  CC12IE   = CC12IC^6;
sbit  CC13IR   = CC13IC^7;
sbit  CC13IE   = CC13IC^6;
sbit  CC14IR   = CC14IC^7;
sbit  CC14IE   = CC14IC^6;
sbit  CC15IR   = CC15IC^7;
sbit  CC15IE   = CC15IC^6;
sbit  CC16IR   = CC16IC^7;
sbit  CC16IE   = CC16IC^6;
sbit  CC17IR   = CC17IC^7;
sbit  CC17IE   = CC17IC^6;
sbit  CC18IR   = CC18IC^7;
sbit  CC18IE   = CC18IC^6;
sbit  CC19IR   = CC19IC^7;
sbit  CC19IE   = CC19IC^6;
sbit  CC20IR   = CC20IC^7;
sbit  CC20IE   = CC20IC^6;
sbit  CC21IR   = CC21IC^7;
sbit  CC21IE   = CC21IC^6;
sbit  CC22IR   = CC22IC^7;
sbit  CC22IE   = CC22IC^6;
sbit  CC23IR   = CC23IC^7;
sbit  CC23IE   = CC23IC^6;
sbit  CC24IR   = CC24IC^7;
sbit  CC24IE   = CC24IC^6;
sbit  CC25IR   = CC25IC^7;
sbit  CC25IE   = CC25IC^6;
sbit  CC26IR   = CC26IC^7;
sbit  CC26IE   = CC26IC^6;
sbit  CC27IR   = CC27IC^7;
sbit  CC27IE   = CC27IC^6;
sbit  CC28IR   = CC28IC^7;
sbit  CC28IE   = CC28IC^6;
sbit  CC29IR   = CC29IC^7;
sbit  CC29IE   = CC29IC^6;
sbit  CC30IR   = CC30IC^7;
sbit  CC30IE   = CC30IC^6;
sbit  CC31IR   = CC31IC^7;
sbit  CC31IE   = CC31IC^6;
sbit  ACC0     = CCM0^3;
sbit  ACC1     = CCM0^7;
sbit  ACC2     = CCM0^11;
sbit  ACC3     = CCM0^15;
sbit  ACC4     = CCM1^3;
sbit  ACC5     = CCM1^7;
sbit  ACC6     = CCM1^11;
sbit  ACC8     = CCM2^3;
sbit  ACC7     = CCM1^15;
sbit  ACC8     = CCM2^3;
sbit  ACC9     = CCM2^7;
sbit  ACC10    = CCM2^11;
sbit  ACC11    = CCM2^15;
sbit  ACC12    = CCM3^3;
sbit  ACC13    = CCM3^7;
sbit  ACC14    = CCM3^11;
sbit  ACC15    = CCM3^15;
sbit  ACC16    = CCM4^3;
sbit  ACC17    = CCM4^7;
sbit  ACC18    = CCM4^11;
sbit  ACC19    = CCM4^15;
sbit  ACC20    = CCM5^3;
sbit  ACC21    = CCM5^7;
sbit  ACC22    = CCM5^11;
sbit  ACC23    = CCM5^15;
sbit  ACC24    = CCM6^3;
sbit  ACC25    = CCM6^7;
sbit  ACC26    = CCM6^11;
sbit  ACC27    = CCM6^15;
sbit  ACC28    = CCM7^3;
sbit  ACC29    = CCM7^7;
sbit  ACC30    = CCM7^11;
sbit  ACC31    = CCM7^15;
sbit  T0IE     = T0IC^6;
sbit  T0IR     = T0IC^7;
sbit  T0M      = T01CON^3;
sbit  T0R      = T01CON^6;
sbit  T1IE     = T1IC^6;
sbit  T1IR     = T1IC^7;
sbit  T1M      = T01CON^11;
sbit  T1R      = T01CON^14;
sbit  T7IE     = T7IC^6;
sbit  T7IR     = T7IC^7;
sbit  T7M      = T78CON^3;
sbit  T7R      = T78CON^6;
sbit  T8IE     = T8IC^6;
sbit  T8IR     = T8IC^7;
sbit  T8M      = T78CON^11;
sbit  T8R      = T78CON^14;

//* CPU *
sfr   CSP      = 0xFE08;
sfr   MDC      = 0xFF0E;
sfr   ONES     = 0xFF1E;
sfr   PSW      = 0xFF10;
sfr   SP       = 0xFE12;
sfr   STKOV    = 0xFE14;
sfr   STKUN    = 0xFE16;
sfr   SYSCON   = 0xFF12;
sfr   TFR      = 0xFFAC;
sfr   ZEROS    = 0xFF1C;
sfr   BUSCON0  = 0xFF0C;
sfr   BUSCON1  = 0xFF14;
sfr   BUSCON2  = 0xFF16;
sfr   BUSCON3  = 0xFF18;
sfr   BUSCON4  = 0xFF1A;
sfr   ADDRSEL1 = 0xFE18;
sfr   ADDRSEL2 = 0xFE1A;
sfr   ADDRSEL3 = 0xFE1C;
sfr   ADDRSEL4 = 0xFE1E;
sbit  WRCFG    = SYSCON^7;
sbit  CLKEN    = SYSCON^8;
sbit  BYTDIS   = SYSCON^9;
sbit  ROMEN    = SYSCON^10;
sbit  SGTEN    = SYSCON^11;
sbit  ROMS1    = SYSCON^12;
sbit  MDRIU    = MDC^4;
sbit  N        = PSW^0;
sbit  C        = PSW^1;
sbit  V        = PSW^2;
sbit  Z        = PSW^3;
sbit  E        = PSW^4;
sbit  MULIP    = PSW^5;
sbit  USR0     = PSW^6;
sbit  HLDEN    = PSW^10;
sbit  IEN      = PSW^11;
sbit  RWDC0    = BUSCON0^4;
sbit  MTTC0    = BUSCON0^5;
sbit  ALECTL0  = BUSCON0^9;
sbit  BUSACT0  = BUSCON0^10;
sbit  RDYEN0   = BUSCON0^12;
sbit  RWDC1    = BUSCON1^4;
sbit  MTTC1    = BUSCON1^5;
sbit  ALECTL1  = BUSCON1^9;
sbit  BUSACT1  = BUSCON1^10;
sbit  RDYEN1   = BUSCON1^12;
sbit  CSREN1   = BUSCON1^14;
sbit  CSWEN1   = BUSCON1^15;
sbit  RWDC2    = BUSCON2^4;
sbit  MTTC2    = BUSCON2^5;
sbit  ALECTL2  = BUSCON2^9;
sbit  BUSACT2  = BUSCON2^10;
sbit  RDYEN2   = BUSCON2^12;
sbit  CSREN2   = BUSCON2^14;
sbit  CSWEN2   = BUSCON2^15;
sbit  RWDC3    = BUSCON3^4;
sbit  MTTC3    = BUSCON3^5;
sbit  ALECTL3  = BUSCON3^9;
sbit  BUSACT3  = BUSCON3^10;
sbit  RDYEN3   = BUSCON3^12;
sbit  CSREN3   = BUSCON3^14;
sbit  CSWEN3   = BUSCON3^15;
sbit  RWDC4    = BUSCON4^4;
sbit  MTTC4    = BUSCON4^5;
sbit  ALECTL4  = BUSCON4^9;
sbit  BUSACT4  = BUSCON4^10;
sbit  RDYEN4   = BUSCON4^12;
sbit  CSREN4   = BUSCON4^14;
sbit  CSWEN4   = BUSCON4^15;
sbit  ILLBUS   = TFR^0;
sbit  ILLINA   = TFR^1;
sbit  ILLOPA   = TFR^2;
sbit  PRTFLT   = TFR^3;
sbit  UNDOPC   = TFR^7;
sbit  STKUF    = TFR^13;
sbit  STKOF    = TFR^14;
sbit  NMI      = TFR^15;
sfr DIP =0xF0F8;
sfr DIPX =0xF0FA;

sfr EXICON =0xF1C0;

//* PEC *
sfr   PECC0    = 0xFEC0;
sfr   PECC1    = 0xFEC2;
sfr   PECC2    = 0xFEC4;
sfr   PECC3    = 0xFEC6;
sfr   PECC4    = 0xFEC8;
sfr   PECC5    = 0xFECA;
sfr   PECC6    = 0xFECC;
sfr   PECC7    = 0xFECE;

#define  SRCP0 (*((unsigned int volatile sdata *) 0xFCE0))
#define  DSTP0 (*((unsigned int volatile sdata *) 0xFCE2))
#define  SRCP1 (*((unsigned int volatile sdata *) 0xFCE4))
#define  DSTP1 (*((unsigned int volatile sdata *) 0xFCE6))
#define  SRCP2 (*((unsigned int volatile sdata *) 0xFCE8))
#define  DSTP2 (*((unsigned int volatile sdata *) 0xFCEA))
#define  SRCP3 (*((unsigned int volatile sdata *) 0xFCEC))
#define  DSTP3 (*((unsigned int volatile sdata *) 0xFCEE))
#define  SRCP4 (*((unsigned int volatile sdata *) 0xFCF0))
#define  DSTP4 (*((unsigned int volatile sdata *) 0xFCF2))
#define  SRCP5 (*((unsigned int volatile sdata *) 0xFCF4))
#define  DSTP5 (*((unsigned int volatile sdata *) 0xFCF6))
#define  SRCP6 (*((unsigned int volatile sdata *) 0xFCF8))
#define  DSTP6 (*((unsigned int volatile sdata *) 0xFCFA))
#define  SRCP7 (*((unsigned int volatile sdata *) 0xFCFC))
#define  DSTP7 (*((unsigned int volatile sdata *) 0xFCFE))

//* I/O Ports *
sfr   DP0L     = 0xF100;
sfr   DP0H     = 0xF102;
sfr   DP1L     = 0xF104;
sfr   DP1H     = 0xF106;
sfr   DP2      = 0xFFC2;
sfr   DP3      = 0xFFC6;
sfr   DP4      = 0xFFCA;
sfr   DP6      = 0xFFCE;
sfr   DP7      = 0xFFD2;
sfr   DP9      = 0xFFD6;
sfr   P0L      = 0xFF00;
sfr   P0H      = 0xFF02;
sfr   P1L      = 0xFF04;
sfr   P1H      = 0xFF06;
sfr   P2       = 0xFFC0;
sfr   P3       = 0xFFC4;
sfr   P4       = 0xFFC8;
sfr   P5       = 0xFFA2;
sfr   P6       = 0xFFCC;
sfr   P7       = 0xFFD0;
sfr   P9       = 0xFFD4;

sfr   ODP2     = 0xF1C2;
sfr   ODP3     = 0xF1C6;

sfr   ODP6     = 0xF1CE;
sfr   ODP7     = 0xF1D2;
sfr   ODP9     = 0xF1D6;
sfr   RP0H     = 0xF108;

//* Serial Interface *
sfr   S0BG     = 0xFEB4;
sfr   S0CON    = 0xFFB0;
sfr   S0EIC    = 0xFF70;
sfr   S0RBUF   = 0xFEB2;
sfr   S0RIC    = 0xFF6E;
sfr   S0TBUF   = 0xFEB0;
sfr   S0TBIC   = 0xF19C;
sfr   S0TIC    = 0xFF6C;
sbit  S0STP    = S0CON^3;
sbit  S0REN    = S0CON^4;
sbit  S0PEN    = S0CON^5;
sbit  S0FEN    = S0CON^6;
sbit  S0OEN    = S0CON^7;
sbit  S0PE     = S0CON^8;
sbit  S0FE     = S0CON^9;
sbit  S0OE     = S0CON^10;
sbit  S0ODD    = S0CON^12;
sbit  S0BRS    = S0CON^13;
sbit  S0LB     = S0CON^14;
sbit  S0R      = S0CON^15;
sbit  S0TIE    = S0TIC^6;
sbit  S0TIR    = S0TIC^7;
sbit  S0TBIE   = S0TBIC^6;
sbit  S0TBIR   = S0TBIC^7;
sbit  S0RIE    = S0RIC^6;
sbit  S0RIR    = S0RIC^7;
sbit  S0EIE    = S0EIC^6;
sbit  S0EIR    = S0EIC^7;

sfr   SSC0TIC   = 0xFF72;
sfr   SSC0RIC   = 0xFF74;
sfr   SSC0EIC   = 0xFF76;
sfr   SSC0TB    = 0xF0B0;
sfr   SSC0RB    = 0xF0B2;
sfr   SSC0BR    = 0xF0B4;
sfr   SSC0CON   = 0xFFB2;
sbit  SSC0HB    = SSC0CON^4;
sbit  SSC0PH    = SSC0CON^5;
sbit  SSC0P0    = SSC0CON^6;
sbit  SSC0TE    = SSC0CON^8;
sbit  SSC0TEN   = SSC0CON^8;
sbit  SSC0RE    = SSC0CON^9;
sbit  SSC0REN   = SSC0CON^9;
sbit  SSC0PE    = SSC0CON^10;
sbit  SSC0PEN   = SSC0CON^10;
sbit  SSC0BE    = SSC0CON^11;
sbit  SSC0BEN   = SSC0CON^11;
sbit  SSC0BSY   = SSC0CON^12;
sbit  SSC0MS    = SSC0CON^14;
sbit  SSC0EN    = SSC0CON^15;
sbit  SSC0TIR   = SSC0TIC^7;
sbit  SSC0TIE   = SSC0TIC^6;
sbit  SSC0RIR   = SSC0RIC^7;
sbit  SSC0RIE   = SSC0RIC^6;
sbit  SSC0EIR   = SSC0EIC^7;
sbit  SSC0EIE   = SSC0EIC^6;

//* GPT1, GPT2 *
sfr   CAPREL   = 0xFE4A;
sfr   CRIC     = 0xFF6A;
sfr   T2       = 0xFE40;
sfr   T2CON    = 0xFF40;
sfr   T2IC     = 0xFF60;
sfr   T3       = 0xFE42;
sfr   T3CON    = 0xFF42;
sfr   T3IC     = 0xFF62;
sfr   T4       = 0xFE44;
sfr   T4CON    = 0xFF44;
sfr   T4IC     = 0xFF64;
sfr   T5       = 0xFE46;
sfr   T5CON    = 0xFF46;
sfr   T5IC     = 0xFF66;
sfr   T6       = 0xFE48;
sfr   T6CON    = 0xFF48;
sfr   T6IC     = 0xFF68;
sfr   WDT      = 0xFEAE;
sfr   WDTCON   = 0xFFAE;
sbit  CRIE     = CRIC^6;
sbit  CRIR     = CRIC^7;
sbit  T2R      = T2CON^6;
sbit  T2UD     = T2CON^7;
sbit  T2UDE    = T2CON^8;
sbit  T2IE     = T2IC^6;
sbit  T2IR     = T2IC^7;
sbit  T3R      = T3CON^6;
sbit  T3UD     = T3CON^7;
sbit  T3UDE    = T3CON^8;
sbit  T3OE     = T3CON^9;
sbit  T3OTL    = T3CON^10;
sbit  T3IE     = T3IC^6;
sbit  T3IR     = T3IC^7;
sbit  T4R      = T4CON^6;
sbit  T4UD     = T4CON^7;
sbit  T4UDE    = T4CON^8;
sbit  T4IE     = T4IC^6;
sbit  T4IR     = T4IC^7;
sbit  T5R      = T5CON^6;
sbit  T5UD     = T5CON^7;
sbit  T5UDE    = T5CON^8;
sbit  T5CLR    = T5CON^14;
sbit  T5SC     = T5CON^15;
sbit  T5IE     = T5IC^6;
sbit  T5IR     = T5IC^7;
sbit  T6R      = T6CON^6;
sbit  T6UD     = T6CON^7;
sbit  T6UDE    = T6CON^8;
sbit  T6OE     = T6CON^9;
sbit  T6OTL    = T6CON^10;
sbit  T6SR     = T6CON^15;
sbit  T6IE     = T6IC^6;
sbit  T6IR     = T6IC^7;
sbit  T0IN     = P3^0;
sbit  T2IN     = P3^7;
sbit  T3IN     = P3^6;
sbit  T4IN     = P3^5;
sbit  T5IN     = P5^13;
sbit  T6IN     = P5^12;
sbit  T2EUD    = P5^15;
sbit  T3EUD    = P3^4;
sbit  T4EUD    = P5^14;
sbit  T5EUD    = P5^11;
sbit  T3EUD    = P3^4;
sbit  T3OUT    = P3^3;
sbit  CAPIN    = P3^2;
sbit  T6OUT    = P3^1;
sbit  WDTIN    = WDTCON^0;
sbit  WDTR     = WDTCON^1;
sbit 	SWR			 = WDTCON^2;
sbit	WDTPRE	 = WDTCON^7;

//Vectors
#define CC0IC_VEC			0x10
#define CC1IC_VEC			0x11
#define CC2IC_VEC			0x12
#define CC3IC_VEC			0x13
#define CC4IC_VEC			0x14
#define CC5IC_VEC			0x15
#define CC6IC_VEC			0x16
#define CC7IC_VEC			0x17
#define CC8IC_VEC			0x18
#define CC9IC_VEC			0x19
#define CC10IC_VEC		0x1A
#define CC11IC_VEC		0x1B
#define CC12IC_VEC		0x1C
#define CC13IC_VEC		0x1D
#define CC14IC_VEC		0x1E
#define CC15IC_VEC		0x1F

#define T0IC_VEC		0x20
#define T1IC_VEC		0x21
#define T2IC_VEC		0x22
#define T3IC_VEC		0x23
#define T4IC_VEC		0x24
#define T5IC_VEC		0x25
#define T6IC_VEC		0x26
#define T7IC_VEC		0x3D
#define T8IC_VEC		0x3E


#define CAN0IC_VEC	0x54
#define CAN1IC_VEC	0x55
#define CAN2IC_VEC	0x56
#define CAN3IC_VEC	0x57
#define CAN4IC_VEC	0x59
#define CAN5IC_VEC	0x5A
#define CAN6IC_VEC	0x5B
#define CAN7IC_VEC	0x5C
#define CAN8IC_VEC	0x60
#define CAN9IC_VEC	0x61
#define CAN10IC_VEC	0x62
#define CAN11IC_VEC	0x63
#define CAN12IC_VEC	0x64
#define CAN13IC_VEC	0x65
#define CAN14IC_VEC	0x66
#define CAN15IC_VEC	0x67


#define CRIC_VEC				0x27
#define ASC_CIC_VEC			0x28
#define ADC_EIC_VEC			0x29
#define S0TIC_VEC				0x2A
#define S0RIC_VEC				0x2B
#define S0EIC_VEC				0x2C
#define SSC0TIC_VEC			0x2D
#define SSC0RIC_VEC			0x2E
#define SSC0EIC_VEC			0x2F

#define CC16IC_VEC			0x30
#define CC17IC_VEC			0x31
#define CC18IC_VEC			0x32
#define CC19IC_VEC			0x33
#define CC20IC_VEC			0x34
#define CC21IC_VEC			0x35
#define CC22IC_VEC			0x36
#define CC23IC_VEC			0x37
#define CC24IC_VEC			0x38
#define CC25IC_VEC			0x39
#define CC26IC_VEC			0x3A
#define CC27IC_VEC			0x3B
#define CC28IC_VEC			0x3C
#define CC29IC_VEC			0x44
#define CC30IC_VEC			0x45
#define CC31IC_VEC			0x46

//PLLCON
sfr PLLCON = 0xF1D0;

//******----- Reserve Registr's----*******

sfr	ODP4   = 0xF1CA;
#define  ALTSEL0P1L (*((unsigned int volatile sdata *) 0xF1EA))
#define  ALTSEL0P1H (*((unsigned int volatile sdata *) 0xF1EC))
#define  ALTSEL0P2 (*((unsigned int volatile sdata *) 0xF1EE))
#define  ALTSEL0P3 (*((unsigned int volatile sdata *) 0xF1F0))
#define  ALTSEL1P3 (*((unsigned int volatile sdata *) 0xF1F2))
#define  ALTSEL0P4 (*((unsigned int volatile sdata *) 0xF1F4))
#define  ALTSEL0P6 (*((unsigned int volatile sdata *) 0xF1F6))
#define  ALTSEL0P7 (*((unsigned int volatile sdata *) 0xF1F8))
#define  ALTSEL1P7 (*((unsigned int volatile sdata *) 0xF1FA))
#define  ALTSEL0P9 (*((unsigned int volatile sdata *) 0xF1FC))
#define  ALTSEL1P9 (*((unsigned int volatile sdata *) 0xF1FE))
/*
sfr   ALTSEL0P1L     = 0xF1EA; 		 
sfr   ALTSEL0P1H     = 0xF1EC;
sfr   ALTSEL0P2      = 0xF1EE;
sfr   ALTSEL0P3      = 0xF1F0;
sfr   ALTSEL1P3      = 0xF1F2;
sfr   ALTSEL0P4      = 0xF1F4;
sfr   ALTSEL0P6      = 0xF1F6;
sfr   ALTSEL0P7      = 0xF1F8;
sfr   ALTSEL1P7      = 0xF1FA;
sfr   ALTSEL0P8      = 0xF1FC;
sfr   ALTSEL1P8      = 0xF1FE;
*/
sfr   EXISEL0        = 0xF1DA;
sfr   EXISEL1        = 0xF1D8;

//******---Port control reg---********

sfr   PICON          = 0xF1C4;
sfr   POCON0L        = 0xF080;
sfr   POCON0H        = 0xF082;
sfr   POCON1L        = 0xF084;
sfr   POCON1H        = 0xF086;
sfr   POCON2         = 0xF088;
sfr   POCON3         = 0xF08A;
sfr   POCON4         = 0xF08C;
sfr   POCON6         = 0xF08E;
sfr   POCON7         = 0xF090;
sfr   POCON8         = 0xF092;

//***********---I2C Reg---*********

sfr   I2C_SMBSDA     = 0xF0DA;
sfr   I2C_SMBST      = 0xF0DC;
sfr   I2C_SMBCST     = 0xF0DE;
sfr   I2C_SMBCTL1    = 0xF0E0;
sfr   I2C_SMBADDR    = 0xF0E2;
sfr   I2C_SMBCTL2    = 0xF0E4;
sfr   I2C_SMBTOPR    = 0xF0E6;
sfr   I2C_SMBCTL3    = 0xF0E8;
sfr   I2C_CFG        = 0xF0EA;
sfr   I2C_DIC	     = 0xF186;

//*************ASC0 & ASC1 & SSC1**********

sfr   S1BG     = 0xFEAA;
sfr   S1CON    = 0xF1B8;
sfr   S1EIC    = 0xF192;
sfr   S1RBUF   = 0xFEA8;
sfr   S1RIC    = 0xF18A;
sfr   S1TBUF   = 0xFEA6;
sfr   S1TBIC   = 0xF13C;
sfr   S1TIC    = 0xF182;
sfr   S0FDV    = 0xFEB6; 
sfr   S1FDV    = 0xFEAC;
sbit  S1STP    = S1CON^3;
sbit  S1REN    = S1CON^4;
sbit  S1PEN    = S1CON^5;
sbit  S1FEN    = S1CON^6;
sbit  S1OEN    = S1CON^7;
sbit  S1PE     = S1CON^8;
sbit  S1FE     = S1CON^9;
sbit  S1OE     = S1CON^10;
sbit  S1ODD    = S1CON^12;
sbit  S1BRS    = S1CON^13;
sbit  S1LB     = S1CON^14;
sbit  S1R      = S1CON^15;
sbit  S1TIE    = S1TIC^6;
sbit  S1TIR    = S1TIC^7;
sbit  S1TBIE   = S1TBIC^6;
sbit  S1TBIR   = S1TBIC^7;
sbit  S1RIE    = S1RIC^6;
sbit  S1RIR    = S1RIC^7;
sbit  S1EIE    = S1EIC^6;
sbit  S1EIR    = S1EIC^7;

sfr   SSC1TIC   = 0xF122;
sfr   SSC1RIC   = 0xF124;
sfr   SSC1EIC   = 0xF126;
sfr   SSC1TB    = 0xF05A;
sfr   SSC1RB    = 0xF05C;
sfr   SSC1BR    = 0xF05E;
sfr   SSC1CON   = 0xFF5E;
sbit  SSC1HB    = SSC1CON^4;
sbit  SSC1PH    = SSC1CON^5;
sbit  SSC1P0    = SSC1CON^6;
sbit  SSC1TE    = SSC1CON^8;
sbit  SSC1TEN   = SSC1CON^8;
sbit  SSC1RE    = SSC1CON^9;
sbit  SSC1REN   = SSC1CON^9;
sbit  SSC1PE    = SSC1CON^10;
sbit  SSC1PEN   = SSC1CON^10;
sbit  SSC1BE    = SSC1CON^11;
sbit  SSC1BEN   = SSC1CON^11;
sbit  SSC1BSY   = SSC1CON^12;
sbit  SSC1MS    = SSC1CON^14;
sbit  SSC1EN    = SSC1CON^15;
sbit  SSC1TIR   = SSC1TIC^7;
sbit  SSC1TIE   = SSC1TIC^6;
sbit  SSC1RIR   = SSC1RIC^7;
sbit  SSC1RIE   = SSC1RIC^6;
sbit  SSC1EIR   = SSC1EIC^7;
sbit  SSC1EIE   = SSC1EIC^6;

//*******--- CAPCOM  Registr's----*********
  
sfr   CC1_SEE   = 0xFE2E;   
sfr   CC2_SEE   = 0xFE2A;  

sfr   CC1_SEM   = 0xFE2C;      
sfr   CC2_SEM   = 0xFE28;	 

sfr   CC1_DRM   = 0xFF5A;	 
sfr   CC2_DRM   = 0xFF2A;      

sfr   CC1_OUT   = 0xFF5C;	 
sfr   CC2_OUT   = 0xFF2C;	 



sfr   CC1_IOC   = 0xF062; 		 
sfr   CC2_IOC   = 0xF066; 

//*******--- CAPCOM6  Registr's----*********
#define  CCU6_T12 (*((unsigned int volatile sdata *) 0xE890))
#define  CCU6_T12PR (*((unsigned int volatile sdata *) 0xE892))
#define  CCU6_T12DTC (*((unsigned int volatile sdata *) 0xE894))
#define  CCU6_CC60R (*((unsigned int volatile sdata *) 0xE898))
#define  CCU6_CC60SR (*((unsigned int volatile sdata *) 0xE8A0))
#define  CCU6_CC61R (*((unsigned int volatile sdata *) 0xE89A))
#define  CCU6_CC61SR (*((unsigned int volatile sdata *) 0xE8A2))
#define  CCU6_CC62R (*((unsigned int volatile sdata *) 0xE89C))
#define  CCU6_CC62SR (*((unsigned int volatile sdata *) 0xE8A4))
#define  CCU6_T13 (*((unsigned int volatile sdata *) 0xE8B0))
#define  CCU6_T13PR (*((unsigned int volatile sdata *) 0xE8B2))
#define  CCU6_CC63R (*((unsigned int volatile sdata *) 0xE8B4))
#define  CCU6_CC63SR (*((unsigned int volatile sdata *) 0xE8B6))
#define  CCU6_CMPSTAT (*((unsigned int volatile sdata *) 0xE8A8))
#define  CCU6_CMPMODIF (*((unsigned int volatile sdata *) 0xE8AA))
#define  CCU6_TCTR0 (*((unsigned int volatile sdata *) 0xE8AC))
#define  CCU6_TCTR2 (*((unsigned int volatile sdata *) 0xE8AE))
#define  CCU6_TCTR4 (*((unsigned int volatile sdata *) 0xE8A6))
#define  CCU6_MODCTR (*((unsigned int volatile sdata *) 0xE8C0))
#define  CCU6_TRPCTR (*((unsigned int volatile sdata *) 0xE8C2))
#define  CCU6_PSLR (*((unsigned int volatile sdata *) 0xE8C4))
#define  CCU6_MCMOUTS (*((unsigned int volatile sdata *) 0xE8CA))
#define  CCU6_MCMOUT (*((unsigned int volatile sdata *) 0xE8CC))
#define  CCU6_MCMCTR (*((unsigned int volatile sdata *) 0xE8CE))
#define  CCU6_T12MSEL (*((unsigned int volatile sdata *) 0xE8C6))
#define  CCU6_IS (*((unsigned int volatile sdata *) 0xE8D0))
#define  CCU6_ISS (*((unsigned int volatile sdata *) 0xE8D2))
#define  CCU6_ISR (*((unsigned int volatile sdata *) 0xE8D4))
#define  CCU6_INP (*((unsigned int volatile sdata *) 0xE8D6))
#define  CCU6_IEN (*((unsigned int volatile sdata *) 0xE8D8))
/*
sfr   CCU6_T12        	= 0xE890;
sfr   CCU6_T12PR	= 0xE892;
sfr   CCU6_T12DTC     	= 0xE894;
sfr   CCU6_CC60R      	= 0xE898;
sfr   CCU6_CC60SR     	= 0xE8A0;
sfr   CCU6_CC61R	= 0xE89A;
sfr   CCU6_CC61SR     	= 0xE8A2;
sfr   CCU6_CC62R	= 0xE89C;
sfr   CCU6_CC62SR	= 0xE8A4;
sfr   CCU6_T13		= 0xE8B0;
sfr   CCU6_T13PR	= 0xE8B2;
sfr   CCU6_CC63R      	= 0xE8B4;
sfr   CCU6_CC63SR     	= 0xE8B6;
sfr   CCU6_CMPSTAT    	= 0xE8A8;
sfr   CCU6_CMPMODIF	= 0xE8AA;
sfr   CCU6_TCTR0	= 0xE8AC;
sfr   CCU6_TCTR2	= 0xE8AE;
sfr   CCU6_TCTR4	= 0xE8A6;
sfr   CCU6_MODCTR	= 0xE8C0;
sfr   CCU6_TRPCTR	= 0xE8C2;
sfr   CCU6_PSLR		= 0xE8C4;
sfr   CCU6_MCMOUTS    	= 0xE8CA;
sfr   CCU6_MCMOUT	= 0xE8CC;
sfr   CCU6_MCMCTR	= 0xE8CE;
sfr   CCU6_T12MSEL    	= 0xE8C6;

sfr   CCU6_IS		= 0xE8D0;
sfr   CCU6_ISS		= 0xE8D2;
sfr   CCU6_ISR		= 0xE8D4;
sfr   CCU6_INP		= 0xE8D6;
sfr   CCU6_IEN		= 0xE8D8;
*/
sfr   CCU6_T12IC	= 0xF190;
sfr   CCU6_T13IC	= 0xF198;
sfr   CCU6_EIC		= 0xF188;
sfr   CCU6_IC		= 0xF140;

sfr   XPERCON        	= 0xF024;

sfr   XADRS1 	        = 0xF014;
sfr   XADRS2		= 0xF016;
sfr   XADRS3		= 0xF018;
sfr   XADRS4		= 0xF01A;
sfr   XADRS5		= 0xF01C;
sfr   XADRS6		= 0xF01E;
		       
sfr   XBCON1		= 0xF114;
sfr   XBCON2		= 0xF116;
sfr   XBCON3		= 0xF118;
sfr   XBCON4		= 0xF11A;
sfr   XBCON5		= 0xF11C;
sfr   XBCON6		= 0xF11E;


//***********---RTC Reg---*********
sfr   RTCCST        	= 0xF030;
sfr   RTUDST        	= 0xF032;
sfr   RTCEIST        	= 0xF034;
sfr   RTCIEN        	= 0xF036;
sfr   RTPRD		= 0xF038;	    
sfr   RTCRD_hi		= 0xF03A;
sfr   RTCRD_lo		= 0xF03C;
sfr   RTCLD_hi		= 0xF03E;	    
sfr   RTCLD_lo		= 0xF040;
sfr   RTCCMP1_hi	= 0xF042;
sfr   RTCCMP1_lo	= 0xF044;
sfr   RTCCMP2_hi	= 0xF046;
sfr   RTCCMP2_lo	= 0xF048;
sfr   RTCCMP3_hi	= 0xF04A;
sfr   RTCCMP3_lo	= 0xF04C;
			    															

//rtc irq_reg

sfr   RTC_IC		= 0xF13A;
sfr   RTC_IC1		= 0xF150;
sfr   RTC_IC2		= 0xF152;
sfr   RTC_IC3		= 0xF154;

//adc_reg

sfr   ADC_CON         = 0xFFA0;
sfr   ADC_CON1        = 0xFFA6; 
sfr   ADC2_CON        = 0xF1A0;
sfr   ADC2_CON1       = 0xF1A6;
sfr   ADC_DAT         = 0xFEA0;
sfr   ADC_DAT2        = 0xF0A0;
sfr   ADC2_DAT        = 0xFEA2;
sfr   ADC2_DAT2       = 0xF0A2;
sfr   ADC_SWITCH      = 0xFEA4;

sfr   ADC_CIC         = 0xFF98;
sfr   ADC_EIC         = 0xFF9A;
sfr   ADC2_CIC        = 0xF18E;
sfr   ADC2_EIC        = 0xF196;

//---------------- CPUID -------------

sfr   CPUID		= 0xF00C;

sfr   MDH		= 0xFE0C;
sfr   MDL		= 0xFE0E;
sfr   MDC		= 0xFF0E;

//---------------MAC------------------

sfr  MAL   =0xFE5C;
sfr  MAH   =0xFE5E;
sfr  QR0   =0xF004;
sfr  QX0   =0xF000;
sfr  QR1   =0xF006;
sfr  QX1   =0xF002;
sfr  MSW   =0xFFDE;
sfr  MRW   =0xFFDA;
sfr  IDX0   =0xFF08;
sfr  IDX1   =0xFF0A;
sfr  MCW   =0xFFDC;

#endif










