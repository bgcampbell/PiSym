#include <stdio.h>
#include "pico/stdlib.h"
#include "Instructions.h"
#include "PiSym.h"
#include "InstructionTest.h"
#include "CommandLine.h"
#include "pico_hal.h"
// #include "Tape.hpp"

uint8_t           A;
uint8_t           X;
uint8_t           Y;
uint8_t           SP;
uint16_t          PC;
USR               SR;

uint8_t  nrCycles;
uint32_t RemainingCycles;
uint32_t StartTiming;
uint32_t ActualTiming;
bool     SkipDelay;
bool     PowerOn;

uint8_t Memory[65536];

uint8_t memoryTypeRead[32768];
uint8_t memoryTypeWrite[32768];
uint16_t addressMap[32768];
uint8_t debugRAM[0x8000 / 8];

uint8_t lastNMIState = 1;
uint8_t lastPA7 = 0;

volatile uint8_t SY6532_TimerValue = 1;
volatile uint8_t SY6532_TimerIRQRequest = 0;
volatile uint8_t SY6532_TimerIRQTriggered = 0;
volatile short SY6532_Timer_Prescaler = 1;
volatile short prescalerCounter = 0;

uint8_t SY6532_PA7IRQRequest = 0;
uint8_t SY6532_PA7NegativeEdge = 0;
uint8_t SY6532_PA7Triggered = 0;

bool SystemRAMWriteProtected = false;
bool RAMPage1WriteProtected = false;
bool RAMPage2WriteProtected = false;
bool RAMPage3WriteProtected = false;

bool UserPin25LED = false;

int linecount = 0;
bool tstate = false;

bool ActivateBreakpoint = false;

TConfig Configuration;

TTape   CurrentTape;


//                            0      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15
//                          PA0    PA1    PA2    PA3    PA4    PA5    PA6    PA7    PB0    PB1    PB2    PB3    PB4    PB5    PB6    PB7
const bool gpioInit[] = {  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, 
//                           16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31
//                         IO16   IO17   IO18   IO19   IO20   IO21   SYNC   Resv   Resv    LED    IRQ    NMI    RST   Resv   Resv   Resv
                           true,  true,  true,  true,  true,  true,  true, false, false,  true,  true,  true,  true, false, false,  false
                        };
const int gpioInitCount = sizeof(gpioInit)/sizeof(bool);

const eMemoryAddressResult readFunctions[] =
{
//  0x00                                    0x01                                    0x02                                      0x03
    eMemoryAddressResult::ReadORA,          eMemoryAddressResult::MEMORY,           eMemoryAddressResult::ReadORB,            eMemoryAddressResult::MEMORY,
//  0x04                                    0x05                                    0x06                                      0x07
    eMemoryAddressResult::ReadTimerIRQON,   eMemoryAddressResult::ReadIRQFlags,     eMemoryAddressResult::ReadTimerIRQON,     eMemoryAddressResult::ReadIRQFlags,
//  0x08                                    0x09                                    0x0A                                      0x0B
    eMemoryAddressResult::ReadORA,          eMemoryAddressResult::MEMORY,           eMemoryAddressResult::ReadORB,            eMemoryAddressResult::MEMORY,
//  0x0C                                    0x0D                                    0x0E                                      0x0F
    eMemoryAddressResult::ReadTimerIRQOFF,  eMemoryAddressResult::ReadIRQFlags,     eMemoryAddressResult::ReadTimerIRQOFF,    eMemoryAddressResult::ReadIRQFlags,
//  0x10                                    0x11                                    0x12                                      0x13
    eMemoryAddressResult::ReadORA,          eMemoryAddressResult::MEMORY,           eMemoryAddressResult::ReadORB,            eMemoryAddressResult::MEMORY,
//  0x14                                    0x15                                    0x16                                      0x17
    eMemoryAddressResult::ReadTimerIRQON,   eMemoryAddressResult::ReadIRQFlags,     eMemoryAddressResult::ReadTimerIRQON,     eMemoryAddressResult::ReadIRQFlags,
//  0x18                                    0x19                                    0x1A                                      0x1B
    eMemoryAddressResult::ReadORA,          eMemoryAddressResult::MEMORY,           eMemoryAddressResult::ReadORB,            eMemoryAddressResult::MEMORY,
//  0x1C                                    0x1D                                    0x1E                                      0x1F
    eMemoryAddressResult::ReadTimerIRQOFF,  eMemoryAddressResult::ReadIRQFlags,     eMemoryAddressResult::ReadTimerIRQOFF,    eMemoryAddressResult::ReadIRQFlags,
};

const eMemoryAddressResult writeFunctions[] =
{
//  0x00                                          0x01                                          0x02                                            0x03
    eMemoryAddressResult::WriteORA,               eMemoryAddressResult::WriteDDRA,              eMemoryAddressResult::WriteORB,                 eMemoryAddressResult::WriteDDRB,
//  0x04                                          0x05                                          0x06                                            0x07
    eMemoryAddressResult::PA7IRQOFFNEG,           eMemoryAddressResult::PA7IRQOFFPOS,           eMemoryAddressResult::PA7IRQONNEG,              eMemoryAddressResult::PA7IRQONPOS,
//  0x08                                          0x09                                          0x0A                                            0x0B
    eMemoryAddressResult::WriteORA,               eMemoryAddressResult::MEMORY,                 eMemoryAddressResult::WriteORB,                 eMemoryAddressResult::MEMORY,
//  0x0C                                          0x0D                                          0x0E                                            0x0F
    eMemoryAddressResult::PA7IRQOFFNEG,           eMemoryAddressResult::PA7IRQOFFPOS,           eMemoryAddressResult::PA7IRQONNEG,              eMemoryAddressResult::PA7IRQONPOS,
//  0x10                                          0x11                                          0x12                                            0x13
    eMemoryAddressResult::WriteORA,               eMemoryAddressResult::MEMORY,                 eMemoryAddressResult::WriteORB,                 eMemoryAddressResult::MEMORY,
//  0x14                                          0x15                                          0x16                                            0x17
    eMemoryAddressResult::WriteTimerDiv1IRQON,    eMemoryAddressResult::WriteTimerDiv8IRQON,    eMemoryAddressResult::WriteTimerDiv64IRQON,     eMemoryAddressResult::WriteTimerDiv1024IRQON,
//  0x18                                          0x19                                          0x1A                                            0x1B
    eMemoryAddressResult::WriteORA,               eMemoryAddressResult::MEMORY,                 eMemoryAddressResult::WriteORB,                 eMemoryAddressResult::MEMORY,
//  0x1C                                          0x1D                                          0x1E                                            0x1F
    eMemoryAddressResult::WriteTimerDiv1IRQOFF,   eMemoryAddressResult::WriteTimerDiv8IRQOFF,   eMemoryAddressResult::WriteTimerDiv64IRQOFF,    eMemoryAddressResult::WriteTimerDiv1024IRQOFF,
};

void createMemoryAnalysis()
{
  memset(debugRAM, 0, sizeof(debugRAM));
  for(uint16_t addr = 0; addr < 0x8000; addr++)
  {
    // RAM
    MemoryTypeRead(addr, eMemoryAddressResult::MEMORY);
    MemoryTypeWrite(addr, eMemoryAddressResult::MEMORY);
    MapAddress(addr, addr);
    // ROM
    MemoryTypeRead(addr + 0x8000, eMemoryAddressResult::MEMORY);
    MemoryTypeWrite(addr + 0x8000, eMemoryAddressResult::NOTHING);
    MapAddress(addr + 0x8000, addr + 0x8000);
  }

  // Alias SUPERMON ROM from 0xFxxx to 0x8xxx
  for(uint16_t addr = 0; addr < 0x1000; addr++)
      MapAddress(addr + 0xF000, addr + 0x8000);

  // SY6532 RAM
  for(uint16_t addr = 0xA600; addr < 0xA800; addr++)
  {
    MemoryTypeRead(addr, eMemoryAddressResult::MEMORY); // Includes Aliases
    MemoryTypeWrite(addr, eMemoryAddressResult::WriteProtect); // Includes Aliases
    MapAddress(addr, addr & 0xFE7F);
  }

  for(uint16_t addr = 0xA400; addr < 0xA600; addr++)
  {
    MemoryTypeRead(addr, readFunctions[addr & 0x1F]);
    MemoryTypeWrite(addr, writeFunctions[addr & 0x1F]);
  }

  // CRT I/O
  MemoryTypeRead(0x8B71, eMemoryAddressResult::SelectCRT);
  MemoryTypeRead(0x8AFF, eMemoryAddressResult::BaudCRT);

  MemoryTypeRead(0x8A58, eMemoryAddressResult::ReadCRT);     // INTCHR
  MemoryTypeRead(0xA660, eMemoryAddressResult::ReadCRT);     // INVEC

  MemoryTypeRead(0x8A47, eMemoryAddressResult::WriteCRT);    // OUTCHR 
  MemoryTypeRead(0xA663, eMemoryAddressResult::WriteCRT);    // OUTVEC

  MemoryTypeRead(0x8B3C, eMemoryAddressResult::TSTAT_CRT);

  MemoryTypeRead(0x8C78, eMemoryAddressResult::LoadTape);
  MemoryTypeRead(0x8C7B, eMemoryAddressResult::LoadTape);
  MemoryTypeRead(0x8D52, eMemoryAddressResult::IgnoreSubroutine);  // SYNC

  MemoryTypeRead(0x8E87, eMemoryAddressResult::SaveTape); // DUMPT

  MemoryTypeRead(0x9FFF, eMemoryAddressResult::SetGPIO25LED);
}

void __not_in_flash_func(indicate)(bool fValue)
{
  gpio_put(SYNCPin, fValue);
  if(!UserPin25LED)
    gpio_put(TimingIndicatorPin, !fValue);
}
#ifdef _SPEEDTEST_
  void createSpeedTest()
  {
    blank();
    Memory[NMIVECTOR & 0x8FFF] = Memory[RESETVECTOR & 0x8FFF] = Memory[IRQBRKVECTOR & 0x8FFF] = 0x00;
    Memory[(NMIVECTOR & 0x8FFF) + 1] = Memory[(RESETVECTOR & 0x8FFF) + 1] = Memory[(IRQBRKVECTOR & 0x8FFF) + 1] = 0x80;
    Memory[0x8000] = 0xFF;
    Memory[0x8001] = _SPEEDTEST_;
  }
#endif

void setup() 
{
    for(int i = 0; i < gpioInitCount; i++)
    {
        if(gpioInit[i])
          gpio_init(i);
    }
    gpio_set_dir(RESETPin, GPIO_IN);
    gpio_pull_up(RESETPin);

    gpio_set_dir(NMIPin, GPIO_IN);
    gpio_pull_up(NMIPin);

    gpio_set_dir(IRQPin, GPIO_IN);
    gpio_pull_up(IRQPin);

    gpio_set_dir(TimingIndicatorPin, GPIO_OUT);
    gpio_set_dir(SYNCPin, GPIO_OUT);

    setCycleCount(0x00FFFFFF);
    
    indicate(true);

    for(int i = 0; i < 16; i++)
        gpio_pull_up(i);
#ifdef _SPEEDTEST_
    createSpeedTest();
#else
    copyArrayToMemory(symon1_1, 0x8000, symon1_1Size);
    copyArrayToMemory(BASIC1_1, 0xC000, BASIC1_1Size);
    copyArrayToMemory(BasicTrig, 0x9EC7, BasicTrigSize);
    copyArrayToMemory(RAE_B000, 0xB000, RAE_B000Size);
    copyArrayToMemory(RAE_E000, 0xE000, RAE_E000Size);
#endif

    createMemoryAnalysis();
    PowerOn = true;

    reset();
    indicate(true);

    SystemRAMWriteProtected = false;
    RAMPage1WriteProtected = false;
    RAMPage2WriteProtected = false;
    RAMPage3WriteProtected = false;

    Configuration.Load();

    DebuggerActive = false;
    DebuggerOn = false;

    if(Configuration.UseInternalFlash)
    	CurrentTape.Path.SetDevice(eDevice::Flash);
    else if(Configuration.UseExternalSD)
    	CurrentTape.Path.SetDevice(eDevice::SD);

    CurrentTape.LoadRoot();
    
    if(gpio_get(NMIPin) == 0)  // DEBUG switched on
        CommandLine();

    PowerOn = false;
}

uint32_t lastCycleCount;
uint32_t cycleCount;

void __not_in_flash_func(Loop)(void) 
{
  setCycleCount(0x00FFFFFF);
  indicate(true);
  StartTiming = 0x00FFFFFF;
  SingleStep();
  indicate(false);
  ActualTiming = systick_hw->cvr;
  uint32_t desiredCycles = (uint32_t)(nrCycles * 150) - 80;
  uint32_t actualTime = StartTiming - ActualTiming;
  SkipDelay = (actualTime > desiredCycles);
  RemainingCycles = desiredCycles - actualTime;
    
  if(!SkipDelay)
    setCycleCount(RemainingCycles);
  lastCycleCount = 0x7FFFFFFFL;

#ifndef FULLSPEED
  if(!SkipDelay)
  {
    while(true)
    {
      cycleCount = systick_hw->cvr;
      if(cycleCount >= lastCycleCount)  // = test breaks if counter is frozen
        break;

      lastCycleCount = cycleCount;
    }
  }
#endif
}

char typeText[25][8] = { 
                        "Mem",
                        "N/A",
                        "ORA",
                        "ORA",
                        "ORB",
                        "ORB",
                        "DDRA",
                        "DDRB",
                        "T1ON",
                        "T8ON",
                        "T64ON",
                        "TKON",
                        "T1OFF",
                        "T8OFF",
                        "T64OFF",
                        "TKOFF",
                        "T?ON",
                        "T?OFF",
                        "P7OFF-",
                        "P7OFF+",
                        "P7ON-",
                        "P7ON+",
                        "RdIRQ",
                        "CRT",
                        "CRT"
                     };

void dumpMapPage(uint16_t fStart, uint16_t fSize)
{
  printf("\r\nAddress Mapping");
  for(uint16_t i = 0; i < fSize; i += 32)
  {
    printf("\r\n%04X: ", fStart + i);
    for(uint16_t j = 0; j < 32; j++)
    {
      if(MapAddress(fStart + i + j) == fStart + i + j)
        printf("  -  ");
      else
        printf("%04X ", MapAddress(fStart + i + j));
    }
  }
  printf("\r\n\r\nRead Type");
  for(uint16_t i = 0; i < fSize; i += 16)
  {
    printf("\r\n%04X: ", fStart + i);
    for(uint16_t j = 0; j < 16; j++)
      printf("%6s ", typeText[MemoryTypeRead(fStart + i + j)]);
  }
  printf("\r\n\r\nWrite Type");
  for(uint16_t i = 0; i < fSize; i += 16)
  {
    printf("\r\n%04X: ", fStart + i);
    for(uint16_t j = 0; j < 16; j++)
      printf("%6s ", typeText[MemoryTypeWrite(fStart + i + j)]);
  }
  CRLF();
}

void copyArrayToMemory(const uint8_t* fArray, uint16_t fMemoryAddress, int fSize)
{
  memcpy(Memory + fMemoryAddress, fArray, (size_t)fSize);
}

void __not_in_flash_func(reset)(void)
{
    uint16_t addr = PowerOn ? RESETVECTOR : SYSRAM_RESETVECTOR;

    switch (testAddress(&addr, 0))
    {
        case eMemoryAddressResult::MEMORY:
            PC = get16(addr);
            SR.Bits.I = 1;
            SR.Bits.One = 1;
            break;
     
        default:
            break;
     }
}

void __not_in_flash_func(nmi)(uint16_t fInstructionPC)
{
    bool nmiState = gpio_get(NMIPin);

    if((nmiState == 0) && (lastNMIState == 1))
    {
        // Debug NMI is disabled while running SUPERMON
        if((fInstructionPC & 0xF000) != 0x8000)
        {
            push(PC);
            push(SR.Value);
            SR.Bits.I = 1;
            PC = get16(PowerOn ? NMIVECTOR : SYSRAM_NMIVECTOR);
            lastNMIState = 1;
        }
    }
    else
        lastNMIState = nmiState;
}

void __not_in_flash_func(irq)(void)
{
    if(!SR.Bits.I && (SY6532_TimerIRQRequest || (SY6532_PA7IRQRequest && SY6532_PA7Triggered) || gpio_get(IRQPin) == 0))
    {
      printf("\r\nIRQ\r\n");
        push(PC);
        SR.Bits.B = 0;
        push(SR.Value);
        SR.Bits.I = 1;
        PC = get16(PowerOn ? IRQBRKVECTOR : SYSRAM_IRQBRKVECTOR);
    }
}

int main()
{
    stdio_init_all();
    setup();

    while (true)
        Loop();
}

void CRLF()
{
  printf("\r\n");
}
