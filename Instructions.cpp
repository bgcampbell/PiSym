#include "Instructions.h"
#include "InstructionTest.h"

const uint8_t baseCycleTimes[256] =
{ //0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
    7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0, // 0
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, // 1
    6, 5, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0, // 2
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, // 3
    6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0, // 4
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, // 5
    6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0, // 6
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, // 7
    0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0, // 8
    2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0, // 9
    2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0, // A
    2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0, // B
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, // C
    2, 5, 0, 0, 0, 4, 2, 0, 2, 4, 0, 0, 0, 4, 7, 0, // D
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, // E
    2, 5, 0, 0, 0, 4, 4, 0, 2, 4, 0, 0, 0, 4, 7, 0  // F
};

uint16_t InstructionPC;

// Stack Operations

void __not_in_flash_func(push)(uint8_t fValue)
{
    // Always in RAM
    set8((uint16_t)(0x0100 + SP--), fValue);
}
void __not_in_flash_func(push)(uint16_t fValue)
{
    set16((uint16_t)(0x00FF + SP), fValue);
    SP -= 2;
}
uint16_t __not_in_flash_func(popWord)()
{
    return popuint8_t() + ((uint16_t)popuint8_t() << 8);
}
uint8_t __not_in_flash_func(popuint8_t)()
{
    return get8((uint16_t)(0x0100 + ++SP));
}

// Addressing Modes

uint16_t __not_in_flash_func(absolute)(uint16_t fOperandAddr)
{
    uint16_t result = getOperand16(fOperandAddr);
    PC += 2;

    return result;
}
uint16_t __not_in_flash_func(absX)(uint16_t fOperandAddr)
{
   uint16_t result = getOperand16(fOperandAddr) + X;

   if((result & 0xFF) < X) // Page cross
        nrCycles++;
    PC += 2;

    return result;
}
uint16_t __not_in_flash_func(absXNoPC)(uint16_t fOperandAddr)
{
    uint16_t result = getOperand16(fOperandAddr) + X;

    PC += 2;

    return result;
}
uint16_t __not_in_flash_func(absY)(uint16_t fOperandAddr)
{
    uint16_t result = getOperand16(fOperandAddr) + Y;

    if((result & 0xFF) < Y) // Page cross
        nrCycles++;
    PC += 2;

    return result;
}
uint8_t __not_in_flash_func(imm)(uint16_t fOperandAddr)
{
    PC++;
    return get8(fOperandAddr);
}
uint16_t __not_in_flash_func(indJMP)(uint16_t fOperandAddr)
{
    // Simulate the bug
    uint16_t addr = getOperand16(fOperandAddr++);  // JMP ($1234) -> addr = 0x1234
    uint16_t lowByte = get8(addr++);

     if ((addr & 0xFF) == 0x00)
        addr -= 0x0100;

    uint16_t highByte = get8(addr);

    addr = lowByte | (highByte << 8);
    PC += 2;
    return addr;
}
uint16_t __not_in_flash_func(indX)(uint16_t fOperandAddr)
{
    uint16_t addr = (uint16_t)((getOperand8(fOperandAddr) + X) & 0xFF);
    PC++;
    return get16(addr);
}
uint16_t __not_in_flash_func(indY)(uint16_t fOperandAddr)
{
    uint16_t addr = get16(getOperand8(fOperandAddr)) + Y;

    PC++;
    if((addr & 0xFF) < Y)   // Page Cross
        nrCycles++;
    return addr;
}
void __not_in_flash_func(rel)(int fCondition, uint16_t fOperandAddr)
{
    uint8_t distance = getOperand8(fOperandAddr);

    PC++;
    if (fCondition)
    {
        uint16_t oldPC = PC;

        nrCycles++;
        if (distance > 0x7F)
            PC -= (uint16_t)(0x100 - distance);
        else
            PC += distance;
        if((oldPC & 0xFF00) != (PC & 0xFF00))    // Page cross
            nrCycles++;
    }
}
uint16_t __not_in_flash_func(zpg)(uint16_t fOperandAddr)
{
    PC++;
    return (uint8_t)(getOperand8(fOperandAddr));
}
uint16_t __not_in_flash_func(zpgX)(uint16_t fOperandAddr)
{
    PC++;
    return (uint8_t)(getOperand8(fOperandAddr) + X);
}
uint16_t __not_in_flash_func(zpgY)(uint16_t fOperandAddr)
{
    PC++;
    return (uint8_t)(getOperand8(fOperandAddr) + Y);
}

// Status Register

void __not_in_flash_func(setNZ)(uint8_t fValue)
{
    SR.Bits.N = (fValue & 0x80) != 0x00;
    SR.Bits.Z = fValue == 0;
}

// Instructions

void __not_in_flash_func(ADC)(uint8_t fOperand)
{
    int result;

    if (SR.Bits.D)
    {
        result = (A & 0x0F) + (fOperand & 0x0F) + SR.Bits.C;
        if (result > 0x09)
            result += 0x06;
        result += (A & 0xF0) + (fOperand & 0xF0);

        if(result > 0x9F)
            result += 0x60;
    }
    else
        result = (A + fOperand + SR.Bits.C);
    
    SR.Bits.V = (~(A ^ fOperand) & (A ^ result) & 0x80) != 0;
    SR.Bits.C = result > 0xFF;
    setNZ(A = result);
}
void __not_in_flash_func(AND)(uint8_t fOperand)
{
    setNZ(A &= fOperand);
}
void __not_in_flash_func(ASL)(uint16_t fAddr)
{
    uint8_t value = get8(fAddr);

    SR.Bits.C = (value & 0x80) != 0;
    setNZ(set8(fAddr, value << 1));
}
void __not_in_flash_func(ASLA)(void)
{
    SR.Bits.C = (A & 0x80) != 0;
    setNZ(A <<= 1);
}
void __not_in_flash_func(BCC)(uint16_t fOperandAddr)
{
    rel(!SR.Bits.C, fOperandAddr);
}
void __not_in_flash_func(BCS)(uint16_t fOperandAddr)
{
    rel(SR.Bits.C, fOperandAddr);
}
void __not_in_flash_func(BEQ)(uint16_t fOperandAddr)
{
    rel(SR.Bits.Z, fOperandAddr);
}
void __not_in_flash_func(BIT)(uint8_t fOperand)
{
    uint8_t testValue = (uint8_t)(A & fOperand);

    setNZ(testValue);
    SR.Bits.V = (fOperand & 0x40) != 0;
    SR.Bits.N = (fOperand & 0x80) != 0;
}
void __not_in_flash_func(BMI)(uint16_t fOperandAddr)
{
    rel(SR.Bits.N, fOperandAddr);
}
void __not_in_flash_func(BNE)(uint16_t fOperandAddr)
{
    rel(!SR.Bits.Z, fOperandAddr);
}
void __not_in_flash_func(BPL)(uint16_t fOperandAddr)
{
    rel(!SR.Bits.N, fOperandAddr);
}
void __not_in_flash_func(BRK)(void)
{
    push((uint16_t)(PC + 2)); // Allows extra uint8_t for parameter
    SR.Bits.B = 1;    // I is not set automatically
    push(SR.Value);
    PC = get16(SYSRAM_IRQBRKVECTOR);
}
void __not_in_flash_func(BVC)(uint16_t fOperandAddr)
{
    rel(!SR.Bits.V, fOperandAddr);
}
void __not_in_flash_func(BVS)(uint16_t fOperandAddr)
{
    rel(SR.Bits.V, fOperandAddr);
}
void __not_in_flash_func(CLC)(void)
{
    SR.Bits.C = 0;
}
void __not_in_flash_func(CLD)(void)
{
    SR.Bits.D = 0;  
}
void __not_in_flash_func(CLI)(void)
{
    SR.Bits.I = 0;
}
void __not_in_flash_func(CLV)(void)
{            
    SR.Bits.V = 0;
}
void __not_in_flash_func(compare)(uint8_t fReg, uint8_t fOperand)
{
    SR.Bits.C = fReg >= fOperand;
    setNZ((uint8_t)(fReg - fOperand));
}
void __not_in_flash_func(CMP)(uint8_t fOperand)
{
    compare(A, fOperand);
}
void __not_in_flash_func(CPX)(uint8_t fOperand)
{
    compare(X, fOperand);
}
void __not_in_flash_func(CPY)(uint8_t fOperand)
{
    compare(Y, fOperand);
}
void __not_in_flash_func(DECrement)(uint16_t fAddr)
{
    setNZ(set8(fAddr, get8(fAddr) - 1));
}
void __not_in_flash_func(DEX)(void)
{
    setNZ(--X);
}
void __not_in_flash_func(DEY)(void)
{
    setNZ(--Y);
}
void __not_in_flash_func(EOR)(uint8_t fOperand)
{
    setNZ(A ^= fOperand);
}
void __not_in_flash_func(INC)(uint16_t fAddr)
{
    setNZ(set8(fAddr, get8(fAddr) + 1));
}
void __not_in_flash_func(INX)(void)
{
    setNZ(++X);
}
void __not_in_flash_func(INY)(void)
{
    setNZ(++Y);
}
void __not_in_flash_func(JMP)(uint16_t fAddr)
{
    PC = fAddr;
}
void __not_in_flash_func(JSR)(uint16_t fAddr)
{
    push((uint16_t)(PC - 1));
    PC = fAddr;
}
void __not_in_flash_func(LDA)(uint8_t fValue)
{
    setNZ(A = fValue);
}
void __not_in_flash_func(LDX)(uint8_t fValue)
{
    setNZ(X = fValue);
}
void __not_in_flash_func(LDY)(uint8_t fValue)
{
    setNZ(Y = fValue);
}
void __not_in_flash_func(LSR)(uint16_t fAddr)
{
    uint8_t value = get8(fAddr);

    SR.Bits.C = value & 0x01;
    setNZ(set8(fAddr, value >> 1));
}
void __not_in_flash_func(LSRA)(void)
{
    SR.Bits.C = A & 0x01;
    setNZ(A >>= 1);
}
void __not_in_flash_func(ORA)(uint8_t fValue)
{
    setNZ(A |= fValue);
}
void __not_in_flash_func(PHA)(void)
{
    SP_Location--;
    push(A);
}
void __not_in_flash_func(PHP)(void)
{
    SR.Bits.One = 1;
    SP_Location--;
    push(SR.Value);
}
void __not_in_flash_func(PLA)(void)
{
    SP_Location++;
    setNZ(A = popuint8_t());
}
void __not_in_flash_func(PLP)(void)
{
    SP_Location++;
    SR.Value = popuint8_t();
}
void __not_in_flash_func(ROL)(uint16_t fAddr)
{
    int oldC = SR.Bits.C;
    uint8_t value = get8(fAddr);

    SR.Bits.C = (value & 0x80) != 0x00;
    setNZ(set8(fAddr, (value << 1) | oldC));
}
void __not_in_flash_func(ROLA)(void)
{
    int oldC = SR.Bits.C;

    SR.Bits.C = (A & 0x80) != 0x00;
    setNZ(A = (A << 1) | oldC);
}
void __not_in_flash_func(ROR)(uint16_t fAddr)
{
    int oldC = SR.Bits.C;
    uint8_t value = get8(fAddr);

    SR.Bits.C = value & 0x01;
    setNZ(set8(fAddr, (value >> 1) | (oldC ? 0x80 : 0x00)));
}
void __not_in_flash_func(RORA)(void)
{
    int oldC = SR.Bits.C;

    SR.Bits.C = A & 0x01;
    setNZ(A = (A >> 1) | (oldC ? 0x80 : 0));
}
void __not_in_flash_func(RTI)(void)
{
    SR.Value = popuint8_t();
    PC = popWord();
}
void __not_in_flash_func(RTS)(void)
{
    PC = popWord() + 1;
}
void __not_in_flash_func(SBC)(uint8_t fOperand)
{
    int result;
    uint8_t invertedOperand = ~fOperand;

    if (SR.Bits.D)
    {
        int HC = ((A & 0x0F) + (invertedOperand & 0x0F) + SR.Bits.C) > 0x0F;

        result = (int)A + (int)invertedOperand + (int)SR.Bits.C;
        SR.Bits.C  = result > 0xFF;

        if(!HC)
            result -= 0x06;
            
        if(!SR.Bits.C)
            result -= 0x60;
        result &= 0xFF;
    }
    else
    {
        result = (int)A - (int)fOperand - (int)!SR.Bits.C;
        SR.Bits.C = result >= 0;
    }
    SR.Bits.V = (~(A ^ invertedOperand) & (A ^ (uint8_t)result) & 0x80) != 0;
    setNZ(A = result);
}
void __not_in_flash_func(SEC)(void)
{
    SR.Bits.C = 1;
}
void __not_in_flash_func(SED)(void)
{
    SR.Bits.D = 1;  
}
void __not_in_flash_func(SEI)(void)
{
    SR.Bits.I = 1;  
}
void __not_in_flash_func(STA)(uint16_t fAddr)
{
    set8(fAddr, A);
}
void __not_in_flash_func(STX)(uint16_t fAddr)
{
    set8(fAddr, X);
}
void __not_in_flash_func(STY)(uint16_t fAddr)
{
    set8(fAddr, Y);
}
void __not_in_flash_func(TAX)(void)
{
    X = A;
    setNZ(X);
}
void __not_in_flash_func(TAY)(void)
{
    Y = A;
    setNZ(Y);
}
void __not_in_flash_func(TSX)(void)
{
    X = SP;
    setNZ(X);
}
void __not_in_flash_func(TXA)(void)
{
    A = X;
    setNZ(A);
}
void __not_in_flash_func(TXS)(void)
{
    SP = X;
}
void __not_in_flash_func(TYA)(void)
{
    A = Y;
    setNZ(A);
}

uint8_t __not_in_flash_func(SingleStep)(void)
{
    InstructionPC = PC;

    uint16_t addr = MapAddress(PC++); // addr points to opcode, PC points to next byte
    uint8_t opcode = get8(&addr, true);

    nrCycles = baseCycleTimes[opcode];

    switch (opcode)
    {
        case 0x00:  // BRK
            BRK();
            break;
        case 0x01:  // ORA (ind,X)
            ORA(get8(indX(addr + 1)));
            break;
        case 0x05:  // ORA zpg
            ORA(get8(zpg(addr + 1)));
            break;
        case 0x06:  // ASL zpg
            ASL(zpg(addr + 1));
            break;
        case 0x08:  // PHP
            PHP();
            break;
        case 0x09:  // ORA #
            ORA(imm(addr + 1));
            break;
        case 0x0a:  // ASL A
            ASLA();
            break;
        case 0x0d:  // ORA abs
            ORA(get8(absolute(addr + 1)));
            break;
        case 0x0e:  // ASL abs
            ASL(absolute(addr + 1));
            break;
        case 0x10:  // BPL
            BPL(addr + 1);
            break;
        case 0x11:  // ORA (ind),Y
            ORA(get8(indY(addr + 1)));
            break;
        case 0x15:  // ORA zpg,X
            ORA(get8(zpgX(addr + 1)));
            break;
        case 0x16:  // ASL zpg,X
            ASL(zpgX(addr + 1));
            break;
        case 0x18:  // CLC
            CLC();
            break;
        case 0x19:  // ORA abs,Y
            ORA(get8(absY(addr + 1)));
            break;
        case 0x1d:  // ORA abs,X
            ORA(get8(absX(addr + 1)));
            break;
        case 0x1e:  // ASL abs,X
            ASL(absXNoPC(addr + 1));
            break;
        case 0x20:  // JSR abs
            JSR(absolute(addr + 1));
            break;
        case 0x21:  // AND (ind),Y
            AND(get8(indY(addr + 1)));
            break;
        case 0x24:  // BIT zpg
            BIT(get8(zpg(addr + 1)));
            break;
        case 0x25:  // AND zpg
            AND(get8(zpg(addr + 1)));
            break;
        case 0x26:  // ROL zpg
            ROL(zpg(addr + 1));
            break;
        case 0x28:  // PLP
            PLP();
            break;
        case 0x29:  // AND #
            AND(imm(addr + 1));
            break;
        case 0x2a:  // ROL A
            ROLA();
            break;
        case 0x2c:  // BIT abs
            BIT(get8(absolute(addr + 1)));
            break;
        case 0x2d:  // AND abs
            AND(get8(absolute(addr + 1)));
            break;
        case 0x2e:  // ROL abs
            ROL(absolute(addr + 1));
            break;
        case 0x30:  // BMI
            BMI(addr + 1);
            break;
        case 0x31:  // AND (ind),Y
            AND(get8(indY(addr + 1)));
            break;
        case 0x35:  // AND zpg,X
            AND(get8(zpgX(addr + 1)));
            break;
        case 0x36:  // ROL zpg,X
            ROL(zpgX(addr + 1));
            break;
        case 0x38:  // SEC
            SEC();
            break;
        case 0x39:  // AND abs,Y
            AND(get8(absY(addr + 1)));
            break;
        case 0x3d:  // AND abs,X
            AND(get8(absX(addr + 1)));
            break;
        case 0x3e:  // ROL abs,X
            ROL(absXNoPC(addr + 1));
            break;
        case 0x40:  // RTI
            RTI();
            break;
        case 0x41:  // EOR (ind,X)
            EOR(get8(indX(addr + 1)));
            break;
        case 0x45:  // EOR zpg
            EOR(get8(zpg(addr + 1)));
            break;
        case 0x46:  // LSR zpg
            LSR(zpg(addr + 1));
            break;
        case 0x48:  // PHA
            PHA();
            break;
        case 0x49:  // EOR #
            EOR(imm(addr + 1));
            break;
        case 0x4a:  // LSR A
            LSRA();
            break;
        case 0x4c:  // JMP abs
            JMP(absolute(addr + 1));
            break;
        case 0x4d:  // EOR abs
            EOR(get8(absolute(addr + 1)));
            break;
        case 0x4e:  // LSR abs
            LSR(absolute(addr + 1));
            break;
        case 0x50:  // BVC
            BVC(addr + 1);
            break;
        case 0x51:  // EOR (ind),Y
            EOR(get8(indY(addr + 1)));
            break;
        case 0x55:  // EOR zpg,X
            EOR(get8(zpgX(addr + 1)));
            break;
        case 0x56:  // LSR zpg,X
            LSR(zpgX(addr + 1));
            break;
        case 0x58:  // CLI
            CLI();
            break;
        case 0x59:  // EOR abs,Y
            EOR(get8(absY(addr + 1)));
            break;
        case 0x5d:  // EOR abs,X
            EOR(get8(absX(addr + 1)));
            break;
        case 0x5e:  // LSR abs,X
            LSR(absXNoPC(addr + 1));
            break;
        case 0x60:  // RTS
            RTS();
            break;
        case 0x61:  // ADC (ind,X)
            ADC(get8(indX(addr + 1)));
            break;
        case 0x65:  // ADC zpg
            ADC(get8(zpg(addr + 1)));
            break;
        case 0x66:  // ROR zpg
            ROR(zpg(addr + 1));
            break;
        case 0x68:  // PLA
            PLA();
            break;
        case 0x69:  // ADC #
            ADC(imm(addr + 1));
            break;
        case 0x6a:  // ROR A
            RORA();
            break;
        case 0x6c:  // JMP (ind)
            JMP(indJMP(addr + 1));
            break;
        case 0x6d:  // ADC abs
            ADC(get8(absolute(addr + 1)));
            break;
        case 0x6e:  // ROR abs
            ROR(absolute(addr + 1));
            break;
        case 0x70:  // BVS
            BVS(addr + 1);
            break;
        case 0x71:  // ADC (ind),Y
            ADC(get8(indY(addr + 1)));
            break;
        case 0x75:  // ADC zpg,X
            ADC(get8(zpgX(addr + 1)));
            break;
        case 0x76:  // ROR zpg,X
            ROR(zpgX(addr + 1));
            break;
        case 0x78:  // SEI
            SEI();
            break;
        case 0x79:  // ADC abs,Y
            ADC(get8(absY(addr + 1)));
            break;
        case 0x7d:  // ADC abs,X
            ADC(get8(absX(addr + 1)));
            break;
        case 0x7e:  // ROR abs,X
            ROR(absXNoPC(addr + 1));
            break;
        case 0x81:  // STA (ind,X)
            STA(indX(addr + 1));
            break;
        case 0x84:  // STY zpg
            STY(zpg(addr + 1));
            break;
        case 0x85:  // STA zpg
            STA(zpg(addr + 1));
            break;
        case 0x86:  // STX zpg
            STX(zpg(addr + 1));
            break;
        case 0x88:  // DEY
            DEY();
            break;
        case 0x8a:  // TXA
            TXA();
            break;
        case 0x8c:  // STY abs
            STY(absolute(addr + 1));
            break;
        case 0x8d:  // STA abs
            STA(absolute(addr + 1));
            break;
        case 0x8e:  // STX abs
            STX(absolute(addr + 1));
            break;
        case 0x90:  // BCC
            BCC(addr + 1);
            break;
        case 0x91:  // STA (ind),Y
            STA(indY(addr + 1));
            break;
        case 0x94:  // STY zpg,X
            STY(zpgX(addr + 1));
            break;
        case 0x95:  // STA zpg,X
            STA(zpgX(addr + 1));
            break;
        case 0x96:  // STX zpg,Y
            STY(zpgY(addr + 1));
            break;
        case 0x98:  // TYA
            TYA();
            break;
        case 0x99:  // STA abs,Y
            STA(absY(addr + 1));
            break;
        case 0x9a:  // TXS
            TXS();
            break;
        case 0x9d:  // STA abs,X
            STA(absX(addr + 1));
            break;
        case 0xa0:  // LDY #
            LDY(imm(addr + 1));
            break;
        case 0xa1:  // LDA (ind,X)
            LDA(get8(indX(addr + 1)));
            break;
        case 0xa2:  // LDX #
        // printf("\r\nSingleStep LDX : PC = %04X", PC);
            LDX(imm(addr + 1));
            break;
        case 0xa4:  // LDY zpg
            LDY(get8(zpg(addr + 1)));
            break;
        case 0xa5:  // LDA zpg
            LDA(get8(zpg(addr + 1)));
            break;
        case 0xa6:  // LDX zpg
            LDX(get8(zpg(addr + 1)));
            break;
        case 0xa8:  // TAY
            TAY();
            break;
        case 0xa9:  // LDA #
            LDA(imm(addr + 1));
            break;
        case 0xaa:  // TAX
            TAX();
            break;
        case 0xac:  // LDY abs
            LDY(get8(absolute(addr + 1)));
            break;
        case 0xad:  // LDA abs
            LDA(get8(absolute(addr + 1)));
            break;
        case 0xae:  // LDX abs
            LDX(get8(absolute(addr + 1)));
            break;
        case 0xb0:  // BCS
            BCS(addr + 1);
            break;
        case 0xb1:  // LDA (ind),Y
            LDA(get8(indY(addr + 1)));
            break;
        case 0xb4:  // LDY zpg,X
            LDY(get8(zpgX(addr + 1)));
            break;
        case 0xb5:  // LDA zpg,X
            LDA(get8(zpgX(addr + 1)));
            break;
        case 0xb6:  // LDX zpg,Y
            LDX(get8(zpgY(addr + 1)));
            break;
        case 0xb8:  // CLV
            CLV();
            break;
        case 0xb9:  // LDA abs,Y
            LDA(get8(absY(addr + 1)));
            break;
        case 0xba:  // TSX
            TSX();
            break;
        case 0xbc:  // LDY abs,X
            LDY(get8(absX(addr + 1)));
            break;
        case 0xbd:  // LDA abs,X
            LDA(get8(absX(addr + 1)));
            break;
        case 0xbe:  // LDX, abs,Y
            LDX(get8(absY(addr + 1)));
            break;
        case 0xc0:  // CPY #
            CPY(imm(addr + 1));
            break;
        case 0xc1:  // CMP (ind,X)
            CMP(get8(indX(addr + 1)));
            break;
        case 0xc4:  // CPY zpg
            CPY(get8(zpg(addr + 1)));
            break;
        case 0xc5:  // CMP zpg
            CMP(get8(zpg(addr + 1)));
            break;
        case 0xc6:  // DEC zpg
            DECrement(zpg(addr + 1));
            break;
        case 0xc8:  // INY
            INY();
            break;
        case 0xc9:  // CMP #
            CMP(imm(addr + 1));
            break;
        case 0xca:  // DEX
            DEX();
            break;
        case 0xcc:  // CPY abs
            CPY(get8(absolute(addr + 1)));
            break;
        case 0xcd:  // CMP abs
            CMP(get8(absolute(addr + 1)));
            break;
        case 0xce:  // DECrement abs
            DECrement(absolute(addr + 1));
            break;
        case 0xd0:  // BNE
            BNE(addr + 1);
            break;
        case 0xd1:  // CMP (ind),Y
            CMP(get8(indY(addr + 1)));
            break;
        case 0xd5:  // CMP zpg,X
            CMP(get8(zpgX(addr + 1)));
            break;
        case 0xd6:  // DECrement zpg,X
            DECrement(zpgX(addr + 1));
            break;
        case 0xd8:  // CLD
            CLD();
            break;
        case 0xd9:  // CMP abs,Y
            CMP(get8(absY(addr + 1)));
            break;
        case 0xdd:  // CMP abs,X
            CMP(get8(absX(addr + 1)));
            break;
        case 0xde:  // DECrement abs,X
            DECrement(absX(addr + 1));
            break;
        case 0xe0:  // CPX #
            CPX(imm(addr + 1));
            break;
        case 0xe1:  // SBC (ind,X)
            SBC(get8(indX(addr + 1)));
            break;
        case 0xe4:  // CPX zpg
            CPX(get8(zpg(addr + 1)));
            break;
        case 0xe5:  // SBC zpg
            SBC(get8(zpg(addr + 1)));
            break;
        case 0xe6:  // INC zpg
            INC(zpg(addr + 1));
            break;
        case 0xe8:  // INX
            INX();
            break;
        case 0xe9:  // SBC #
            SBC(imm(addr + 1));
            break;
        case 0xea:  // NOP
            break;
        case 0xec:  // CPX abs
            CPX(get8(absolute(addr + 1)));
            break;
        case 0xed:  // SBC abs
            SBC(get8(absolute(addr + 1)));
            break;
        case 0xee:  // INC abs
            INC(absolute(addr + 1));
            break;
        case 0xf0:  // BEQ
            BEQ(addr + 1);
            break;
        case 0xf1:  // SBC (ind),Y
            SBC(get8(indY(addr + 1)));
            break;
        case 0xf5:  // SBC zpg,X
            SBC(get8(zpgX(addr + 1)));
            break;
        case 0xf6:  // INC zpg,X
            INC(zpgX(addr + 1));
            break;
        case 0xf8:  // SED
            SED();
            break;
        case 0xf9:  // SBC abs,Y
            SBC(get8(absY(addr + 1)));
            break;
        case 0xfd:  // SBC abs,X
            SBC(get8(absX(addr + 1)));
            break;
        case 0xfe:  // INC abs,X
            INC(absX(addr + 1));
            break;
#ifdef _SPEEDTEST_
        case 0xff:  // SPEED TEST
            nrCycles = imm(addr + 1);
            PC -= 2;
            break;
#endif
        default:
            // printf("At start: %s\r\n", opcodeText);
            printf("************ Invalid Opcode %02X at PC %04X translated to %04X ************\r\n", opcode, PC - 1, addr);
            dumpMemory(addr & 0xFF00, 256);
            if(DebuggerActive)
                Trace();
            while(true);
            break;
    }

    if(gpio_get(RESETPin) == 0)
    {
        while(gpio_get(RESETPin) == 0);
        reset();
    }
    nmi(InstructionPC);
    irq();

    return opcode;
}
