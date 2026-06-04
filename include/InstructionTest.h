#pragma once

#include "CommandLine.h"

// uint16_t absolute(uint16_t fAddr);
// uint16_t absX(uint16_t fAddr);
// uint16_t absXNoPC(uint16_t fAddr);
// uint16_t absY(uint16_t fAddr);
// uint8_t  imm(uint16_t fAddr);
// uint16_t indJMP(uint16_t fAddr);
// uint16_t indX(uint16_t fAddr);
// uint16_t indY(uint16_t fAddr);
// void rel(int fCondition, uint16_t fAddr);
// uint8_t zpg(uint16_t fAddr);
// uint8_t zpgX(uint16_t fAddr);
// uint16_t zpgY(uint16_t fAddr);

// void setNZ(uint8_t fValue);

void ADC(uint8_t fOperand);
void AND(uint8_t fOperand);
void ASL(uint16_t fAddr);
void ASLA();

void BCC(uint16_t fAddr);
void BCS(uint16_t fAddr);
void BEQ(uint16_t fAddr);
void BIT(uint8_t fOperand);
void BMI(uint16_t fAddr);
void BNE(uint16_t fAddr);
void BPL(uint16_t fAddr);
void BRK();
void BVC(uint16_t fAddr);
void BVS(uint16_t fAddr);

void CLC();
void CLD();
void CLI();
void CLV();
void CMP(uint8_t fOperand);
void CPX(uint8_t fOperand);
void CPY(uint8_t fOperand);

void DECrement(uint16_t fOperand);
void DEX();
void DEY();

void EOR(uint8_t fOperand);

void INC(uint16_t fOperand);
void INX();
void INY();

void JMP(uint16_t fAddr);
void JSR(uint16_t fAddr);

void LDA(uint8_t fOperand);
void LDX(uint8_t fOperand);
void LDY(uint8_t fOperand);
void LSR(uint16_t fOperand);
void LSRA();

void ORA(uint8_t fOperand);

void PHA();
void PHP();
void PLA();
void PLP();

void ROL(uint16_t fOperand);
void ROLA();
void ROR(uint16_t fOperand);
void RORA();
void RTI();
void RTS();

void SBC(uint8_t fOperand);
void SEC();
void SED();
void SEI();
void STA(uint16_t fAddr);
void STX(uint16_t fAddr);
void STY(uint16_t fAddr);

void TAX();
void TAY();
void TSX();
void TXA();
void TXS();
void TYA();

int  RunTests(void);
void blank();
void PrintStatus();
void printRegisters(int fPCAdjust = 0);
void stack();