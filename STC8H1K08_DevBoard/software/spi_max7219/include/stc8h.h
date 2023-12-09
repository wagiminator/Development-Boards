// ===================================================================================
// Header File for STC8H Microcontrollers                                     * v1.0 *
// ===================================================================================

#pragma once
#include <stdint.h>

#define __BIT   __bit
#define __DATA  __data
#define __IDATA __idata
#define __PDATA __pdata
#define __XDATA __xdata
#define __CODE  __code
#define __REENTRANT  __reentrant

#define SBIT(name, addr, bit)         __sbit  __at(addr+bit) name
#define SFR(name, addr)               __sfr   __at(addr) name
#define SFRX(addr)                    (*(unsigned char volatile __xdata *)(addr))
#define SFR16X(addr)                  (*(unsigned int  volatile __xdata *)(addr))

#define SFRX_ON()                     P_SW2 |= 0x80
#define SFRX_OFF()                    P_SW2 &= 0x7f
#define SFRX_SET(__SFR__, __POS__)    {SFRX_ON();(__SFR__)|= (0x01<<(__POS__));SFRX_OFF();}
#define SFRX_RESET(__SFR__, __POS__)  {SFRX_ON();(__SFR__)&=~(0x01<<(__POS__));SFRX_OFF();}
#define SFRX_WRITE(__SFR__, __VAL__)  {SFRX_ON();(__SFR__)=(__POS__);SFRX_OFF();}

#define NOP()                         __asm NOP __endasm

#define INTERRUPT(name, vector) void name(void) __interrupt(vector)
#define INTERRUPT_USING(name, vector, regnum) void name(void) __interrupt(vector) __using(regnum)

// Interrupt numbers
#define INT_NO_INT0       0         // interrupt number for INT0
#define INT_NO_TMR0       1         // interrupt number for timer0
#define INT_NO_INT1       2         // interrupt number for INT1
#define INT_NO_TMR1       3         // interrupt number for timer1
#define INT_NO_UART1      4         // interrupt number for UART1
#define INT_NO_ADC        5         // interrupt number for ADC
#define INT_NO_LVD        6         // interrupt number for LVD
#define INT_NO_PCA        7         // interrupt number for PCA
#define INT_NO_UART2      8         // interrupt number for UART2
#define INT_NO_SPI        9         // interrupt number for SPI
#define INT_NO_INT2       10        // interrupt number for INT2
#define INT_NO_INT3       11        // interrupt number for INT3
#define INT_NO_TMR2       12        // interrupt number for timer2
#define INT_NO_INT4       16        // interrupt number for INT4
#define INT_NO_UART3      17        // interrupt number for UART3
#define INT_NO_UART4      18        // interrupt number for UART4
#define INT_NO_TMR3       19        // interrupt number for timer3
#define INT_NO_TMR4       20        // interrupt number for timer4
#define INT_NO_CMP        21        // interrupt number for CMP
#define INT_NO_I2C        24        // interrupt number for I2C
#define INT_NO_USB        25        // interrupt number for USB
#define INT_NO_PWMA       26        // interrupt number for PWMA
#define INT_NO_PWMB       27        // interrupt number for PWMB
#define INT_NO_TKSU       35        // interrupt number for TKSU
#define INT_NO_RTC        36        // interrupt number for RTC
#define INT_NO_P0         37        // interrupt number for P0
#define INT_NO_P1         38        // interrupt number for P1
#define INT_NO_P2         39        // interrupt number for P2
#define INT_NO_P3         40        // interrupt number for P3
#define INT_NO_P4         41        // interrupt number for P4
#define INT_NO_P5         42        // interrupt number for P5
#define INT_NO_P6         43        // interrupt number for P6
#define INT_NO_P7         44        // interrupt number for P7
#define INT_NO_DMA_M2M    47        // interrupt number for DMA memcopy
#define INT_NO_DMA_ADC    48        // interrupt number for DMA ADC
#define INT_NO_DMA_SPI    49        // interrupt number for DMA SPI
#define INT_NO_DMA_U1T    50        // interrupt number for DMA UART1 TX
#define INT_NO_DMA_U1R    51        // interrupt number for DMA UART1 RX
#define INT_NO_DMA_U2T    52        // interrupt number for DMA UART2 TX
#define INT_NO_DMA_U2R    53        // interrupt number for DMA UART2 RX
#define INT_NO_DMA_U3T    54        // interrupt number for DMA UART3 TX
#define INT_NO_DMA_U3R    55        // interrupt number for DMA UART3 RX
#define INT_NO_DMA_U4T    56        // interrupt number for DMA UART4 TX
#define INT_NO_DMA_U4R    57        // interrupt number for DMA UART4 RX
#define INT_NO_DMA_LCM    58        // interrupt number for DMA LCM
#define INT_NO_LCM        59        // interrupt number for LCM

// Interrupt vector addresses
#define INT_ADDR_INT0     0x0003    // interrupt vector address for INT0
#define INT_ADDR_TMR0     0x000B    // interrupt vector address for timer0
#define INT_ADDR_INT1     0x0013    // interrupt vector address for INT1
#define INT_ADDR_TMR1     0x001B    // interrupt vector address for timer1
#define INT_ADDR_UART1    0x0023    // interrupt vector address for UART1
#define INT_ADDR_ADC      0x002B    // interrupt vector address for ADC
#define INT_ADDR_LVD      0x0033    // interrupt vector address for LVD
#define INT_ADDR_PCA      0x003B    // interrupt vector address for PCA
#define INT_ADDR_UART2    0x0043    // interrupt vector address for UART2
#define INT_ADDR_SPI      0x004B    // interrupt vector address for SPI
#define INT_ADDR_INT2     0x0053    // interrupt vector address for INT2
#define INT_ADDR_INT3     0x005B    // interrupt vector address for INT3
#define INT_ADDR_TMR2     0x0063    // interrupt vector address for timer2
#define INT_ADDR_INT4     0x0083    // interrupt vector address for INT4
#define INT_ADDR_UART3    0x008B    // interrupt vector address for UART3
#define INT_ADDR_UART4    0x0093    // interrupt vector address for UART4
#define INT_ADDR_TMR3     0x009B    // interrupt vector address for timer3
#define INT_ADDR_TMR4     0x00A3    // interrupt vector address for timer4
#define INT_ADDR_CMP      0x00AB    // interrupt vector address for CMP
#define INT_ADDR_I2C      0x00C3    // interrupt vector address for I2C
#define INT_ADDR_USB      0x00CB    // interrupt vector address for USB
#define INT_ADDR_PWMA     0x00D3    // interrupt vector address for PWMA
#define INT_ADDR_PWMB     0x00DB    // interrupt vector address for PWMB
#define INT_ADDR_TKSU     0x011B    // interrupt vector address for TKSU
#define INT_ADDR_RTC      0x0123    // interrupt vector address for RTC
#define INT_ADDR_P0       0x012B    // interrupt vector address for P0
#define INT_ADDR_P1       0x0133    // interrupt vector address for P1
#define INT_ADDR_P2       0x013B    // interrupt vector address for P2
#define INT_ADDR_P3       0x0143    // interrupt vector address for P3
#define INT_ADDR_P4       0x014B    // interrupt vector address for P4
#define INT_ADDR_P5       0x0153    // interrupt vector address for P5
#define INT_ADDR_P6       0x015B    // interrupt vector address for P6
#define INT_ADDR_P7       0x0163    // interrupt vector address for P7
#define INT_ADDR_DMA_M2M  0x017B    // interrupt vector address for DMA
#define INT_ADDR_DMA_ADC  0x0183    // interrupt vector address for DMA
#define INT_ADDR_DMA_SPI  0x018B    // interrupt vector address for DMA
#define INT_ADDR_DMA_U1T  0x0193    // interrupt vector address for DMA
#define INT_ADDR_DMA_U1R  0x019B    // interrupt vector address for DMA
#define INT_ADDR_DMA_U2T  0x01A3    // interrupt vector address for DMA
#define INT_ADDR_DMA_U2R  0x01AB    // interrupt vector address for DMA
#define INT_ADDR_DMA_U3T  0x01B3    // interrupt vector address for DMA
#define INT_ADDR_DMA_U3R  0x01BB    // interrupt vector address for DMA
#define INT_ADDR_DMA_U4T  0x01C3    // interrupt vector address for DMA
#define INT_ADDR_DMA_U4R  0x01CB    // interrupt vector address for DMA
#define INT_ADDR_DMA_LCM  0x01D3    // interrupt vector address for DMA
#define INT_ADDR_LCM      0x01DB    // interrupt vector address for LCM

// Peripheral register addresses
#define  _P0            0x80
SFR(P0,                 _P0);
SBIT(PP00,              _P0, 0);
SBIT(PP01,              _P0, 1);
SBIT(PP02,              _P0, 2);
SBIT(PP03,              _P0, 3);
SBIT(PP04,              _P0, 4);
SBIT(PP05,              _P0, 5);
SBIT(PP06,              _P0, 6);
SBIT(PP07,              _P0, 7);
SFR(SP,                 0x81);
SFR(DPL,                0x82);
SFR(DPH,                0x83);
SFR(S4CON,              0x84);
SFR(S4BUF,              0x85);
SFR(PCON,               0x87);

#define  _TCON          0x88
SFR(TCON,               _TCON);
SBIT(TF1,               _TCON, 7);
SBIT(TR1,               _TCON, 6);
SBIT(TF0,               _TCON, 5);
SBIT(TR0,               _TCON, 4);
SBIT(IE1,               _TCON, 3);
SBIT(IT1,               _TCON, 2);
SBIT(IE0,               _TCON, 1);
SBIT(IT0,               _TCON, 0);
SFR(TMOD,               0x89);

SFR(TL0,                0x8A);
SFR(TL1,                0x8B);
SFR(TH0,                0x8C);
SFR(TH1,                0x8D);
SFR(AUXR,               0x8E);
SFR(INTCLKO,            0x8F);

#define  _P1            0x90
SFR(P1,                 _P1);
SBIT(PP10,              _P1, 0);
SBIT(PP11,              _P1, 1);
SBIT(PP12,              _P1, 2);
SBIT(PP13,              _P1, 3);
SBIT(PP14,              _P1, 4);
SBIT(PP15,              _P1, 5);
SBIT(PP16,              _P1, 6);
SBIT(PP17,              _P1, 7);
SFR(P1M1,               0x91);
SFR(P1M0,               0x92);
SFR(P0M1,               0x93);
SFR(P0M0,               0x94);
SFR(P2M1,               0x95);
SFR(P2M0,               0x96);

#define  _SCON          0x98
SFR(SCON,               _SCON);
SBIT(SM0,               _SCON, 7);
SBIT(SM1,               _SCON, 6);
SBIT(SM2,               _SCON, 5);
SBIT(REN,               _SCON, 4);
SBIT(TB8,               _SCON, 3);
SBIT(RB8,               _SCON, 2);
SBIT(TI,                _SCON, 1);
SBIT(RI,                _SCON, 0);
SFR(SBUF,               0x99);
SFR(S2CON,              0x9A);
SFR(S2BUF,              0x9B);
SFR(IRCBAND,            0x9D);
SFR(LIRTRIM,            0x9E);
SFR(IRTRIM,             0x9F);

#define  _P2            0xA0
SFR(P2,                 _P2);
SBIT(PP20,              _P2, 0);
SBIT(PP21,              _P2, 1);
SBIT(PP22,              _P2, 2);
SBIT(PP23,              _P2, 3);
SBIT(PP24,              _P2, 4);
SBIT(PP25,              _P2, 5);
SBIT(PP26,              _P2, 6);
SBIT(PP27,              _P2, 7);
SFR(BUS_SPEED,          0xA1);
SFR(P_SW1,              0xA2);

#define  _IE            0xA8
SFR(IE,                 _IE);
SBIT(EA,                _IE, 7);
SBIT(ELVD,              _IE, 6);
SBIT(EADC,              _IE, 5);
SBIT(ES,                _IE, 4);
SBIT(ET1,               _IE, 3);
SBIT(EX1,               _IE, 2);
SBIT(ET0,               _IE, 1);
SBIT(EX0,               _IE, 0);
SFR(SADDR,              0xA9);
SFR(WKTCL,              0xAA);
SFR(WKTCH,              0xAB);
SFR(S3CON,              0xAC);
SFR(S3BUF,              0xAD);
SFR(TA,                 0xAE);
SFR(IE2,                0xAF);

#define  _P3            0xB0
SFR(P3,                 _P3);
SBIT(PP30,              _P3, 0);
SBIT(PP31,              _P3, 1);
SBIT(PP32,              _P3, 2);
SBIT(PP33,              _P3, 3);
SBIT(PP34,              _P3, 4);
SBIT(PP35,              _P3, 5);
SBIT(PP36,              _P3, 6);
SBIT(PP37,              _P3, 7);
SFR(P3M1,               0xB1);
SFR(P3M0,               0xB2);
SFR(P4M1,               0xB3);
SFR(P4M0,               0xB4);
SFR(IP2,                0xB5);
SFR(IP2H,               0xB6);
SFR(IPH,                0xB7);

#define  _IP            0xB8
SFR(IP,                 _IP);
SBIT(PPCA,              _IP, 7);
SBIT(PLVD,              _IP, 6);
SBIT(PADC,              _IP, 5);
SBIT(PS,                _IP, 4);
SBIT(PT1,               _IP, 3);
SBIT(PX1,               _IP, 2);
SBIT(PT0,               _IP, 1);
SBIT(PX0,               _IP, 0);
SFR(SADEN,              0xB9);
SFR(P_SW2,              0xBA);
SFR(ADC_CONTR,          0xBC);
SFR(ADC_RES,            0xBD);
SFR(ADC_RESL,           0xBE);

#define  _P4            0xC0
SFR(P4,                 _P4);
SBIT(PP40,              _P4, 0);
SBIT(PP41,              _P4, 1);
SBIT(PP42,              _P4, 2);
SBIT(PP43,              _P4, 3);
SBIT(PP44,              _P4, 4);
SBIT(PP45,              _P4, 5);
SBIT(PP46,              _P4, 6);
SBIT(PP47,              _P4, 7);
SFR(WDT_CONTR,          0xC1);
SFR(IAP_DATA,           0xC2);
SFR(IAP_ADDRH,          0xC3);
SFR(IAP_ADDRL,          0xC4);
SFR(IAP_CMD,            0xC5);
SFR(IAP_TRIG,           0xC6);
SFR(IAP_CONTR,          0xC7);

#define  _P5            0xC8
SFR(P5,                 _P5);
SBIT(PP50,              _P5, 0);
SBIT(PP51,              _P5, 1);
SBIT(PP52,              _P5, 2);
SBIT(PP53,              _P5, 3);
SBIT(PP54,              _P5, 4);
SBIT(PP55,              _P5, 5);
SBIT(PP56,              _P5, 6);
SBIT(PP57,              _P5, 7);
SFR(P5M1,               0xC9);
SFR(P5M0,               0xCA);
SFR(P6M1,               0xCB);
SFR(P6M0,               0xCC);
SFR(SPSTAT,             0xCD);
SFR(SPCTL,              0xCE);
SFR(SPDAT,              0xCF);

#define  _PSW           0xD0
SFR(PSW,                _PSW);
SBIT(CY,                _PSW, 7);
SBIT(AC,                _PSW, 6);
SBIT(F0,                _PSW, 5);
SBIT(RS1,               _PSW, 4);
SBIT(RS0,               _PSW, 3);
SBIT(OV,                _PSW, 2);
SBIT(F1,                _PSW, 1);
SBIT(P,                 _PSW, 0);
SFR(T4T3M,              0xD1);
SFR(T4H,                0xD2);
SFR(T4L,                0xD3);
SFR(T3H,                0xD4);
SFR(T3L,                0xD5);
SFR(T2H,                0xD6);
SFR(T2L,                0xD7);

SFR(ACC,                0xE0);

#define  _P6            0xE8
SFR(P6,                 _P6);
SBIT(PP60,              _P6, 0);
SBIT(PP61,              _P6, 1);
SBIT(PP62,              _P6, 2);
SBIT(PP63,              _P6, 3);
SBIT(PP64,              _P6, 4);
SBIT(PP65,              _P6, 5);
SBIT(PP66,              _P6, 6);
SBIT(PP67,              _P6, 7);

SFR(B,                  0xF0);

#define  _P7            0xF8
SFR(P7,                 _P7);
SBIT(PP70,              _P7, 0);
SBIT(PP71,              _P7, 1);
SBIT(PP72,              _P7, 2);
SBIT(PP73,              _P7, 3);
SBIT(PP74,              _P7, 4);
SBIT(PP75,              _P7, 5);
SBIT(PP76,              _P7, 6);
SBIT(PP77,              _P7, 7);

SFR(VRTRIM,             0xA6);

SFR(USBCLK,             0xDC);

SFR(ADCCFG,             0xDE);
SFR(IP3,                0xDF);

SFR(P7M1,               0xE1);
SFR(P7M0,               0xE2);
SFR(DPS,                0xE3);
SFR(DPL1,               0xE4);
SFR(DPH1,               0xE5);
SFR(CMPCR1,             0xE6);
SFR(CMPCR2,             0xE7);

SFR(USBDAT,             0xEC);
SFR(IP3H,               0xEE);
SFR(AUXINTIF,           0xEF);

SFR(USBCON,             0xF4);

SFR(IAP_TPS,            0xF5);

SFR(USBADR,             0xFC);

SFR(RSTCFG,             0xFF);

// Set B7 of P_SW2 before read/write the following registers

#define CKSEL             (*(unsigned char volatile __XDATA *)0xfe00)
#define CLKDIV            (*(unsigned char volatile __XDATA *)0xfe01)
#define IRCCR             (*(unsigned char volatile __XDATA *)0xfe02)
#define HIRCCR            (*(unsigned char volatile __XDATA *)0xfe02)
#define XOSCCR            (*(unsigned char volatile __XDATA *)0xfe03)
#define IRC32KCR          (*(unsigned char volatile __XDATA *)0xfe04)
#define MCLKOCR           (*(unsigned char volatile __XDATA *)0xfe05)
#define IRCDB             (*(unsigned char volatile __XDATA *)0xfe06)
#define IRC48MCR          (*(unsigned char volatile __XDATA *)0xfe07)
#define X32KCR            (*(unsigned char volatile __XDATA *)0xfe08)
#define RSTFLAG           (*(unsigned char volatile __XDATA *)0xfe09)
#define PxPU                                                  0xfe10
#define P0PU                                           SFRX(PxPU + 0)
#define P1PU                                           SFRX(PxPU + 1)
#define P2PU              (*(unsigned char volatile __XDATA *)0xfe12)
#define P3PU              (*(unsigned char volatile __XDATA *)0xfe13)
#define P4PU              (*(unsigned char volatile __XDATA *)0xfe14)
#define P5PU              (*(unsigned char volatile __XDATA *)0xfe15)
#define P6PU              (*(unsigned char volatile __XDATA *)0xfe16)
#define P7PU              (*(unsigned char volatile __XDATA *)0xfe17)
#define PxNCS                                                 0xfe18
#define P0NCS             (*(unsigned char volatile __XDATA *)0xfe18)
#define P1NCS             (*(unsigned char volatile __XDATA *)0xfe19)
#define P2NCS             (*(unsigned char volatile __XDATA *)0xfe1a)
#define P3NCS             (*(unsigned char volatile __XDATA *)0xfe1b)
#define P4NCS             (*(unsigned char volatile __XDATA *)0xfe1c)
#define P5NCS             (*(unsigned char volatile __XDATA *)0xfe1d)
#define P6NCS             (*(unsigned char volatile __XDATA *)0xfe1e)
#define P7NCS             (*(unsigned char volatile __XDATA *)0xfe1f)
#define PxSR                                                  0xfe20
#define P0SR              (*(unsigned char volatile __XDATA *)0xfe20)
#define P1SR              (*(unsigned char volatile __XDATA *)0xfe21)
#define P2SR              (*(unsigned char volatile __XDATA *)0xfe22)
#define P3SR              (*(unsigned char volatile __XDATA *)0xfe23)
#define P4SR              (*(unsigned char volatile __XDATA *)0xfe24)
#define P5SR              (*(unsigned char volatile __XDATA *)0xfe25)
#define P6SR              (*(unsigned char volatile __XDATA *)0xfe26)
#define P7SR              (*(unsigned char volatile __XDATA *)0xfe27)
#define PxDR                                                  0xfe28
#define P0DR              (*(unsigned char volatile __XDATA *)0xfe28)
#define P1DR              (*(unsigned char volatile __XDATA *)0xfe29)
#define P2DR              (*(unsigned char volatile __XDATA *)0xfe2a)
#define P3DR              (*(unsigned char volatile __XDATA *)0xfe2b)
#define P4DR              (*(unsigned char volatile __XDATA *)0xfe2c)
#define P5DR              (*(unsigned char volatile __XDATA *)0xfe2d)
#define P6DR              (*(unsigned char volatile __XDATA *)0xfe2e)
#define P7DR              (*(unsigned char volatile __XDATA *)0xfe2f)
#define PxIE                                                  0xfe30
#define P0IE              (*(unsigned char volatile __XDATA *)0xfe30)
#define P1IE              (*(unsigned char volatile __XDATA *)0xfe31)
#define P2IE              (*(unsigned char volatile __XDATA *)0xfe32)
#define P3IE              (*(unsigned char volatile __XDATA *)0xfe33)
#define P4IE              (*(unsigned char volatile __XDATA *)0xfe34)
#define P5IE              (*(unsigned char volatile __XDATA *)0xfe35)
#define P6IE              (*(unsigned char volatile __XDATA *)0xfe36)
#define P7IE              (*(unsigned char volatile __XDATA *)0xfe37)
#define LCMIFCFG          (*(unsigned char volatile __XDATA *)0xfe50)
#define RTCCR             (*(unsigned char volatile __XDATA *)0xfe60)
#define RTCCFG            (*(unsigned char volatile __XDATA *)0xfe61)
#define RTCIEN            (*(unsigned char volatile __XDATA *)0xfe62)
#define RTCIF             (*(unsigned char volatile __XDATA *)0xfe63)
#define ALAHOUR           (*(unsigned char volatile __XDATA *)0xfe64)
#define ALAMIN            (*(unsigned char volatile __XDATA *)0xfe65)
#define ALASEC            (*(unsigned char volatile __XDATA *)0xfe66)
#define ALASSEC           (*(unsigned char volatile __XDATA *)0xfe67)
#define INIYEAR           (*(unsigned char volatile __XDATA *)0xfe68)
#define INIMONTH          (*(unsigned char volatile __XDATA *)0xfe69)
#define INIDAY            (*(unsigned char volatile __XDATA *)0xfe6a)
#define INIHOUR           (*(unsigned char volatile __XDATA *)0xfe6b)
#define INIMIN            (*(unsigned char volatile __XDATA *)0xfe6c)
#define INISEC            (*(unsigned char volatile __XDATA *)0xfe6d)
#define INISSEC           (*(unsigned char volatile __XDATA *)0xfe6e)
#define YEAR              (*(unsigned char volatile __XDATA *)0xfe70)
#define MONTH             (*(unsigned char volatile __XDATA *)0xfe71)
#define DAY               (*(unsigned char volatile __XDATA *)0xfe72)
#define HOUR              (*(unsigned char volatile __XDATA *)0xfe73)
#define MIN               (*(unsigned char volatile __XDATA *)0xfe74)
#define SEC               (*(unsigned char volatile __XDATA *)0xfe75)
#define SSEC              (*(unsigned char volatile __XDATA *)0xfe76)

#define I2CCFG            (*(unsigned char volatile __XDATA *)0xfe80)
#define I2CMSCR           (*(unsigned char volatile __XDATA *)0xfe81)
#define I2CMSST           (*(unsigned char volatile __XDATA *)0xfe82)
#define I2CSLCR           (*(unsigned char volatile __XDATA *)0xfe83)
#define I2CSLST           (*(unsigned char volatile __XDATA *)0xfe84)
#define I2CSLADR          (*(unsigned char volatile __XDATA *)0xfe85)
#define I2CTXD            (*(unsigned char volatile __XDATA *)0xfe86)
#define I2CRXD            (*(unsigned char volatile __XDATA *)0xfe87)
#define I2CMSAUX          (*(unsigned char volatile __XDATA *)0xfe88)

#define TM2PS             (*(unsigned char volatile __XDATA *)0xfea2)
#define TM3PS             (*(unsigned char volatile __XDATA *)0xfea3)
#define TM4PS             (*(unsigned char volatile __XDATA *)0xfea4)
#define ADCTIM            (*(unsigned char volatile __XDATA *)0xfea8)

#define PWMA_ETRPS        (*(unsigned char volatile __XDATA *)0xfeb0)
#define PWMA_ENO          (*(unsigned char volatile __XDATA *)0xfeb1)
#define PWMA_PS           (*(unsigned char volatile __XDATA *)0xfeb2)
#define PWMA_IOAUX        (*(unsigned char volatile __XDATA *)0xfeb3)
#define PWMB_ETRPS        (*(unsigned char volatile __XDATA *)0xfeb4)
#define PWMB_ENO          (*(unsigned char volatile __XDATA *)0xfeb5)
#define PWMB_PS           (*(unsigned char volatile __XDATA *)0xfeb6)
#define PWMB_IOAUX        (*(unsigned char volatile __XDATA *)0xfeb7)
#define PWMA_CR1          (*(unsigned char volatile __XDATA *)0xfec0)
#define PWMA_CR2          (*(unsigned char volatile __XDATA *)0xfec1)
#define PWMA_SMCR         (*(unsigned char volatile __XDATA *)0xfec2)
#define PWMA_ETR          (*(unsigned char volatile __XDATA *)0xfec3)
#define PWMA_IER          (*(unsigned char volatile __XDATA *)0xfec4)
#define PWMA_SR1          (*(unsigned char volatile __XDATA *)0xfec5)
#define PWMA_SR2          (*(unsigned char volatile __XDATA *)0xfec6)
#define PWMA_EGR          (*(unsigned char volatile __XDATA *)0xfec7)
#define PWMA_CCMRx                                            0xfec8
#define PWMA_CCMR1        (*(unsigned char volatile __XDATA *)0xfec8)
#define PWMA_CCMR2        (*(unsigned char volatile __XDATA *)0xfec9)
#define PWMA_CCMR3        (*(unsigned char volatile __XDATA *)0xfeca)
#define PWMA_CCMR4        (*(unsigned char volatile __XDATA *)0xfecb)
#define PWMA_CCER1        (*(unsigned char volatile __XDATA *)0xfecc)
#define PWMA_CCER2        (*(unsigned char volatile __XDATA *)0xfecd)

#define PWMA_CNTRH        (*(unsigned char volatile __XDATA *)0xfece)
#define PWMA_CNTRL        (*(unsigned char volatile __XDATA *)0xfecf)

#define PWMA_PSCRH        (*(unsigned char volatile __XDATA *)0xfed0)
#define PWMA_PSCRL        (*(unsigned char volatile __XDATA *)0xfed1)

#define PWMA_ARRH         (*(unsigned char volatile __XDATA *)0xfed2)
#define PWMA_ARRL         (*(unsigned char volatile __XDATA *)0xfed3)
#define PWMA_RCR          (*(unsigned char volatile __XDATA *)0xfed4)

#define PWMA_CCR1H        (*(unsigned char volatile __XDATA *)0xfed5)
#define PWMA_CCR1L        (*(unsigned char volatile __XDATA *)0xfed6)

#define PWMA_CCR2H        (*(unsigned char volatile __XDATA *)0xfed7)
#define PWMA_CCR2L        (*(unsigned char volatile __XDATA *)0xfed8)

#define PWMA_CCR3H        (*(unsigned char volatile __XDATA *)0xfed9)
#define PWMA_CCR3L        (*(unsigned char volatile __XDATA *)0xfeda)

#define PWMA_CCR4H        (*(unsigned char volatile __XDATA *)0xfedb)
#define PWMA_CCR4L        (*(unsigned char volatile __XDATA *)0xfedc)
#define PWMA_BKR          (*(unsigned char volatile __XDATA *)0xfedd)
#define PWMA_DTR          (*(unsigned char volatile __XDATA *)0xfede)
#define PWMA_OISR         (*(unsigned char volatile __XDATA *)0xfedf)
#define PWMB_CR1          (*(unsigned char volatile __XDATA *)0xfee0)
#define PWMB_CR2          (*(unsigned char volatile __XDATA *)0xfee1)
#define PWMB_SMCR         (*(unsigned char volatile __XDATA *)0xfee2)
#define PWMB_ETR          (*(unsigned char volatile __XDATA *)0xfee3)
#define PWMB_IER          (*(unsigned char volatile __XDATA *)0xfee4)
#define PWMB_SR1          (*(unsigned char volatile __XDATA *)0xfee5)
#define PWMB_SR2          (*(unsigned char volatile __XDATA *)0xfee6)
#define PWMB_EGR          (*(unsigned char volatile __XDATA *)0xfee7)
#define PWMB_CCMRx                                            0xfee8
#define PWMB_CCMR1        (*(unsigned char volatile __XDATA *)0xfee8)
#define PWMB_CCMR2        (*(unsigned char volatile __XDATA *)0xfee9)
#define PWMB_CCMR3        (*(unsigned char volatile __XDATA *)0xfeea)
#define PWMB_CCMR4        (*(unsigned char volatile __XDATA *)0xfeeb)
#define PWMB_CCER1        (*(unsigned char volatile __XDATA *)0xfeec)
#define PWMB_CCER2        (*(unsigned char volatile __XDATA *)0xfeed)

#define PWMB_CNTRH        (*(unsigned char volatile __XDATA *)0xfeee)
#define PWMB_CNTRL        (*(unsigned char volatile __XDATA *)0xfeef)

#define PWMB_PSCRH        (*(unsigned char volatile __XDATA *)0xfef0)
#define PWMB_PSCRL        (*(unsigned char volatile __XDATA *)0xfef1)

#define PWMB_ARRH         (*(unsigned char volatile __XDATA *)0xfef2)
#define PWMB_ARRL         (*(unsigned char volatile __XDATA *)0xfef3)
#define PWMB_RCR          (*(unsigned char volatile __XDATA *)0xfef4)

#define PWMB_CCR5H        (*(unsigned char volatile __XDATA *)0xfef5)
#define PWMB_CCR5L        (*(unsigned char volatile __XDATA *)0xfef6)

#define PWMB_CCR6H        (*(unsigned char volatile __XDATA *)0xfef7)
#define PWMB_CCR6L        (*(unsigned char volatile __XDATA *)0xfef8)

#define PWMB_CCR7H        (*(unsigned char volatile __XDATA *)0xfef9)
#define PWMB_CCR7L        (*(unsigned char volatile __XDATA *)0xfefa)

#define PWMB_CCR8H        (*(unsigned char volatile __XDATA *)0xfefb)
#define PWMB_CCR8L        (*(unsigned char volatile __XDATA *)0xfefc)
#define PWMB_BKR          (*(unsigned char volatile __XDATA *)0xfefd)
#define PWMB_DTR          (*(unsigned char volatile __XDATA *)0xfefe)
#define PWMB_OISR         (*(unsigned char volatile __XDATA *)0xfeff)

#define PxINTE                                                0xfd00
#define P0INTE            (*(unsigned char volatile __XDATA *)0xfd00)
#define P1INTE            (*(unsigned char volatile __XDATA *)0xfd01)
#define P2INTE            (*(unsigned char volatile __XDATA *)0xfd02)
#define P3INTE            (*(unsigned char volatile __XDATA *)0xfd03)
#define P4INTE            (*(unsigned char volatile __XDATA *)0xfd04)
#define P5INTE            (*(unsigned char volatile __XDATA *)0xfd05)
#define P6INTE            (*(unsigned char volatile __XDATA *)0xfd06)
#define P7INTE            (*(unsigned char volatile __XDATA *)0xfd07)
#define P0INTF            (*(unsigned char volatile __XDATA *)0xfd10)
#define P1INTF            (*(unsigned char volatile __XDATA *)0xfd11)
#define P2INTF            (*(unsigned char volatile __XDATA *)0xfd12)
#define P3INTF            (*(unsigned char volatile __XDATA *)0xfd13)
#define P4INTF            (*(unsigned char volatile __XDATA *)0xfd14)
#define P5INTF            (*(unsigned char volatile __XDATA *)0xfd15)
#define P6INTF            (*(unsigned char volatile __XDATA *)0xfd16)
#define P7INTF            (*(unsigned char volatile __XDATA *)0xfd17)
#define PxIM0                                                 0xfd20
#define P0IM0             (*(unsigned char volatile __XDATA *)0xfd20)
#define P1IM0             (*(unsigned char volatile __XDATA *)0xfd21)
#define P2IM0             (*(unsigned char volatile __XDATA *)0xfd22)
#define P3IM0             (*(unsigned char volatile __XDATA *)0xfd23)
#define P4IM0             (*(unsigned char volatile __XDATA *)0xfd24)
#define P5IM0             (*(unsigned char volatile __XDATA *)0xfd25)
#define P6IM0             (*(unsigned char volatile __XDATA *)0xfd26)
#define P7IM0             (*(unsigned char volatile __XDATA *)0xfd27)
#define PxIM1                                                 0xfd30
#define P0IM1             (*(unsigned char volatile __XDATA *)0xfd30)
#define P1IM1             (*(unsigned char volatile __XDATA *)0xfd31)
#define P2IM1             (*(unsigned char volatile __XDATA *)0xfd32)
#define P3IM1             (*(unsigned char volatile __XDATA *)0xfd33)
#define P4IM1             (*(unsigned char volatile __XDATA *)0xfd34)
#define P5IM1             (*(unsigned char volatile __XDATA *)0xfd35)
#define P6IM1             (*(unsigned char volatile __XDATA *)0xfd36)
#define P7IM1             (*(unsigned char volatile __XDATA *)0xfd37)
#define P0WKUE            (*(unsigned char volatile __XDATA *)0xfd40)
#define P1WKUE            (*(unsigned char volatile __XDATA *)0xfd41)
#define P2WKUE            (*(unsigned char volatile __XDATA *)0xfd42)
#define P3WKUE            (*(unsigned char volatile __XDATA *)0xfd43)
#define P4WKUE            (*(unsigned char volatile __XDATA *)0xfd44)
#define P5WKUE            (*(unsigned char volatile __XDATA *)0xfd45)
#define P6WKUE            (*(unsigned char volatile __XDATA *)0xfd46)
#define P7WKUE            (*(unsigned char volatile __XDATA *)0xfd47)
#define PIN_IP            (*(unsigned char volatile __XDATA *)0xfd60)
#define PIN_IPH           (*(unsigned char volatile __XDATA *)0xfd61)
#define CHIPIDxx                                              0xfde0
#define CHIPID00          (*(unsigned char volatile __XDATA *)0xfde0)

#define MD3               (*(unsigned char volatile __XDATA *)0xfcf0)
#define MD2               (*(unsigned char volatile __XDATA *)0xfcf1)
#define MD1               (*(unsigned char volatile __XDATA *)0xfcf2)
#define MD0               (*(unsigned char volatile __XDATA *)0xfcf3)
#define MD5               (*(unsigned char volatile __XDATA *)0xfcf4)
#define MD4               (*(unsigned char volatile __XDATA *)0xfcf5)
#define ARCON             (*(unsigned char volatile __XDATA *)0xfcf6)
#define OPCON             (*(unsigned char volatile __XDATA *)0xfcf7)

#define COMEN             (*(unsigned char volatile __XDATA *)0xfb00)
#define SEGENL            (*(unsigned char volatile __XDATA *)0xfb01)
#define SEGENH            (*(unsigned char volatile __XDATA *)0xfb02)
#define LEDCTRL           (*(unsigned char volatile __XDATA *)0xfb03)
#define LEDCKS            (*(unsigned char volatile __XDATA *)0xfb04)
#define COM0_DA_L         (*(unsigned char volatile __XDATA *)0xfb10)
#define COM1_DA_L         (*(unsigned char volatile __XDATA *)0xfb11)
#define COM2_DA_L         (*(unsigned char volatile __XDATA *)0xfb12)
#define COM3_DA_L         (*(unsigned char volatile __XDATA *)0xfb13)
#define COM4_DA_L         (*(unsigned char volatile __XDATA *)0xfb14)
#define COM5_DA_L         (*(unsigned char volatile __XDATA *)0xfb15)
#define COM6_DA_L         (*(unsigned char volatile __XDATA *)0xfb16)
#define COM7_DA_L         (*(unsigned char volatile __XDATA *)0xfb17)
#define COM0_DA_H         (*(unsigned char volatile __XDATA *)0xfb18)
#define COM1_DA_H         (*(unsigned char volatile __XDATA *)0xfb19)
#define COM2_DA_H         (*(unsigned char volatile __XDATA *)0xfb1a)
#define COM3_DA_H         (*(unsigned char volatile __XDATA *)0xfb1b)
#define COM4_DA_H         (*(unsigned char volatile __XDATA *)0xfb1c)
#define COM5_DA_H         (*(unsigned char volatile __XDATA *)0xfb1d)
#define COM6_DA_H         (*(unsigned char volatile __XDATA *)0xfb1e)
#define COM7_DA_H         (*(unsigned char volatile __XDATA *)0xfb1f)
#define COM0_DC_L         (*(unsigned char volatile __XDATA *)0xfb20)
#define COM1_DC_L         (*(unsigned char volatile __XDATA *)0xfb21)
#define COM2_DC_L         (*(unsigned char volatile __XDATA *)0xfb22)
#define COM3_DC_L         (*(unsigned char volatile __XDATA *)0xfb23)
#define COM4_DC_L         (*(unsigned char volatile __XDATA *)0xfb24)
#define COM5_DC_L         (*(unsigned char volatile __XDATA *)0xfb25)
#define COM6_DC_L         (*(unsigned char volatile __XDATA *)0xfb26)
#define COM7_DC_L         (*(unsigned char volatile __XDATA *)0xfb27)
#define COM0_DC_H         (*(unsigned char volatile __XDATA *)0xfb28)
#define COM1_DC_H         (*(unsigned char volatile __XDATA *)0xfb29)
#define COM2_DC_H         (*(unsigned char volatile __XDATA *)0xfb2a)
#define COM3_DC_H         (*(unsigned char volatile __XDATA *)0xfb2b)
#define COM4_DC_H         (*(unsigned char volatile __XDATA *)0xfb2c)
#define COM5_DC_H         (*(unsigned char volatile __XDATA *)0xfb2d)
#define COM6_DC_H         (*(unsigned char volatile __XDATA *)0xfb2e)
#define COM7_DC_H         (*(unsigned char volatile __XDATA *)0xfb2f)

#define TSCHEN1           (*(unsigned char volatile __XDATA *)0xfb40)
#define TSCHEN2           (*(unsigned char volatile __XDATA *)0xfb41)
#define TSCFG1            (*(unsigned char volatile __XDATA *)0xfb42)
#define TSCFG2            (*(unsigned char volatile __XDATA *)0xfb43)
#define TSWUTC            (*(unsigned char volatile __XDATA *)0xfb44)
#define TSCTRL            (*(unsigned char volatile __XDATA *)0xfb45)
#define TSSTA1            (*(unsigned char volatile __XDATA *)0xfb46)
#define TSSTA2            (*(unsigned char volatile __XDATA *)0xfb47)
#define TSRT              (*(unsigned char volatile __XDATA *)0xfb48)

#define TSDATH            (*(unsigned char volatile __XDATA *)0xfb49)
#define TSDATL            (*(unsigned char volatile __XDATA *)0xfb4A)

#define TSTH00H           (*(unsigned char volatile __XDATA *)0xfb50)
#define TSTH00L           (*(unsigned char volatile __XDATA *)0xfb51)

#define TSTH01H           (*(unsigned char volatile __XDATA *)0xfb52)
#define TSTH01L           (*(unsigned char volatile __XDATA *)0xfb53)

#define TSTH02H           (*(unsigned char volatile __XDATA *)0xfb54)
#define TSTH02L           (*(unsigned char volatile __XDATA *)0xfb55)

#define TSTH03H           (*(unsigned char volatile __XDATA *)0xfb56)
#define TSTH03L           (*(unsigned char volatile __XDATA *)0xfb57)

#define TSTH04H           (*(unsigned char volatile __XDATA *)0xfb58)
#define TSTH04L           (*(unsigned char volatile __XDATA *)0xfb59)

#define TSTH05H           (*(unsigned char volatile __XDATA *)0xfb5a)
#define TSTH05L           (*(unsigned char volatile __XDATA *)0xfb5b)

#define TSTH06H           (*(unsigned char volatile __XDATA *)0xfb5c)
#define TSTH06L           (*(unsigned char volatile __XDATA *)0xfb5d)

#define TSTH07H           (*(unsigned char volatile __XDATA *)0xfb5e)
#define TSTH07L           (*(unsigned char volatile __XDATA *)0xfb5f)

#define TSTH08H           (*(unsigned char volatile __XDATA *)0xfb60)
#define TSTH08L           (*(unsigned char volatile __XDATA *)0xfb61)

#define TSTH09H           (*(unsigned char volatile __XDATA *)0xfb62)
#define TSTH09L           (*(unsigned char volatile __XDATA *)0xfb63)

#define TSTH10H           (*(unsigned char volatile __XDATA *)0xfb64)
#define TSTH10L           (*(unsigned char volatile __XDATA *)0xfb65)

#define TSTH11H           (*(unsigned char volatile __XDATA *)0xfb66)
#define TSTH11L           (*(unsigned char volatile __XDATA *)0xfb67)

#define TSTH12H           (*(unsigned char volatile __XDATA *)0xfb68)
#define TSTH12L           (*(unsigned char volatile __XDATA *)0xfb69)

#define TSTH13H           (*(unsigned char volatile __XDATA *)0xfb6a)
#define TSTH13L           (*(unsigned char volatile __XDATA *)0xfb6b)

#define TSTH14H           (*(unsigned char volatile __XDATA *)0xfb6c)
#define TSTH14L           (*(unsigned char volatile __XDATA *)0xfb6d)

#define TSTH15H           (*(unsigned char volatile __XDATA *)0xfb6e)
#define TSTH15L           (*(unsigned char volatile __XDATA *)0xfb6f)

#define DMA_M2M_CFG       (*(unsigned char volatile __XDATA *)0xfa00)
#define DMA_M2M_CR        (*(unsigned char volatile __XDATA *)0xfa01)
#define DMA_M2M_STA       (*(unsigned char volatile __XDATA *)0xfa02)
#define DMA_M2M_AMT       (*(unsigned char volatile __XDATA *)0xfa03)
#define DMA_M2M_DONE      (*(unsigned char volatile __XDATA *)0xfa04)
#define DMA_M2M_TXAH      (*(unsigned char volatile __XDATA *)0xfa05)
#define DMA_M2M_TXAL      (*(unsigned char volatile __XDATA *)0xfa06)
#define DMA_M2M_RXAH      (*(unsigned char volatile __XDATA *)0xfa07)
#define DMA_M2M_RXAL      (*(unsigned char volatile __XDATA *)0xfa08)

#define DMA_ADC_CFG       (*(unsigned char volatile __XDATA *)0xfa10)
#define DMA_ADC_CR        (*(unsigned char volatile __XDATA *)0xfa11)
#define DMA_ADC_STA       (*(unsigned char volatile __XDATA *)0xfa12)
#define DMA_ADC_RXAH      (*(unsigned char volatile __XDATA *)0xfa17)
#define DMA_ADC_RXAL      (*(unsigned char volatile __XDATA *)0xfa18)
#define DMA_ADC_CFG2      (*(unsigned char volatile __XDATA *)0xfa19)
#define DMA_ADC_CHSW0     (*(unsigned char volatile __XDATA *)0xfa1a)
#define DMA_ADC_CHSW1     (*(unsigned char volatile __XDATA *)0xfa1b)

#define DMA_SPI_CFG       (*(unsigned char volatile __XDATA *)0xfa20)
#define DMA_SPI_CR        (*(unsigned char volatile __XDATA *)0xfa21)
#define DMA_SPI_STA       (*(unsigned char volatile __XDATA *)0xfa22)
#define DMA_SPI_AMT       (*(unsigned char volatile __XDATA *)0xfa23)
#define DMA_SPI_DONE      (*(unsigned char volatile __XDATA *)0xfa24)
#define DMA_SPI_TXAH      (*(unsigned char volatile __XDATA *)0xfa25)
#define DMA_SPI_TXAL      (*(unsigned char volatile __XDATA *)0xfa26)
#define DMA_SPI_RXAH      (*(unsigned char volatile __XDATA *)0xfa27)
#define DMA_SPI_RXAL      (*(unsigned char volatile __XDATA *)0xfa28)
#define DMA_SPI_CFG2      (*(unsigned char volatile __XDATA *)0xfa29)

#define DMA_UR1T_CFG      (*(unsigned char volatile __XDATA *)0xfa30)
#define DMA_UR1T_CR       (*(unsigned char volatile __XDATA *)0xfa31)
#define DMA_UR1T_STA      (*(unsigned char volatile __XDATA *)0xfa32)
#define DMA_UR1T_AMT      (*(unsigned char volatile __XDATA *)0xfa33)
#define DMA_UR1T_DONE     (*(unsigned char volatile __XDATA *)0xfa34)
#define DMA_UR1T_TXAH     (*(unsigned char volatile __XDATA *)0xfa35)
#define DMA_UR1T_TXAL     (*(unsigned char volatile __XDATA *)0xfa36)
#define DMA_UR1R_CFG      (*(unsigned char volatile __XDATA *)0xfa38)
#define DMA_UR1R_CR       (*(unsigned char volatile __XDATA *)0xfa39)
#define DMA_UR1R_STA      (*(unsigned char volatile __XDATA *)0xfa3a)
#define DMA_UR1R_AMT      (*(unsigned char volatile __XDATA *)0xfa3b)
#define DMA_UR1R_DONE     (*(unsigned char volatile __XDATA *)0xfa3c)
#define DMA_UR1R_RXAH     (*(unsigned char volatile __XDATA *)0xfa3d)
#define DMA_UR1R_RXAL     (*(unsigned char volatile __XDATA *)0xfa3e)

#define DMA_UR2T_CFG      (*(unsigned char volatile __XDATA *)0xfa40)
#define DMA_UR2T_CR       (*(unsigned char volatile __XDATA *)0xfa41)
#define DMA_UR2T_STA      (*(unsigned char volatile __XDATA *)0xfa42)
#define DMA_UR2T_AMT      (*(unsigned char volatile __XDATA *)0xfa43)
#define DMA_UR2T_DONE     (*(unsigned char volatile __XDATA *)0xfa44)
#define DMA_UR2T_TXAH     (*(unsigned char volatile __XDATA *)0xfa45)
#define DMA_UR2T_TXAL     (*(unsigned char volatile __XDATA *)0xfa46)
#define DMA_UR2R_CFG      (*(unsigned char volatile __XDATA *)0xfa48)
#define DMA_UR2R_CR       (*(unsigned char volatile __XDATA *)0xfa49)
#define DMA_UR2R_STA      (*(unsigned char volatile __XDATA *)0xfa4a)
#define DMA_UR2R_AMT      (*(unsigned char volatile __XDATA *)0xfa4b)
#define DMA_UR2R_DONE     (*(unsigned char volatile __XDATA *)0xfa4c)
#define DMA_UR2R_RXAH     (*(unsigned char volatile __XDATA *)0xfa4d)
#define DMA_UR2R_RXAL     (*(unsigned char volatile __XDATA *)0xfa4e)

#define DMA_UR3T_CFG      (*(unsigned char volatile __XDATA *)0xfa50)
#define DMA_UR3T_CR       (*(unsigned char volatile __XDATA *)0xfa51)
#define DMA_UR3T_STA      (*(unsigned char volatile __XDATA *)0xfa52)
#define DMA_UR3T_AMT      (*(unsigned char volatile __XDATA *)0xfa53)
#define DMA_UR3T_DONE     (*(unsigned char volatile __XDATA *)0xfa54)
#define DMA_UR3T_TXAH     (*(unsigned char volatile __XDATA *)0xfa55)
#define DMA_UR3T_TXAL     (*(unsigned char volatile __XDATA *)0xfa56)
#define DMA_UR3R_CFG      (*(unsigned char volatile __XDATA *)0xfa58)
#define DMA_UR3R_CR       (*(unsigned char volatile __XDATA *)0xfa59)
#define DMA_UR3R_STA      (*(unsigned char volatile __XDATA *)0xfa5a)
#define DMA_UR3R_AMT      (*(unsigned char volatile __XDATA *)0xfa5b)
#define DMA_UR3R_DONE     (*(unsigned char volatile __XDATA *)0xfa5c)
#define DMA_UR3R_RXAH     (*(unsigned char volatile __XDATA *)0xfa5d)
#define DMA_UR3R_RXAL     (*(unsigned char volatile __XDATA *)0xfa5e)

#define DMA_UR4T_CFG      (*(unsigned char volatile __XDATA *)0xfa60)
#define DMA_UR4T_CR       (*(unsigned char volatile __XDATA *)0xfa61)
#define DMA_UR4T_STA      (*(unsigned char volatile __XDATA *)0xfa62)
#define DMA_UR4T_AMT      (*(unsigned char volatile __XDATA *)0xfa63)
#define DMA_UR4T_DONE     (*(unsigned char volatile __XDATA *)0xfa64)
#define DMA_UR4T_TXAH     (*(unsigned char volatile __XDATA *)0xfa65)
#define DMA_UR4T_TXAL     (*(unsigned char volatile __XDATA *)0xfa66)
#define DMA_UR4R_CFG      (*(unsigned char volatile __XDATA *)0xfa68)
#define DMA_UR4R_CR       (*(unsigned char volatile __XDATA *)0xfa69)
#define DMA_UR4R_STA      (*(unsigned char volatile __XDATA *)0xfa6a)
#define DMA_UR4R_AMT      (*(unsigned char volatile __XDATA *)0xfa6b)
#define DMA_UR4R_DONE     (*(unsigned char volatile __XDATA *)0xfa6c)
#define DMA_UR4R_RXAH     (*(unsigned char volatile __XDATA *)0xfa6d)
#define DMA_UR4R_RXAL     (*(unsigned char volatile __XDATA *)0xfa6e)

#define DMA_LCM_CFG       (*(unsigned char volatile __XDATA *)0xfa70)
#define DMA_LCM_CR        (*(unsigned char volatile __XDATA *)0xfa71)
#define DMA_LCM_STA       (*(unsigned char volatile __XDATA *)0xfa72)
#define DMA_LCM_AMT       (*(unsigned char volatile __XDATA *)0xfa73)
#define DMA_LCM_DONE      (*(unsigned char volatile __XDATA *)0xfa74)
#define DMA_LCM_TXAH      (*(unsigned char volatile __XDATA *)0xfa75)
#define DMA_LCM_TXAL      (*(unsigned char volatile __XDATA *)0xfa76)
#define DMA_LCM_RXAH      (*(unsigned char volatile __XDATA *)0xfa77)
#define DMA_LCM_RXAL      (*(unsigned char volatile __XDATA *)0xfa78)
