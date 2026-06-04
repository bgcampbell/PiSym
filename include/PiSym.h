#pragma once

// #include <Arduino.h>
#include <stdint-gcc.h>
#include <string.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/platform.h>
#include <hardware/timer.h>
#include <hardware/structs/systick.h>
// #include <string>
#include "Config.hpp"
#include "PinAllocations.h"

class TConfig;
class TTape;

// #define _TEST_
// #define _SPEEDTEST_ 7
// #define FULLSPEED


// Bit 7 is DebuggerDisable
enum eMemoryAddressResult { MEMORY, NOTHING,
                            ReadORA, WriteORA, ReadORB, WriteORB,
                            WriteDDRA, WriteDDRB,
                            WriteTimerDiv1IRQON, WriteTimerDiv8IRQON, WriteTimerDiv64IRQON, WriteTimerDiv1024IRQON,
                            WriteTimerDiv1IRQOFF, WriteTimerDiv8IRQOFF, WriteTimerDiv64IRQOFF, WriteTimerDiv1024IRQOFF,
                            ReadTimerIRQON, ReadTimerIRQOFF,
                            PA7IRQOFFNEG, PA7IRQOFFPOS, PA7IRQONNEG, PA7IRQONPOS, ReadIRQFlags,
                            ReadCRT, ReadCRT_RESXAF, WriteCRT, SelectCRT, BaudCRT, TSTAT_CRT,
                            WriteProtect, WriteProtectPort,
                            LoadTape, SaveTape, IgnoreSubroutine, SetGPIO25LED
                          };

// Writeable and readable addresses
#define SY6532BASE       0xA400

#define SY6532_ORA         0x00
#define SY6532_DDRA        0x01
#define SY6532_ORB         0x02
#define SY6532_DDRB        0x03
#define SY6532_T1ION       0x14
#define SY6532_T8ION       0x15
#define SY6532_T64ION      0x16
#define SY6532_T1024ION    0x17
#define SY6532_T1IOFF      0x1C
#define SY6532_T8IOFF      0x1D
#define SY6532_T64IOFF     0x1E
#define SY6532_T1024IOFF   0x1F

#define NMIVECTOR       0xFFFA
#define RESETVECTOR     0xFFFC
#define IRQBRKVECTOR    0xFFFE

#define SYSRAM_NMIVECTOR    0xA67A
#define SYSRAM_RESETVECTOR  0xA67C
#define SYSRAM_IRQBRKVECTOR 0xA67E

#define TAPE_MODE       0x00FD    // $80 = SYM-1
#define BUFAD           0x00FE    // Used by Tape Load
#define FILE_ID         0xA64E
#define FILE_START      0xA64C
#define FILE_END        0xA64A

#define M0PLUS_SYST_CVR_OFFSET ((uint32_t)(0x0000e018))

union USR
{
    uint8_t Value;
    struct
    {
        unsigned int        C : 1;
        unsigned int        Z : 1;
        unsigned int        I : 1;
        unsigned int        D : 1;
        unsigned int        B : 1;
        unsigned int        One : 1;
        unsigned int        V : 1;
        unsigned int        N : 1;
    } Bits;
};

extern uint8_t nrCycles;
extern uint32_t RemainingCycles;
extern uint32_t StartTiming;
extern uint32_t ActualTiming;
extern bool SkipDelay;
extern bool ActivateBreakpoint;
extern bool SystemRAMWriteProtected;
extern bool RAMPage1WriteProtected;
extern bool RAMPage2WriteProtected;
extern bool RAMPage3WriteProtected;

extern bool UserPin25LED;

extern uint8_t                  A;
extern uint8_t                  X;
extern uint8_t                  Y;
extern uint8_t                  SP;
extern uint16_t                 PC;
extern union USR                SR;

extern TConfig Configuration;
extern TTape   CurrentTape;

extern uint8_t Memory[];
extern uint8_t memoryTypeRead[];
extern uint8_t memoryTypeWrite[];
extern uint16_t addressMap[];
extern uint8_t debugRAM[];

inline uint16_t MapAddress(uint16_t fAddress)
{
  if(fAddress < 0x8000)
    return fAddress;
  return addressMap[fAddress - 0x8000];
}
inline void MapAddress(uint16_t fAddress, uint16_t fValue)
{
  if(fAddress < 0x8000)
    return;
  addressMap[fAddress - 0x8000] = fValue;
}
inline eMemoryAddressResult MemoryTypeRead(uint16_t fAddress)
{
  if(fAddress < 0x8000)
  {
    if(debugRAM[fAddress / 8] & (1 << (fAddress % 8)))
      return (eMemoryAddressResult)(eMemoryAddressResult::MEMORY | 0x80); // Set Debug Bit
    else
      return eMemoryAddressResult::MEMORY;
  }
  return (eMemoryAddressResult)memoryTypeRead[fAddress - 0x8000];
}
inline void MemoryTypeRead(uint16_t fAddress, eMemoryAddressResult fValue)
{
  if(fAddress < 0x8000)
  {
    if((fValue & 0x80) != 0)
       debugRAM[fAddress / 8] |= (1 << (fAddress % 8));
    else
       debugRAM[fAddress / 8] &= ~(1 << (fAddress % 8));
    return;
  }
  memoryTypeRead[fAddress - 0x8000] = fValue;
}
inline eMemoryAddressResult MemoryTypeWrite(uint16_t fAddress)
{
  if(fAddress < 0x8000)
  {
    if((fAddress >= 0x0400) && (fAddress < 0x01000))
      return eMemoryAddressResult::WriteProtect;
    return eMemoryAddressResult::MEMORY;
  }
  return (eMemoryAddressResult)memoryTypeWrite[fAddress - 0x8000];
}
inline void MemoryTypeWrite(uint16_t fAddress, eMemoryAddressResult fValue)
{
  if(fAddress < 0x8000)
    return;
  memoryTypeWrite[fAddress - 0x8000] = fValue;
}

extern const int symon1_1Size;
extern const uint8_t symon1_1[];

extern const int BASIC1_1Size;
extern const uint8_t BASIC1_1[];

extern const int BasicTrigSize;
extern const uint8_t BasicTrig[];

extern const int RAE_B000Size;
extern const uint8_t RAE_B000[];

extern const int RAE_E000Size;
extern const uint8_t RAE_E000[];

extern const int LDA_TestSize;
extern const unsigned char LDA_Test[];

extern volatile uint8_t SY6532_TimerIRQRequest;
extern uint8_t SY6532_PA7NegativeEdge;
extern uint8_t SY6532_PA7IRQRequest;
extern uint8_t SY6532_PA7Triggered;
extern uint8_t lastPA7;

extern uint32_t lastCycleCount;
extern uint32_t cycleCount;

extern volatile uint8_t SY6532_TimerValue;
extern volatile uint8_t SY6532_TimerIRQRequest;
extern volatile uint8_t SY6532_TimerIRQTriggered;
extern volatile short SY6532_Timer_Prescaler;
extern volatile short prescalerCounter;

void copyArrayToMemory(const uint8_t* fArray, uint16_t fMemoryAddress, int fSize);

void createMemoryAnalysis();
void __not_in_flash_func(nmi)(uint16_t fInstructionPC);
void reset(void);
void __not_in_flash_func(irq)(void);

void push(uint8_t fValue);
void push(uint16_t fValue);
uint16_t popWord();
enum eMemoryAddressResult testAddress(uint16_t *fAddr, int fWrite);
uint8_t SingleStep();
void dumpMapPage(uint16_t fStart, uint16_t fSize);
void dumpMemory(uint16_t fStart, uint16_t fSize);
void CRLF();
void Loop();

inline uint32_t getCycleCount()
{
  uint32_t result = systick_hw->cvr;

  return result;
}

inline void setCycleCount(uint32_t fValue)
{
  systick_hw->csr = 0x00;
  systick_hw->rvr = fValue;
  systick_hw->cvr = 0x00000000L;
  systick_hw->csr = 0x05;
}


inline void getPA7Interrupt()
{
  // IfPA7 is an output, no IRQ chance of trigger
  if((Memory[SY6532BASE + SY6532_DDRA] & 0x80) != 0x00)
    return;

  uint8_t PA7_State = gpio_get(PA7);

  if(SY6532_PA7NegativeEdge)
  {
    if((lastPA7 != 0x00) && (PA7_State == 0x00))
      SY6532_PA7Triggered = 0x40;
  }
  else
  {
    if((lastPA7 == 0x00) && (PA7_State != 0x00))
      SY6532_PA7Triggered = 0x40;
  }
  lastPA7 = PA7_State;
}
