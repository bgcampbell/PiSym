#pragma once

#include "ReadWrite.h"

#define SHOW_PARAM  1
#define _16BITDATA  2

#define Imp 0
#define Acc 1
#define Imm 2
#define Zpg 3
#define ZpX 4
#define ZpY 5
#define Abs 6
#define AbX 7
#define AbY 8
#define Ind 9
#define InX 10
#define InY 11
#define Rel 12
#define Val 13

extern const uint8_t operands[];

class TTraceData
{
    public:
        uint8_t     ObjectCode[3];
        uint16_t    PC;
        uint8_t     A;
        uint8_t     X;
        uint8_t     Y;
        uint8_t     SP;
        USR         SR;
        uint8_t     Flags;
        uint16_t    Address;
        uint16_t    Data;

    public:
        void Snapshot(uint16_t fAddress);
        std::string RegisterString(int fPCAdjust = 0);
        void PrintTraceData(char *fInstructionText);
};
