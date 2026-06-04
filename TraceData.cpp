#include "TraceData.hpp"


const uint8_t operands[256] =
{ //  0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    Imp, InX, Val, Val, Val, Zpg, Zpg, Val, Imp, Imm, Acc, Val, Val, Abs, Abs, Val, // 0
    Rel, InY, Val, Val, Val, ZpX, ZpX, Val, Imp, AbY, Val, Val, Val, AbX, AbX, Val, // 1
    Abs, InX, Val, Val, Zpg, Zpg, Zpg, Val, Imp, Imm, Acc, Val, Abs, Abs, Abs, Val, // 2
    Rel, InY, Val, Val, Val, ZpX, ZpX, Val, Imp, AbY, Val, Val, Val, AbX, AbX, Val, // 3
    Imp, InX, Val, Val, Val, Zpg, Zpg, Val, Imp, Imm, Acc, Val, Abs, Abs, Abs, Val, // 4
    Rel, InY, Val, Val, Val, ZpX, ZpX, Val, Imp, AbY, Val, Val, Val, AbX, AbX, Val, // 5
    Imp, InX, Val, Val, Val, Zpg, Zpg, Val, Imp, Imm, Acc, Val, Ind, Abs, Abs, Val, // 6
    Rel, InY, Val, Val, Val, ZpX, ZpX, Val, Imp, AbY, Val, Val, Val, AbX, AbX, Val, // 7
    Val, InX, Val, Val, Zpg, Zpg, Zpg, Val, Imp, Val, Imp, Val, Abs, Abs, Abs, Val, // 8
    Rel, InY, Val, Val, ZpX, ZpX, ZpY, Val, Imp, AbY, Imp, Val, Val, AbX, Val, Val, // 9
    Imm, InX, Imm, Val, Zpg, Zpg, Zpg, Val, Imp, Imm, Imp, Val, Abs, Abs, Abs, Val, // A
    Rel, InY, Val, Val, ZpX, ZpX, ZpY, Val, Imp, AbY, Imp, Val, AbX, AbX, AbY, Val, // B
    Imm, InX, Val, Val, Zpg, Zpg, Zpg, Val, Imp, Imm, Imp, Val, Abs, Abs, Abs, Val, // C
    Rel, InY, Val, Val, Val, ZpX, ZpX, Val, Imp, AbY, Val, Val, Val, AbX, AbX, Val, // D
    Imm, InX, Val, Val, Zpg, Zpg, Zpg, Val, Imp, Imm, Imp, Val, Abs, Abs, Abs, Val, // E
    Rel, InY, Val, Val, Val, ZpX, ZpX, Val, Imp, AbY, Val, Val, Val, AbX, AbX, Imm  // F
};

void TTraceData::Snapshot(uint16_t fAddress)
{
    uint16_t addr = MapAddress(fAddress); // addr points to opcode, PC points to next byte
    
    ObjectCode[0] = getOperand8(addr);
    ObjectCode[1] = getOperand8(addr + 1);
    ObjectCode[2] = getOperand8(addr + 2);

    PC = fAddress;
    A = ::A;
    X = ::X;
    Y = ::Y;
    SP = ::SP;
    SR = ::SR;

    Flags = 0x00;

    switch(operands[ObjectCode[0]])
    {
      case Imp:
      case Val:
      case Imm:
      case Acc:
      case Rel:
        Flags = 0x00;
        Address = 0xFFFF;
        Data = 0xFF;
        break;
      case Zpg:
        Flags = SHOW_PARAM;
        Address = ObjectCode[1];
        Data = get8NoSideEffect(Address);
        break;
      case ZpX:
        Flags = SHOW_PARAM;
        Address = (ObjectCode[1] + X) & 0xFF;
        Data = get8NoSideEffect(Address);
        break;
      case ZpY:
        Flags = SHOW_PARAM;
        Address = (ObjectCode[1] + Y) & 0xFF;
        Data = get8NoSideEffect(Address);
        break;
      case Abs:
        if((ObjectCode[0] == 0x20) || (ObjectCode[0] == 0x4C))  // Ignore for JSR and JMP
            break;
        Flags = SHOW_PARAM;
        Address = ObjectCode[1] | (ObjectCode[2] << 8);
        Data = get8NoSideEffect(Address);
        break;
      case AbX:
        Flags = SHOW_PARAM;
        Address = (ObjectCode[1] | (ObjectCode[2] << 8)) + X;
        Data = get8NoSideEffect(Address);
        break;
      case AbY:
        Flags = SHOW_PARAM;
        Address = (ObjectCode[1] | (ObjectCode[2] << 8)) + Y;
        Data = get8NoSideEffect(Address);
        break;
      case Ind:
        Flags = SHOW_PARAM + _16BITDATA;
        Address = ObjectCode[1] | (ObjectCode[2] << 8);
        Data = getOperand16(Address);
        break;
      case InX:
        Flags = SHOW_PARAM;
        Address = getOperand16((ObjectCode[1] + X) & 0xFF);
        Data = get8NoSideEffect(Address);
        break;
      case InY:
        Flags = SHOW_PARAM;
        Address = getOperand16(ObjectCode[1]) + Y;
        Data = get8NoSideEffect(Address);
        break;
    }
}

std::string TTraceData::RegisterString(int fPCAdjust)
{
    std::string result;
    char memory[33];

    result.resize(200);
    memory[0] = 0;

    if(Flags & SHOW_PARAM)
    {
        if(Flags & _16BITDATA)
            sprintf(memory, " Addr %04X = %04X", Address, Data);
        else
            sprintf(memory, " Addr %04X = %02X", Address, Data);
    }
    sprintf(&result[0], "PC = %04X, A = %02X, X = %02X, Y = %02X, SP = 01%02X, SR = %s%s1%s%s%s%s%s%s", PC + fPCAdjust, A, X, Y, SP,
        SR.Bits.N ? "N" : "n", SR.Bits.V ? "V" : "v", SR.Bits.B ? "B" : "b", SR.Bits.D ? "D" : "d", SR.Bits.I ? "I" : "i", SR.Bits.Z ? "Z" : "z", SR.Bits.C ? "C" : "c", memory);  
    
    return result;
}

void TTraceData::PrintTraceData(char *fInstructionText)
{
    Disassemble(PC, fInstructionText);
    std::string text(fInstructionText);

    text.append(48 - strlen(fInstructionText), ' ');
    text.append(RegisterString());
    printf("%s\r\n", text.c_str());
}

