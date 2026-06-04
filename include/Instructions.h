#pragma once

#include "IO.h"
#include "ReadWrite.h"

enum eMemoryAddressResult testAddress(uint16_t *fAddr, int fWrite);

extern uint16_t InstructionPC;

void push(uint8_t fValue);
void push(uint16_t fValue);
uint16_t popWord();
uint16_t absolute(uint16_t fOperandAddr);
uint16_t absX(uint16_t fOperandAddr);
uint16_t absXNoPC(uint16_t fOperandAddr);
uint16_t absY(uint16_t fOperandAddr);
uint8_t imm(uint16_t fOperandAddr);
uint16_t indJMP(uint16_t fOperandAddr);
uint16_t indX(uint16_t fOperandAddr);
uint16_t indY(uint16_t fOperandAddr);
void rel(int fCondition, uint16_t fOperandAddr);
uint16_t zpg(uint16_t fOperandAddr);
uint16_t zpgX(uint16_t fOperandAddr);
uint16_t zpgY(uint16_t fOperandAddr);
uint8_t popuint8_t();
void setNZ(uint8_t fValue);
