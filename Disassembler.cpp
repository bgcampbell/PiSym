#include "Disassembler.h"

char result[LABELSIZE];

const char *getSymbolOperand(uint16_t fAddress)
{
  const char* label = Lookup(fAddress);

  if(label == NULL)
  {
    sprintf(result, "$%04X", fAddress);
    return result;
  }
  sprintf(result, "%s ($%04X)", label, fAddress);
  return result;
}
const char *mnemonics[256] =
{ //   0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F
    "BRK", "ORA", ".DB", ".DB", ".DB", "ORA", "ASL", ".DB", "PHP", "ORA", "ASL", ".DB", ".DB", "ORA", "ASL", ".DB", // 0
    "BPL", "ORA", ".DB", ".DB", ".DB", "ORA", "ASL", ".DB", "CLC", "ORA", ".DB", ".DB", ".DB", "ORA", "ASL", ".DB", // 1
    "JSR", "AND", ".DB", ".DB", "BIT", "AND", "ROL", ".DB", "PLP", "AND", "ROL", ".DB", "BIT", "AND", "ROL", ".DB", // 2
    "BMI", "AND", ".DB", ".DB", ".DB", "AND", "ROL", ".DB", "SEC", "AND", ".DB", ".DB", ".DB", "AND", "ROL", ".DB", // 3
    "RTI", "EOR", ".DB", ".DB", ".DB", "EOR", "LSR", ".DB", "PHA", "EOR", "LSR", ".DB", "JMP", "EOR", "LSR", ".DB", // 4
    "BVC", "EOR", ".DB", ".DB", ".DB", "EOR", "LSR", ".DB", "CLC", "EOR", ".DB", ".DB", ".DB", "EOR", "LSR", ".DB", // 5
    "RTS", "ADC", ".DB", ".DB", ".DB", "ADC", "ROR", ".DB", "PLA", "ADC", "ROR", ".DB", "JMP", "ADC", "ROR", ".DB", // 6
    "BVS", "ADC", ".DB", ".DB", ".DB", "ADC", "ROR", ".DB", "SEC", "ADC", ".DB", ".DB", ".DB", "ADC", "ROR", ".DB", // 7
    ".DB", "STA", ".DB", ".DB", "STY", "STA", "STX", ".DB", "DEY", ".DB", "TXA", ".DB", "STY", "STA", "STX", ".DB", // 8
    "BCC", "STA", ".DB", ".DB", "STY", "STA", "STX", ".DB", "TYA", "STA", "TXS", ".DB", ".DB", "STA", ".DB", ".DB", // 9
    "LDY", "LDA", "LDX", ".DB", "LDY", "LDA", "LDX", ".DB", "TAY", "LDA", "TAX", ".DB", "LDY", "LDA", "LDX", ".DB", // A
    "BCS", "LDA", ".DB", ".DB", "LDY", "LDA", "LDX", ".DB", "CLV", "LDA", "TSX", ".DB", "LDY", "LDA", "LDX", ".DB", // B
    "CPY", "CMP", ".DB", ".DB", "CPY", "CMP", "DEC", ".DB", "INY", "CMP", "DEX", ".DB", "CPY", "CMP", "DEC", ".DB", // C
    "BNE", "CMP", ".DB", ".DB", ".DB", "CMP", "DEC", ".DB", "CLD", "CMP", ".DB", ".DB", ".DB", "CMP", "DEC", ".DB", // D
    "CPX", "SBC", ".DB", ".DB", "CPX", "SBC", "INC", ".DB", "INX", "SBC", "NOP", ".DB", "CPX", "SBC", "INC", ".DB", // E
    "BEQ", "SBC", ".DB", ".DB", ".DB", "SBC", "INC", ".DB", "SED", "SBC", ".DB", ".DB", ".DB", "SBC", "INC", "DLY"  // F
};

TTraceData Disassemble(uint16_t fAddress, char *fInstructionText)
{
  TTraceData traceData;

  traceData.Snapshot(fAddress);
  char temp[33];
  int opcode = traceData.ObjectCode[0];
  uint16_t dest;
  const char *label;

  sprintf(fInstructionText, "%04X  ", fAddress);
  switch(operands[opcode])
  {
    case Imp:
    case Acc:
    case Val:
      sprintf(temp, "%02X        ", traceData.ObjectCode[0]);
      strcat(fInstructionText, temp);
      break;
    case Imm:
    case Zpg:
    case ZpX:
    case ZpY:
    case InX:
    case InY:
    case Rel:
      sprintf(temp, "%02X %02X     ", traceData.ObjectCode[0], traceData.ObjectCode[1]);
      strcat(fInstructionText, temp);
      break;
    case Abs:
    case AbX:
    case AbY:
    case Ind:
      sprintf(temp, "%02X %02X %02X  ", traceData.ObjectCode[0], traceData.ObjectCode[1], traceData.ObjectCode[2]);
      strcat(fInstructionText, temp);
      break;
  }

  sprintf(temp, "%8s %3s", Lookup(fAddress), mnemonics[opcode]);
  strcat(fInstructionText, temp);

  switch(operands[opcode])
  {
    case Imp:
      break;
    case Acc:
      strcat(fInstructionText, " A");
      break;
    case Imm:
      sprintf(temp, " #$%02X", traceData.ObjectCode[1]);
      strcat(fInstructionText, temp);
      break;
    case Zpg:
      sprintf(temp, " $%02X", traceData.ObjectCode[1]);
      strcat(fInstructionText, temp);
      break;
    case ZpX:
      sprintf(temp, " $%02X,X", traceData.ObjectCode[1]);
      strcat(fInstructionText, temp);
      break;
     case ZpY:
      sprintf(temp, " $%02X,Y", traceData.ObjectCode[1]);
      strcat(fInstructionText, temp);
      break;
    case Abs:
      label = getSymbolOperand(*(uint16_t *)(traceData.ObjectCode + 1));

      sprintf(temp, " %s", label);
      strcat(fInstructionText, temp);
      break;
    case AbX:
      label = getSymbolOperand(*(uint16_t *)(traceData.ObjectCode + 1));

      sprintf(temp, " %s,X", label);
      strcat(fInstructionText, temp);
      break;
     case AbY:
      label = getSymbolOperand(*(uint16_t *)(traceData.ObjectCode + 1));

      sprintf(temp, " %s,Y", label);
      strcat(fInstructionText, temp);
      break;
    case Ind:
      label = getSymbolOperand(*(uint16_t *)(traceData.ObjectCode + 1));

      sprintf(temp, " (%s)", label);
      strcat(fInstructionText, temp);
      break;
    case InX:
      sprintf(temp, " ($%02X,X)", traceData.ObjectCode[1]);
      strcat(fInstructionText, temp);
      break;
    case InY:
      sprintf(temp, " ($%02X),Y", traceData.ObjectCode[1]);
      strcat(fInstructionText, temp);
      break;
    case Rel:
      dest = fAddress + 2;
      dest += (int8_t)traceData.ObjectCode[1];

      label = getSymbolOperand(dest);

      sprintf(temp, " %s", label);
      strcat(fInstructionText, temp);
      break;
    case Val:
      sprintf(temp, " $%02X", traceData.ObjectCode[0]);
      strcat(fInstructionText, temp);
      break;
  }
  return traceData;
}
