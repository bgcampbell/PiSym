#pragma once

#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include "PiSym.h"
#include "Disassembler.h"
#include "Instructions.h"
#include "DebugInfo.hpp"
#include "TraceData.hpp"
#include "ReadWrite.h"

#define FORMATLENGTH      64
#define TRACELIMIT      1000

enum eReg { RegA, RegX, RegY, RegSP, RegSR, Stack, Page0, SymonRAM };

enum eAccess { Read, Write, ReadWrite, Clear };

struct SBP
{
    uint16_t    Address;
    bool        OutputOnly;
};

struct SVariable
{
    uint16_t    Address;
    eAccess     Access;
    bool        Break;
};

struct SDebuggerDisable
{
    uint16_t StartAddress;
    uint16_t EndAddress;
};

extern bool DebuggerActive;
extern bool DebuggerOn;
extern std::vector<SDebuggerDisable>     DebuggerDisable;
extern uint8_t SP_Location;

void CommandLine(bool fColdStart = true);
void DoWatch(uint16_t fAddress, eAccess fAccess, uint8_t fValue);
void DoWatchWP(uint16_t fAddress, uint8_t fValue);
void DoWatch16(uint16_t fAddress, eAccess fAccess, uint16_t fValue);
void Trace();
