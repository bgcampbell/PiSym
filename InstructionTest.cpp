#include "InstructionTest.h"


// void dumpMemory(uint16_t fStart, uint16_t fSize);

void blank()
{
  memset(Memory, 0, 65536);
}
int test_abs()
{
  blank();
  Memory[0x0101] = 0x34;
  Memory[0x0102] = 0x12;

  PC = 0x0101; // Starts at byte after operand

  uint16_t addr = absolute(0x101);
  int result = 0;

  if(addr != 0x1234)
  {
    printf("abs() Test 1: Reading from address 0x0101, expected 0x1234, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0103)
  {
    printf("abs() Test 1: PC should be 0x0103, value 0x%04X\r\n", PC);
    result++;
  }

  blank();
  Memory[0x01FF] = 0x34;
  Memory[0x0200] = 0x12;

  PC = 0x01FF; // Starts at byte after operand

  addr = absolute(0x01FF);
  
  if(addr != 0x1234)
  {
    printf("abs() Test 2: Reading from address 0x01FF, expected 0x1234, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0201)
  {
    printf("abs() Test 2: PC should be 0x0201, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_absX()
{
  blank();
  Memory[0x01C1] = 0x34;
  Memory[0x01C2] = 0x12;

  PC = 0x01C1; // Starts at byte after operand
  X = 0;

  uint16_t addr = absX(0x01C1);
  int result = 0;

  if(addr != 0x1234)
  {
    printf("absX() Test 1: Reading from address 0x01C1, expected 0x1234, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x01C3)
  {
    printf("absX() Test 1: PC should be 0x01C3, value 0x%04X\r\n", PC);
    result++;
  }

  blank();
  Memory[0x01C1] = 0x34;
  Memory[0x01C2] = 0x12;

  PC = 0x01C1; // Starts at byte after operand
  X = 0x80;

  addr = absX(0x01C1);
 
  if(addr != 0x12B4)
  {
    printf("absX() Test 2: Reading from address 0x01C1, expected 0x12B4, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x01C3)
  {
    printf("absX() Test 2: PC should be 0x01C3, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_absXNoPC()
{
  blank();
  Memory[0x01C1] = 0x34;
  Memory[0x01C2] = 0x12;

  PC = 0x01C1; // Starts at byte after operand
  X = 0;

  uint16_t addr = absXNoPC(0x01C1);
  int result = 0;

  if(addr != 0x1234)
  {
    printf("absXNoPC() Test 1: Reading from address 0x01C1, expected 0x1234, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x01C3)
  {
    printf("absXNoPC() Test 1: PC should be 0x01C3, value 0x%04X\r\n", PC);
    result++;
  }

  blank();
  Memory[0x01C1] = 0x34;
  Memory[0x01C2] = 0x12;

  PC = 0x01C1; // Starts at byte after operand
  X = 0x80;

  addr = absXNoPC(0x01C1);
 
  if(addr != 0x12B4)
  {
    printf("absXNoPC() Test 2: Reading from address 0x01C1, expected 0x12B4, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x01C3)
  {
    printf("absXNoPC() Test 2: PC should be 0x01C3, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_absY()
{
  blank();
  Memory[0x01C1] = 0x34;
  Memory[0x01C2] = 0x12;

  PC = 0x01C1; // Starts at byte after operand
  Y = 0;

  uint16_t addr = absY(0x01C1);
  int result = 0;

  if(addr != 0x1234)
  {
    printf("absY() Test 1: Reading from address 0x01C1, expected 0x1234, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x01C3)
  {
    printf("absY() Test 1: PC should be 0x01C3, value 0x%04X\r\n", PC);
    result++;
  }

  blank();
  Memory[0x01C1] = 0x34;
  Memory[0x01C2] = 0x12;

  PC = 0x01C1; // Starts at byte after operand
  Y = 0x80;

  addr = absX(0x01C1);
 
  if(addr != 0x12B4)
  {
    printf("absY() Test 2: Reading from address 0x01C1, expected 0x12B4, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x01C3)
  {
    printf("absY() Test 2: PC should be 0x01C3, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_imm()
{
  blank();
  Memory[0x0101] = 0x34;

  PC = 0x0101; // Starts at byte after operand

  uint8_t value = imm(0x101);
  int result = 0;

  if(value != 0x0034)
  {
    printf("imm() Test 1: Reading from address 0x0101, expected 0x0034, read 0x%04X\r\n", value);
    result++;
  }
  if(PC != 0x0102)
  {
    printf("imm() Test 1: PC should be 0x0102, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_indJMP()
{
  blank();
  Memory[0x0101] = 0x34;
  Memory[0x0102] = 0x12;
  Memory[0x1234] = 0xAA;
  Memory[0x1235] = 0x55;
  PC = 0x101; // Starts at byte after operand

  uint16_t addr = indJMP(0x0101);
  int result = 0;

  if(addr != 0x55AA)
  {
    printf("indJMP() Test 1: Reading via address 0x0101, expected 0x55AA, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0103)
  {
    printf("indJMP() Test 1: PC should be 0x0103, value 0x%04X\r\n", PC);
    result++;
  }
  
  blank();
  Memory[0x0101] = 0xFF;
  Memory[0x0102] = 0x12;
  Memory[0x12FF] = 0xAA;
  Memory[0x1200] = 0x55;
  Memory[0x1300] = 0x66;  // Use to detect error
  PC = 0x0101; // Starts at byte after operand

  addr = indJMP(0x0101);

  if(addr == 0x66AA)
  {
    printf("indJMP() Test 2: Reading via address 0x0101, expected 0x55AA, read 0x66AA : JMP indirect bug not working\r\n", addr);
    result++;
  }
  else if(addr != 0x55AA)
  {
    printf("indJMP() Test 2: Reading via address 0x0101, expected 0x55AA, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0103)
  {
    printf("indJMP() Test 2: PC should be 0x0103, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_indX()
{
  blank();
  Memory[0x0101] = 0x10;
  Memory[0x0030] = 0x34;
  Memory[0x0031] = 0x12;
  PC = 0x0101; // Starts at byte after operand
  X = 0x20;

  uint16_t addr = indX(0x0101);
  int result = 0;

  if(addr != 0x1234)
  {
    printf("indX() Test 1: Reading from address 0x0101, expected 0x1234, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0102)
  {
    printf("indX() Test 1: PC should be 0x0102, value 0x%04X\r\n", PC);
    result++;
  }

  blank();
  Memory[0x0101] = 0xE0;
  Memory[0x0040] = 0x34;
  Memory[0x0041] = 0x12;
  PC = 0x0101; // Starts at byte after operand
  X = 0x60;

  addr = indX(0x0101);

  if(addr != 0x1234)
  {
    printf("indX() Test 2: Reading from address 0x0101, expected 0x1234, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0102)
  {
    printf("indX() Test 2: PC should be 0x0102, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_indY()
{
  blank();
  Memory[0x0101] = 0x10;
  Memory[0x0010] = 0x34;
  Memory[0x0011] = 0x12;
  PC = 0x0101; // Starts at byte after operand
  Y = 0x20;

  uint16_t addr = indY(0x0101);
  int result = 0;

  if(addr != 0x1254)
  {
    printf("indY() Test 1: Reading from address 0x0101, expected 0x1254, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0102)
  {
    printf("indY() Test 1: PC should be 0x0102, value 0x%04X\r\n", PC);
    result++;
  }

  blank();
  Memory[0x0101] = 0x40;
  Memory[0x0040] = 0xE4;
  Memory[0x0041] = 0x12;
  PC = 0x0101; // Starts at byte after operand
  Y = 0x60;

  addr = indY(0x0101);

  if(addr != 0x1344)
  {
    printf("indY() Test 2: Reading from address 0x0101, expected 0x1344, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0102)
  {
    printf("indY() Test 2: PC should be 0x0102, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_rel()
{
  blank();
  Memory[0x0120] = 0x10;
  PC = 0x0120;
  nrCycles = 2;

  rel(false, 0x120);
  int result = 0;
  
  if(PC != 0x0121)
  {
    printf("rel() Test 1: PC should be 0x0121, value 0x%04X\r\n", PC);
    result++;
  }
  if(nrCycles != 2)
  {
    printf("rel() Test 1: NrCycles should be 2, value %d\r\n", nrCycles);
    result++; 
  }

  PC = 0x0120;
  nrCycles = 2;

  rel(true, 0x120);
  
  if(PC != 0x0131)
  {
    printf("rel() Test 2: PC should be 0x0131, value 0x%04X\r\n", PC);
    result++;
  }
  if(nrCycles != 3)
  {
    printf("rel() Test 2: NrCycles should be 3, value %d\r\n", nrCycles);
    result++; 
  }

  Memory[0x0120] = 0xF0;
  PC = 0x0120;
  nrCycles = 2;

  rel(true, 0x120);
  
  if(PC != 0x0111)
  {
    printf("rel() Test 3: PC should be 0x0111, value 0x%04X\r\n", PC);
    result++;
  }
  if(nrCycles != 3)
  {
    printf("rel() Test 3: NrCycles should be 3, value %d\r\n", nrCycles);
    result++; 
  }

  Memory[0x01D0] = 0x70;
  PC = 0x01D0;
  nrCycles = 2;

  rel(true, 0x1D0);
  
  if(PC != 0x0241)
  {
    printf("rel() Test 4: PC should be 0x0241, value 0x%04X\r\n", PC);
    result++;
  }
  if(nrCycles != 4)
  {
    printf("rel() Test 4: NrCycles should be 4, value %d\r\n", nrCycles);
    result++; 
  }
  
  Memory[0x0120] = 0xA0;
  PC = 0x0120;
  nrCycles = 2;

  rel(true, 0x120);
  
  if(PC != 0x00C1)
  {
    printf("rel() Test 5: PC should be 0x00C1, value 0x%04X\r\n", PC);
    result++;
  }
  if(nrCycles != 4)
  {
    printf("rel() Test 5: NrCycles should be 4, value %d\r\n", nrCycles);
    result++; 
  }

  return result;
}
int test_zpg()
{
  blank();
  Memory[0x0101] = 0x34;

  PC = 0x0101; // Starts at byte after operand

  uint16_t addr = zpg(0x101);
  int result = 0;

  if(addr != 0x0034)
  {
    printf("zpg() Test 1: Reading from address 0x0101, expected 0x0034, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0102)
  {
    printf("zpg() Test 1: PC should be 0x0102, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_zpgX()
{
  blank();
  Memory[0x0101] = 0x34;

  PC = 0x0101; // Starts at byte after operand
  X = 0x10;

  uint16_t addr = zpgX(0x101);
  int result = 0;

  if(addr != 0x0044)
  {
    printf("zpgX() Test 1: Reading from address 0x0101, expected 0x0044, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0102)
  {
    printf("zpgX() Test 1: PC should be 0x0102, value 0x%04X\r\n", PC);
    result++;
  }

  blank();
  Memory[0x0101] = 0xE4;

  PC = 0x0101; // Starts at byte after operand
  X = 0x40;

  addr = zpgX(0x101);

  if(addr != 0x0024)
  {
    printf("zpgX() Test 2: Reading from address 0x0101, expected 0x0024, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0102)
  {
    printf("zpgX() Test 2: PC should be 0x0102, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_zpgY()
{
  blank();
  Memory[0x0101] = 0x34;

  PC = 0x0101; // Starts at byte after operand
  Y = 0x10;

  uint16_t addr = zpgY(0x101);
  int result = 0;

  if(addr != 0x0044)
  {
    printf("zpgY() Test 1: Reading from address 0x0101, expected 0x0044, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0102)
  {
    printf("zpgY() Test 1: PC should be 0x0102, value 0x%04X\r\n", PC);
    result++;
  }

  blank();
  Memory[0x0101] = 0xE4;

  PC = 0x0101; // Starts at byte after operand
  Y = 0x40;

  addr = zpgY(0x101);

  if(addr != 0x0024)
  {
    printf("zpgY() Test 2: Reading from address 0x0101, expected 0x0024, read 0x%04X\r\n", addr);
    result++;
  }
  if(PC != 0x0102)
  {
    printf("zpgY() Test 2: PC should be 0x0102, value 0x%04X\r\n", PC);
    result++;
  }

  return result;
}
int test_setNZ()
{
  int result = 0;

  SR.Value = 0;

  setNZ(0x05);
  if(SR.Value != 0x00)
  {
    printf("setNZ() Test 1: SR expected 0x00, actual %02X\r\n", SR.Value);
    result++;
  }

  setNZ(0x00);
  if(SR.Value != 0x02)
  {
    printf("setNZ() Test 1: SR expected 0x02, actual %02X\r\n", SR.Value);
    result++;
  }

  setNZ(0x88);
  if(SR.Value != 0x80)
  {
    printf("setNZ() Test 1: SR expected 0x80, actual %02X\r\n", SR.Value);
    result++;
  }

  return result;
}
int report_SR(const char* fTestLabel, uint8_t fExpected, uint8_t fMask)
{
  int result = 0;

  if(fMask & 0x80)
  {
    if((SR.Value & 0x80) != (fExpected & 0x80))
    {
      printf("%s: SR.N should be %s\r\n", fTestLabel, (fExpected & 0x80) ? "set" : "clear");
      result++;
    }
  }
  if(fMask & 0x40)
  {
    if((SR.Value & 0x40) != (fExpected & 0x40))
    {
      printf("%s: SR.V should be %s\r\n", fTestLabel, (fExpected & 0x40) ? "set" : "clear");
      result++;
    }
  }
  if(fMask & 0x02)
  {
    if((SR.Value & 0x02) != (fExpected & 0x02))
    {
      printf("%s: SR.Z should be %s\r\n", fTestLabel, (fExpected & 0x02) ? "set" : "clear");
      result++;
    }
  }
  if(fMask & 0x01)
  {
    if((SR.Value & 0x01) != (fExpected & 0x01))
    {
      printf("%s: SR.Bits.C should be %s\r\n", fTestLabel, (fExpected & 0x01) ? "set" : "clear");
      result++;
    }
  }
  return result;
}
int validateResult(const char* fTestLabel, uint8_t fExpected, uint8_t fMask, eReg fReg, uint8_t fExpectedResult)
{
  int result = 0;

  switch(fReg)
  {
    case eReg::RegA:
      if(A != fExpectedResult)
      {
        printf("%s: A expected 0x%02X, actual %02X\r\n", fTestLabel, fExpectedResult, A);
        result++;
      }
      break;
    case eReg::RegX:
      if(X != fExpectedResult)
      {
        printf("%s: X expected 0x%02X, actual %02X\r\n", fTestLabel, fExpectedResult, X);
        result++;
      }
      break;
    case eReg::RegY:
      if(Y != fExpectedResult)
      {
        printf("%s: Y expected 0x%02X, actual %02X\r\n", fTestLabel, fExpectedResult, Y);
        result++;
      }
      break;
     case eReg::RegSP:
      if(SP != fExpectedResult)
      {
        printf("%s: SP expected 0x%02X, actual %02X\r\n", fTestLabel, fExpectedResult, SP);
        result++;
      }
      break;
     case eReg::RegSR:
      if(SR.Value != fExpectedResult)
      {
        printf("%s: SR expected 0x%02X, actual %02X\r\n", fTestLabel, fExpectedResult, SR.Value);
        result++;
      }
      break;
 }
  result += report_SR(fTestLabel, fExpected, fMask);
  
  return result;
}
int validateResult(const char* fTestLabel, uint8_t fExpected, uint8_t fMask, uint8_t fExpectedA)
{
  return validateResult(fTestLabel, fExpected, fMask, eReg::RegA, fExpectedA);
}
int validateResultRMW(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fMask, uint16_t fAddress, uint8_t fExpectedResult)
{
  int result = 0;

  if(Memory[fAddress] != fExpectedResult)
  {
    printf("%s: Memory[%04X] expected 0x%02X, actual %02X\r\n", fTestLabel, fAddress, fExpectedResult, Memory[fAddress]);
    result++;
  }
  result += report_SR(fTestLabel, fExpectedSR, fMask);
  
  return result;
}

int testADC(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fMask, uint8_t fInitialSR, uint8_t fA, uint8_t fOperand, uint8_t fExpectedA)
{
  SR.Value = fInitialSR;
  A = fA;
  ADC(fOperand);
  return validateResult(fTestLabel, fExpectedSR, fMask, fExpectedA);
}
int test_ADC()
{
  int result = 0;

  // Test Binary  
  // Carry Clear
  result += testADC("ADC() Test 1", 0x00, 0xC3, 0x00, 0x20, 0x35, 0x55);
  result += testADC("ADC() Test 2", 0x01, 0xC3, 0x00, 0x20, 0xE1, 0x01);
  result += testADC("ADC() Test 3", 0x02, 0xC3, 0x00, 0x00, 0x00, 0x00);
  result += testADC("ADC() Test 4", 0x03, 0xC3, 0x00, 0x20, 0xE0, 0x00);
  result += testADC("ADC() Test 5", 0x41, 0xC3, 0x00, 0x90, 0xD5, 0x65);
  result += testADC("ADC() Test 6", 0x43, 0xC3, 0x00, 0x80, 0x80, 0x00);
  result += testADC("ADC() Test 7", 0x80, 0xC3, 0x00, 0x80, 0x04, 0x84);
  result += testADC("ADC() Test 8", 0x81, 0xC3, 0x00, 0xA0, 0xE5, 0x85);
  result += testADC("ADC() Test 9", 0xC0, 0xC3, 0x00, 0x70, 0x35, 0xA5);
  // Carry Set
  result += testADC("ADC() Test 10", 0x00, 0xC3, 0x01, 0x20, 0x35, 0x56);
  result += testADC("ADC() Test 11", 0x01, 0xC3, 0x01, 0x20, 0xE1, 0x02);
  result += testADC("ADC() Test 12", 0x03, 0xC3, 0x01, 0x1F, 0xE0, 0x00);
  result += testADC("ADC() Test 13", 0x41, 0xC3, 0x01, 0x90, 0xD5, 0x66);
  result += testADC("ADC() Test 14", 0x80, 0xC3, 0x01, 0x80, 0x04, 0x85);
  result += testADC("ADC() Test 15", 0x81, 0xC3, 0x01, 0xA0, 0xE5, 0x86);
  result += testADC("ADC() Test 16", 0xC0, 0xC3, 0x01, 0x70, 0x35, 0xA6);

  // Test Decimal
  // Carry Clear
  result += testADC("ADC() Test 17", 0x00, 0x83, 0x08, 0x20, 0x35, 0x55);
  result += testADC("ADC() Test 18", 0x01, 0x83, 0x08, 0x20, 0x85, 0x05);
  result += testADC("ADC() Test 19", 0x02, 0x83, 0x08, 0x00, 0x00, 0x00);
  result += testADC("ADC() Test 20", 0x03, 0x83, 0x08, 0x50, 0x50, 0x00);
  result += testADC("ADC() Test 21", 0x80, 0x83, 0x08, 0x80, 0x05, 0x85);
  result += testADC("ADC() Test 22", 0x81, 0x83, 0x08, 0x99, 0x99, 0x98);
  // Carry Set
  result += testADC("ADC() Test 23", 0x00, 0x83, 0x09, 0x20, 0x35, 0x56);
  result += testADC("ADC() Test 24", 0x01, 0x83, 0x09, 0x20, 0x85, 0x06);
  result += testADC("ADC() Test 25", 0x03, 0x83, 0x09, 0x49, 0x50, 0x00);
  result += testADC("ADC() Test 26", 0x80, 0x83, 0x09, 0x80, 0x05, 0x86);
  result += testADC("ADC() Test 27", 0x81, 0x83, 0x09, 0x99, 0x99, 0x99);

  return result;
}
int testAND(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fA, uint8_t fOperand, uint8_t fExpectedA)
{
  SR.Value = 0x00;
  A = fA;
  AND(fOperand);
  return validateResult(fTestLabel, fExpectedSR, 0x82, fExpectedA);
}
int test_AND()
{
  int result = 0;

  result += testAND("AND() Test 1", 0x00, 0x5A, 0x55, 0x50);
  result += testAND("AND() Test 2", 0x02, 0xAA, 0x55, 0x00);
  result += testAND("AND() Test 3", 0x80, 0xAA, 0xA5, 0xA0);

  return result;
}
int testASL(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fOperand, uint8_t fExpectedResult)
{
  SR.Value = 0x00;
  Memory[0x1234] = fOperand;
  ASL(0x1234);
  return validateResultRMW(fTestLabel, fExpectedSR, 0x83, 0x1234, fExpectedResult);
}
int test_ASL()
{
  int result = 0;

  result += testASL("ASL() Test 1", 0x00, 0x20, 0x40);
  result += testASL("ASL() Test 2", 0x01, 0xA0, 0x40);
  result += testASL("ASL() Test 3", 0x02, 0x00, 0x00);
  result += testASL("ASL() Test 4", 0x03, 0x80, 0x00);
  result += testASL("ASL() Test 5", 0x80, 0x40, 0x80);
  result += testASL("ASL() Test 6", 0x81, 0xC0, 0x80);

  return result;
}
int testASLA(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fA, uint8_t fExpectedA)
{
  SR.Value = 0x00;
  A = fA;
  ASLA();
  return validateResult(fTestLabel, fExpectedSR, 0x83, fExpectedA);
}
int test_ASLA()
{
  int result = 0;

  result += testASLA("ASLA() Test 1", 0x00, 0x20, 0x40);
  result += testASLA("ASLA() Test 2", 0x01, 0xA0, 0x40);
  result += testASLA("ASLA() Test 3", 0x02, 0x00, 0x00);
  result += testASLA("ASLA() Test 4", 0x03, 0x80, 0x00);
  result += testASLA("ASLA() Test 5", 0x80, 0x40, 0x80);
  result += testASLA("ASLA() Test 6", 0x81, 0xC0, 0x80);

  return result;
}
int testBIT(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fA, uint8_t fOperand)
{
  SR.Value = 0x00;
  A = fA;
  BIT(fOperand);
  return validateResult(fTestLabel, fExpectedSR, 0xC2, fA);
}
int test_BIT()
{
  int result = 0;

  result += testBIT("BIT() Test 1", 0x40, 0x5A, 0x55);
  result += testBIT("BIT() Test 2", 0x42, 0xAA, 0x45);
  result += testBIT("BIT() Test 3", 0x80, 0xAA, 0xA5);
  result += testBIT("BIT() Test 4", 0x02, 0xAA, 0x05);

  return result;
}
int testBranch(const char* fTestLabel, uint8_t fInitialSR, uint16_t fPC, uint8_t fBranchDistance, uint16_t fExpectedPC, int fExpectedNrCycles, void (*fBranch)(uint16_t fAddr))
{
  int result = 0;

  nrCycles = 2;
  SR.Value = fInitialSR;
  PC = fPC;
  Memory[fPC] = fBranchDistance;
  (*fBranch)(fPC);
  if(PC != fExpectedPC)
  {
    printf("%s: PC expected to be %04X, actual %04X\r\n", fTestLabel, fExpectedPC, PC);
    result++;
  }
  if(nrCycles != fExpectedNrCycles)
  {
    printf("%s: NrCycles expected to be %d, actual %d\r\n", fTestLabel, fExpectedNrCycles, nrCycles);
    result++;
  }
  return result;
}
int test_BCC()
{
  int result = 0;

  result += testBranch("BCC() Test 1", 0x01, 0x01000, 0x10, 0x1001, 2, BCC);
  result += testBranch("BCC() Test 2", 0x00, 0x01000, 0x10, 0x1011, 3, BCC);
  result += testBranch("BCC() Test 3", 0x00, 0x010F9, 0x10, 0x110A, 4, BCC);

  return result;
}
int test_BCS()
{
  int result = 0;

  result += testBranch("BCS() Test 1", 0x00, 0x01000, 0x10, 0x1001, 2, BCS);
  result += testBranch("BCS() Test 2", 0x01, 0x01000, 0x10, 0x1011, 3, BCS);
  result += testBranch("BCS() Test 3", 0x01, 0x010F9, 0x10, 0x110A, 4, BCS);

  return result;
}
int test_BEQ()
{
  int result = 0;

  result += testBranch("BEQ() Test 1", 0x00, 0x01000, 0x10, 0x1001, 2, BEQ);
  result += testBranch("BEQ() Test 2", 0x02, 0x01000, 0x10, 0x1011, 3, BEQ);
  result += testBranch("BEQ() Test 3", 0x02, 0x010F9, 0x10, 0x110A, 4, BEQ);

  return result;
}
int test_BMI()
{
  int result = 0;

  result += testBranch("BMI() Test 1", 0x00, 0x01000, 0x10, 0x1001, 2, BMI);
  result += testBranch("BMI() Test 2", 0x80, 0x01000, 0x10, 0x1011, 3, BMI);
  result += testBranch("BMI() Test 3", 0x80, 0x010F9, 0x10, 0x110A, 4, BMI);

  return result;
}
int test_BNE()
{
  int result = 0;

  result += testBranch("BNE() Test 1", 0x02, 0x01000, 0x10, 0x1001, 2, BNE);
  result += testBranch("BNE() Test 2", 0x00, 0x01000, 0x10, 0x1011, 3, BNE);
  result += testBranch("BNE() Test 3", 0x00, 0x010F9, 0x10, 0x110A, 4, BNE);

  return result;
}
int test_BPL()
{
  int result = 0;

  result += testBranch("BPL() Test 1", 0x80, 0x01000, 0x10, 0x1001, 2, BPL);
  result += testBranch("BPL() Test 2", 0x00, 0x01000, 0x10, 0x1011, 3, BPL);
  result += testBranch("BPL() Test 3", 0x00, 0x010F9, 0x10, 0x110A, 4, BPL);

  return result;
}
int test_BVC()
{
  int result = 0;

  result += testBranch("BVC() Test 1", 0x40, 0x01000, 0x10, 0x1001, 2, BVC);
  result += testBranch("BVC() Test 2", 0x00, 0x01000, 0x10, 0x1011, 3, BVC);
  result += testBranch("BVC() Test 3", 0x00, 0x010F9, 0x10, 0x110A, 4, BVC);

  return result;
}
int test_BVS()
{
  int result = 0;

  result += testBranch("BVS() Test 1", 0x00, 0x01000, 0x10, 0x1001, 2, BVS);
  result += testBranch("BVS() Test 2", 0x40, 0x01000, 0x10, 0x1011, 3, BVS);
  result += testBranch("BVS() Test 3", 0x40, 0x010F9, 0x10, 0x110A, 4, BVS);

  return result;
}
int test_BRK()
{
  int result = 0;

  SP = 0xFF;
  PC = 0x1234;
  Memory[0x8FFE] = 0x68;  // Account for address mapping
  Memory[0x8FFF] = 0x24;
  SR.Value = 0xAA;
  BRK();
  if(SP != 0xFC)
  {
    printf("BRK() Test 1: SP expected to be 0xFC, actual %02X\r\n", SP);
    result++;
  }
  if(PC != 0x2468)
  {
    printf("BRK() Test 1: PC expected to be 0x2468, actual %04X\r\n", PC);
    result++;
  }
  if(SR.Value != 0xBA)
  {
    printf("BRK() Test 1: SR expected to be 0xBA, actual %02X\r\n", SR.Value);
    result++;
  }
  if(Memory[0x1FD] != 0xBA)
  {
    printf("BRK() Test 1: Stack SR expected to be 0xBA, actual %02X\r\n", Memory[0x1FD]);
    result++;
  }
  if(Memory[0x1FE] != 0x36)
  {
    printf("BRK() Test 1: Stack PC Low expected to be 0x36, actual %02X\r\n", Memory[0x1FE]);
    result++;
  }
  if(Memory[0x1FF] != 0x12)
  {
    printf("BRK() Test 1: Stack PC High expected to be 0x12, actual %02X\r\n", Memory[0x1FF]);
    result++;
  }
  return result;
}
int test_CLC()
{
  SR.Value = 0xFF;
  CLC();
  if(SR.Value != 0xFE)
  {
    printf("CLC() Test 1: Instruction corrupted SR, expected 0xFE actual %02X\r\n", SR.Value);
    return 1;
  }
  return 0;
}
int test_CLD()
{
  SR.Value = 0xFF;
  CLD();
  if(SR.Value != 0xF7)
  {
    printf("CLD() Test 1: Instruction corrupted SR, expected 0xF7 actual %02X\r\n", SR.Value);
    return 1;
  }
  return 0;
}
int test_CLI()
{
  SR.Value = 0xFF;
  CLI();
  if(SR.Value != 0xFB)
  {
    printf("CLI() Test 1: Instruction corrupted SR, expected 0xFB actual %02X\r\n", SR.Value);
    return 1;
  }
  return 0;
}
int test_CLV()
{
  SR.Value = 0xFF;
  CLV();
  if(SR.Value != 0xBF)
  {
    printf("CLV() Test 1: Instruction corrupted SR, expected 0xBF actual %02X\r\n", SR.Value);
    return 1;
  }
  return 0;
}
int testCMP(const char* fTestLabel, uint8_t fExpectedSR, eReg fReg, uint8_t fRegValue, uint8_t fOperand)
{
  SR.Value = 0x00;

  switch(fReg)
  {
    case eReg::RegA:
      A = fRegValue;
      CMP(fOperand);
      break;
    case eReg::RegX:
      X = fRegValue;
      CPX(fOperand);
      break;
    case eReg::RegY:
      Y = fRegValue;
      CPY(fOperand);
      break;
  }
  return report_SR(fTestLabel, fExpectedSR, 0x83);
}
int test_CMP()
{
  int result = 0;

  result += testCMP("CMP() Test 1", 0x00, eReg::RegA, 0x05, 0xF0);
  result += testCMP("CMP() Test 2", 0x01, eReg::RegA, 0x25, 0x20);
  result += testCMP("CMP() Test 3", 0x03, eReg::RegA, 0x20, 0x20);
  result += testCMP("CMP() Test 4", 0x80, eReg::RegA, 0x20, 0x40);
  result += testCMP("CMP() Test 5", 0x81, eReg::RegA, 0xA0, 0x10);

  return result;
}
int test_CPX()
{
  int result = 0;

  result += testCMP("CPX() Test 1", 0x00, eReg::RegX, 0x05, 0xF0);
  result += testCMP("CPX() Test 2", 0x01, eReg::RegX, 0x25, 0x20);
  result += testCMP("CPX() Test 3", 0x03, eReg::RegX, 0x20, 0x20);
  result += testCMP("CPX() Test 4", 0x80, eReg::RegX, 0x20, 0x40);
  result += testCMP("CPX() Test 5", 0x81, eReg::RegX, 0xA0, 0x10);

  return result;
}
int test_CPY()
{
  int result = 0;

  result += testCMP("CPY() Test 1", 0x00, eReg::RegY, 0x05, 0xF0);
  result += testCMP("CPY() Test 2", 0x01, eReg::RegY, 0x25, 0x20);
  result += testCMP("CPY() Test 3", 0x03, eReg::RegY, 0x20, 0x20);
  result += testCMP("CPY() Test 4", 0x80, eReg::RegY, 0x20, 0x40);
  result += testCMP("CPY() Test 5", 0x81, eReg::RegY, 0xA0, 0x10);

  return result;
}
int testDEC(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fOperand, uint8_t fExpectedResult)
{
  SR.Value = 0x00;
  Memory[0x1234] = fOperand;
  DECrement(0x1234);
  return validateResultRMW(fTestLabel, fExpectedSR, 0x82, 0x1234, fExpectedResult);
}
int test_DEC()
{
  int result = 0;

  result += testDEC("DEC() Test 1", 0x00, 0x05, 0x04);
  result += testDEC("DEC() Test 2", 0x02, 0x01, 0x00);
  result += testDEC("DEC() Test 3", 0x80, 0x00, 0xFF);

  return result;
}
int testDEXY(const char* fTestLabel, uint8_t fExpectedSR, eReg fReg, uint8_t fRegValue, uint8_t fExpectedResult)
{
  SR.Value = 0x00;

  switch(fReg)
  {
    case eReg::RegX:
      X = fRegValue;
      DEX();
      break;
    case eReg::RegY:
      Y = fRegValue;
      DEY();
      break;
  }
  return validateResult(fTestLabel, fExpectedSR, 0x82, fReg, fExpectedResult);
}
int test_DEX()
{
  int result = 0;

  result += testDEXY("DEX() Test 1", 0x00, eReg::RegX, 0x05, 0x04);
  result += testDEXY("DEX() Test 2", 0x02, eReg::RegX, 0x01, 0x00);
  result += testDEXY("DEX() Test 3", 0x80, eReg::RegX, 0x00, 0xFF);

  return result;
}
int test_DEY()
{
  int result = 0;

  result += testDEXY("DEY() Test 1", 0x00, eReg::RegY, 0x05, 0x04);
  result += testDEXY("DEY() Test 2", 0x02, eReg::RegY, 0x01, 0x00);
  result += testDEXY("DEY() Test 3", 0x80, eReg::RegY, 0x00, 0xFF);

  return result;
}
int testEOR(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fA, uint8_t fOperand, uint8_t fExpectedA)
{
  SR.Value = 0x00;
  A = fA;
  EOR(fOperand);
  return validateResult(fTestLabel, fExpectedSR, 0x82, fExpectedA);
}
int test_EOR()
{
  int result = 0;

  result += testEOR("EOR() Test 1", 0x00, 0xAA, 0xA5, 0x0F);
  result += testEOR("EOR() Test 2", 0x02, 0xAA, 0xAA, 0x00);
  result += testEOR("EOR() Test 3", 0x80, 0x5A, 0xAA, 0xF0);

  return result;
}
int testINC(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fOperand, uint8_t fExpectedResult)
{
  SR.Value = 0x00;
  Memory[0x1234] = fOperand;
  INC(0x1234);
  return validateResultRMW(fTestLabel, fExpectedSR, 0x82, 0x1234, fExpectedResult);
}
int test_INC()
{
  int result = 0;

  result += testINC("INC() Test 1", 0x00, 0x05, 0x06);
  result += testINC("INC() Test 2", 0x02, 0xFF, 0x00);
  result += testINC("INC() Test 3", 0x80, 0x7F, 0x80);

  return result;
}
int testINXY(const char* fTestLabel, uint8_t fExpectedSR, eReg fReg, uint8_t fRegValue, uint8_t fExpectedResult)
{
  SR.Value = 0x00;

  switch(fReg)
  {
    case eReg::RegX:
      X = fRegValue;
      INX();
      break;
    case eReg::RegY:
      Y = fRegValue;
      INY();
      break;
  }
  return validateResult(fTestLabel, fExpectedSR, 0x82, fReg, fExpectedResult);
}
int test_INX()
{
  int result = 0;

  result += testINXY("INX() Test 1", 0x00, eReg::RegX, 0x05, 0x06);
  result += testINXY("INX() Test 2", 0x02, eReg::RegX, 0xFF, 0x00);
  result += testINXY("INX() Test 3", 0x80, eReg::RegX, 0x7F, 0x80);

  return result;
}
int test_INY()
{
  int result = 0;

  result += testINXY("INY() Test 1", 0x00, eReg::RegY, 0x05, 0x06);
  result += testINXY("INY() Test 2", 0x02, eReg::RegY, 0xFF, 0x00);
  result += testINXY("INY() Test 3", 0x80, eReg::RegY, 0x7F, 0x80);

  return result;
}
int test_JMP()
{
  PC = 0;
  JMP(0x1234);
  if(PC != 0x1234)
  {
    printf("JMP() Test 1: PC expected to be 0x1234, actual %04X\r\n", PC);
    return 1;
  }
  return 0;
}
int test_JSR()
{
  int result = 0;

  SP = 0xFF;
  PC = 0x1236;
  JSR(0x2468);

  if(SP != 0xFD)
  {
    printf("JSR() Test 1: SP expected to be 0xFD, actual %02X\r\n", SP);
    result++;
  }
  if(PC != 0x2468)
  {
    printf("JSR() Test 1: PC expected to be 0x2468, actual %04X\r\n", PC);
    result++;
  }
  if(Memory[0x1FE] != 0x36)
  {
    printf("JSR() Test 1: Stack PC Low expected to be 0x36, actual %02X\r\n", Memory[0x1FE]);
    result++;
  }
  if(Memory[0x1FF] != 0x12)
  {
    printf("JSR() Test 1: Stack PC High expected to be 0x12, actual %02X\r\n", Memory[0x1FF]);
    result++;
  }
  return result;  
}
int testLDx(const char* fTestLabel, uint8_t fExpectedSR, eReg fReg, uint8_t fRegValue)
{
  SR.Value = 0;
  
  switch(fReg)
  {
    case eReg::RegA:
      LDA(fRegValue);
      break;
    case eReg::RegX:
      LDX(fRegValue);
      break;
    case eReg::RegY:
      LDY(fRegValue);
      break;
  }
  return validateResult(fTestLabel, fExpectedSR, 0x82, fReg, fRegValue);
}
int test_LDA()
{
  int result = 0;

  result += testLDx("LDA() Test 1", 0x00, eReg::RegA, 0x05);
  result += testLDx("LDA() Test 2", 0x02, eReg::RegA, 0x00);
  result += testLDx("LDA() Test 3", 0x80, eReg::RegA, 0x80);

  return result;
}
int test_LDX()
{
  int result = 0;

  result += testLDx("LDX() Test 1", 0x00, eReg::RegX, 0x05);
  result += testLDx("LDX() Test 2", 0x02, eReg::RegX, 0x00);
  result += testLDx("LDX() Test 3", 0x80, eReg::RegX, 0x80);

  return result;
}
int test_LDY()
{
  int result = 0;

  result += testLDx("LDY() Test 1", 0x00, eReg::RegY, 0x05);
  result += testLDx("LDY() Test 2", 0x02, eReg::RegY, 0x00);
  result += testLDx("LDY() Test 3", 0x80, eReg::RegY, 0x80);

  return result;
}
int testLSR(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fOperand, uint8_t fExpectedResult)
{
  SR.Value = 0x00;
  Memory[0x1234] = fOperand;
  LSR(0x1234);
  return validateResultRMW(fTestLabel, fExpectedSR, 0x83, 0x1234, fExpectedResult);
}
int test_LSR()
{
  int result = 0;

  result += testLSR("LSR() Test 1", 0x00, 0x80, 0x40);
  result += testLSR("LSR() Test 2", 0x01, 0x81, 0x40);
  result += testLSR("LSR() Test 3", 0x02, 0x00, 0x00);
  result += testLSR("LSR() Test 4", 0x03, 0x01, 0x00);

  return result;
}
int testLSRA(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fA, uint8_t fExpectedA)
{
  SR.Value = 0x00;
  A = fA;
  LSRA();
  return validateResult(fTestLabel, fExpectedSR, 0x83, fExpectedA);
}
int test_LSRA()
{
  int result = 0;

  result += testLSR("LSRA() Test 1", 0x00, 0x80, 0x40);
  result += testLSR("LSRA() Test 2", 0x01, 0x81, 0x40);
  result += testLSR("LSRA() Test 3", 0x02, 0x00, 0x00);
  result += testLSR("LSRA() Test 4", 0x03, 0x01, 0x00);

  return result;
}
int testORA(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fA, uint8_t fOperand, uint8_t fExpectedA)
{
  SR.Value = 0x00;
  A = fA;
  ORA(fOperand);
  return validateResult(fTestLabel, fExpectedSR, 0x82, fExpectedA);
}
int test_ORA()
{
  int result = 0;

  result += testORA("ORA() Test 1", 0x00, 0x5A, 0x55, 0x5F);
  result += testORA("ORA() Test 2", 0x02, 0x00, 0x00, 0x00);
  result += testORA("ORA() Test 3", 0x80, 0x5A, 0xAA, 0xFA);

  return result;
}
int test_PHA()
{
  int result = 0;

  A = 0xA5;
  SP = 0xFF;
  PHA();
  if(SP != 0xFE)
  {
      printf("PHA() Test 1: SP expected 0xFE, actual %02X\r\n", SP);
      result++;
  }
  if(Memory[0x01FF] != 0xA5)
  {
     printf("PHA() Test 1: Memory[0x01FF] expected 0xA5, actual %02X\r\n", Memory[0x01FF]);
    result++;
  }
  return result;
}
int test_PHP()
{
  int result = 0;

  SR.Value = 0xA5;
  SP = 0xFF;
  PHP();
  if(SP != 0xFE)
  {
      printf("PHP() Test 1: SP expected 0xFE, actual %02X\r\n", SP);
      result++;
  }
  if(Memory[0x01FF] != 0xA5)
  {
    printf("PHP() Test 1: Memory[0x01FF] expected 0xA5, actual %02X\r\n", Memory[0x01FF]);
    result++;
  }
  return result;
}
int testPLA(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fValue)
{
  int result = 0;

  SR.Value = 0;
  SP = 0xFE;
  Memory[0x01FF] = fValue;
  PLA();
  if(SP != 0xFF)
  {
      printf("%s: SP expected 0xFF, actual %02X\r\n", fTestLabel, SP);
      result++;
  }
  result += validateResult(fTestLabel, fExpectedSR, 0x82, fValue);
  
  return result;
}
int test_PLA()
{
  int result = 0;

  result += testPLA("PLA() Test 1", 0x00, 0x20);
  result += testPLA("PLA() Test 2", 0x02, 0x00);
  result += testPLA("PLA() Test 3", 0x80, 0xAA);
 
  return result;
}
int test_PLP()
{
  int result = 0;

  SR.Value = 0;
  SP = 0xFE;
  Memory[0x01FF] = 0xA5;
  PLP();
  if(SP != 0xFF)
  {
      printf("PLP() Test 1: SP expected 0xFF, actual %02X\r\n", SP);
      result++;
  }
  result += validateResult("PLP() Test 1", 0xA5, 0xFF, eReg::RegSR, 0xA5);
  
  return result;
}
int testROL(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fOperand, int fCarryIn, uint8_t fExpectedResult)
{
  SR.Value = 0x00;
  SR.Bits.C = fCarryIn;
  Memory[0x1234] = fOperand;
  ROL(0x1234);
  return validateResultRMW(fTestLabel, fExpectedSR, 0x83, 0x1234, fExpectedResult);
}
int test_ROL()
{
  int result = 0;

  result += testROL("ROL() Test 1", 0x00, 0x20, 0, 0x40);
  result += testROL("ROL() Test 2", 0x01, 0xA0, 0, 0x40);
  result += testROL("ROL() Test 3", 0x02, 0x00, 0, 0x00);
  result += testROL("ROL() Test 4", 0x03, 0x80, 0, 0x00);
  result += testROL("ROL() Test 5", 0x80, 0x40, 0, 0x80);
  result += testROL("ROL() Test 6", 0x81, 0xC0, 0, 0x80);
  
  result += testROL("ROL() Test 7",  0x00, 0x20, 1, 0x41);
  result += testROL("ROL() Test 8",  0x01, 0xA0, 1, 0x41);
  result += testROL("ROL() Test 9",  0x80, 0x40, 1, 0x81);
  result += testROL("ROL() Test 10", 0x81, 0xC0, 1, 0x81);

  return result;
}
int testROLA(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fA, int fCarryIn, uint8_t fExpectedA)
{
  SR.Value = 0x00;
  SR.Bits.C = fCarryIn;
  A = fA;
  ROLA();
  return validateResult(fTestLabel, fExpectedSR, 0x83, fExpectedA);
}
int test_ROLA()
{
  int result = 0;

  result += testROLA("ROLA() Test 1", 0x00, 0x20, 0, 0x40);
  result += testROLA("ROLA() Test 2", 0x01, 0xA0, 0, 0x40);
  result += testROLA("ROLA() Test 3", 0x02, 0x00, 0, 0x00);
  result += testROLA("ROLA() Test 4", 0x03, 0x80, 0, 0x00);
  result += testROLA("ROLA() Test 5", 0x80, 0x40, 0, 0x80);
  result += testROLA("ROLA() Test 6", 0x81, 0xC0, 0, 0x80);
  
  result += testROLA("ROLA() Test 7",  0x00, 0x20, 1, 0x41);
  result += testROLA("ROLA() Test 8",  0x01, 0xA0, 1, 0x41);
  result += testROLA("ROLA() Test 9",  0x80, 0x40, 1, 0x81);
  result += testROLA("ROLA() Test 10", 0x81, 0xC0, 1, 0x81);

  return result;
}
int testROR(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fOperand, int fCarryIn, uint8_t fExpectedResult)
{
  SR.Value = 0x00;
  SR.Bits.C = fCarryIn;
  Memory[0x1234] = fOperand;
  ROR(0x1234);
  return validateResultRMW(fTestLabel, fExpectedSR, 0x83, 0x1234, fExpectedResult);
}
int test_ROR()
{
  int result = 0;

  result += testROR("ROR() Test 1", 0x00, 0x80, 0, 0x40);
  result += testROR("ROR() Test 2", 0x01, 0x81, 0, 0x40);
  result += testROR("ROR() Test 3", 0x02, 0x00, 0, 0x00);
  result += testROR("ROR() Test 4", 0x03, 0x01, 0, 0x00);
  
  result += testROR("ROR() Test 6", 0x80, 0x80, 1, 0xC0);
  result += testROR("ROR() Test 7", 0x81, 0x81, 1, 0xC0);

  return result;
}
int testRORA(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fA, int fCarryIn, uint8_t fExpectedA)
{
  SR.Value = 0x00;
  SR.Bits.C = fCarryIn;
  A = fA;
  RORA();
  return validateResult(fTestLabel, fExpectedSR, 0x83, fExpectedA);
}
int test_RORA()
{
  int result = 0;

  result += testRORA("RORA() Test 1", 0x00, 0x80, 0, 0x40);
  result += testRORA("RORA() Test 2", 0x01, 0x81, 0, 0x40);
  result += testRORA("RORA() Test 3", 0x02, 0x00, 0, 0x00);
  result += testRORA("RORA() Test 4", 0x03, 0x01, 0, 0x00);
  
  result += testRORA("RORA() Test 6", 0x80, 0x80, 1, 0xC0);
  result += testRORA("RORA() Test 7", 0x81, 0x81, 1, 0xC0);

  return result;
}
int test_RTI()
{
  int result = 0;

  SR.Value = 0;
  SP = 0xFC;
  Memory[0x01FD] = 0xAA;
  Memory[0x01FE] = 0x68;
  Memory[0x01FF] = 0x24;
  RTI();

  if(SP != 0xFF)
  {
    printf("RTI() Test 1: SP expected to be 0xFF, actual %02X\r\n", SP);
    result++;
  }
  if(PC != 0x2468)
  {
    printf("RTI() Test 1: PC expected to be 0x2468, actual %04X\r\n", PC);
    result++;
  }
  if(SR.Value != 0xAA)
  {
    printf("RTI() Test 1: SR expected to be 0xAA, actual %02X\r\n", SR.Value);
    result++;
  }

  return result;  
}
int test_RTS()
{
  int result = 0;

  SR.Value = 0;
  SP = 0xFD;
  Memory[0x01FE] = 0x68;
  Memory[0x01FF] = 0x24;
  RTS();

  if(SP != 0xFF)
  {
    printf("RTS() Test 1: SP expected to be 0xFF, actual %02X\r\n", SP);
    result++;
  }
  if(PC != 0x2468)
  {
    printf("RTI() Test 1: PC expected to be 0x2468, actual %04X\r\n", PC);
    result++;
  }

  return result;  
}
int testSBC(const char* fTestLabel, uint8_t fExpectedSR, uint8_t fMask, uint8_t fInitialSR, uint8_t fA, uint8_t fOperand, uint8_t fExpectedA)
{
  SR.Value = fInitialSR;
  A = fA;
  SBC(fOperand);
  return validateResult(fTestLabel, fExpectedSR, fMask, fExpectedA);
}
int test_SBC()
{
  int result = 0;

  // Test Binary  
  // Carry Clear : i.e. Borrow Set
  result += testSBC("SBC() Test 1", 0x00, 0xC3, 0x00, 0x20, 0xE1, 0x3E);
  result += testSBC("SBC() Test 2", 0x01, 0xC3, 0x00, 0x40, 0x25, 0x1A);
  result += testSBC("SBC() Test 3", 0x03, 0xC3, 0x00, 0x01, 0x00, 0x00);
  result += testSBC("SBC() Test 4", 0x41, 0xC3, 0x00, 0xFF, 0x7F, 0x7F);
  result += testSBC("SBC() Test 5", 0x80, 0xC3, 0x00, 0xF5, 0xFF, 0xF5);
  result += testSBC("SBC() Test 6", 0x81, 0xC3, 0x00, 0xF5, 0x06, 0xEE);
  result += testSBC("SBC() Test 7", 0xC0, 0xC3, 0x00, 0x7E, 0xFD, 0x80);
  // Carry Set : i.e. Borrow Clear
  result += testSBC("SBC() Test 8",  0x00, 0xC3, 0x01, 0x20, 0xE1, 0x3F);
  result += testSBC("SBC() Test 9",  0x01, 0xC3, 0x01, 0x40, 0x25, 0x1B);
  result += testSBC("SBC() Test 10", 0x03, 0xC3, 0x01, 0x00, 0x00, 0x00);
  result += testSBC("SBC() Test 11", 0x41, 0xC3, 0x01, 0xFE, 0x7F, 0x7F);
  result += testSBC("SBC() Test 12", 0x80, 0xC3, 0x01, 0xF5, 0xFF, 0xF6);
  result += testSBC("SBC() Test 13", 0x81, 0xC3, 0x01, 0xF5, 0x06, 0xEF);
  result += testSBC("SBC() Test 14", 0xC0, 0xC3, 0x01, 0x7E, 0xFE, 0x80);
  // Test Decimal
  // Carry Clear : i.e. Borrow Set
  result += testSBC("SBC() Test 100", 0x00, 0x83, 0x08, 0x05, 0x35, 0x69);
  result += testSBC("SBC() Test 101", 0x01, 0x83, 0x08, 0x95, 0x25, 0x69);
  result += testSBC("SBC() Test 102", 0x03, 0x83, 0x08, 0x01, 0x00, 0x00);
  result += testSBC("SBC() Test 103", 0x80, 0x83, 0x08, 0x05, 0x06, 0x98);
  result += testSBC("SBC() Test 104", 0x81, 0x83, 0x08, 0x99, 0x05, 0x93);
  // Carry Set : i.e. Borrow Clear
  result += testSBC("SBC() Test 105", 0x00, 0x83, 0x09, 0x05, 0x35, 0x70);
  result += testSBC("SBC() Test 106", 0x01, 0x83, 0x09, 0x95, 0x25, 0x70);
  result += testSBC("SBC() Test 107", 0x03, 0x83, 0x09, 0x00, 0x00, 0x00);
  result += testSBC("SBC() Test 108", 0x80, 0x83, 0x09, 0x05, 0x06, 0x99);
  result += testSBC("SBC() Test 109", 0x81, 0x83, 0x09, 0x99, 0x05, 0x94);
  return result;
}
int test_SEC()
{
  SR.Value = 0x00;
  SEC();
  if(SR.Value != 0x01)
  {
    printf("SEC() Test 1: Instruction corrupted SR, expected 0x01 actual %02X\r\n", SR.Value);
    return 1;
  }
  return 0;
}
int test_SED()
{
  SR.Value = 0x00;
  SED();
  if(SR.Value != 0x08)
  {
    printf("SED() Test 1: Instruction corrupted SR, expected 0x08 actual %02X\r\n", SR.Value);
    return 1;
  }
  return 0;
}
int test_SEI()
{
  SR.Value = 0x00;
  SEI();
  if(SR.Value != 0x04)
  {
    printf("SEI() Test 1: Instruction corrupted SR, expected 0x04 actual %02X\r\n", SR.Value);
    return 1;
  }
  return 0;
}
int test_STA()
{
  A = 0x55;
  STA(0x1234);
  if(Memory[0x1234] != 0x55)
  {
    printf("STA() Test 1: Memory write failed, expected 0x55 actual %02X\r\n", Memory[0x1234]);
    return 1;
  }
  return 0;
}
int test_STX()
{
  X = 0x55;
  STX(0x1234);
  if(Memory[0x1234] != 0x55)
  {
    printf("STX() Test 1: Memory write failed, expected 0x55 actual %02X\r\n", Memory[0x1234]);
    return 1;
  }
  return 0;
}
int test_STY()
{
  Y = 0x55;
  STY(0x1234);
  if(Memory[0x1234] != 0x55)
  {
    printf("STY() Test 1: Memory write failed, expected 0x55 actual %02X\r\n", Memory[0x1234]);
    return 1;
  }
  return 0;
}
int testTAX(const char* fTestLabel, uint8_t fValue, uint8_t fExpectedSR)
{
  A = fValue;
  TAX();

  return validateResult(fTestLabel, fExpectedSR, 0x82, eReg::RegX, fValue);
}
int test_TAX()
{
  int result = 0;

  result += testTAX("TAX() Test 1", 0x12, 0x00);
  result += testTAX("TAX() Test 2", 0x00, 0x02);
  result += testTAX("TAX() Test 3", 0x82, 0x80);

  return result;
}
int testTAY(const char* fTestLabel, uint8_t fValue, uint8_t fExpectedSR)
{
  A = fValue;
  TAY();

  return validateResult(fTestLabel, fExpectedSR, 0x82, eReg::RegY, fValue);
}
int test_TAY()
{
  int result = 0;

  result += testTAY("TAY() Test 1", 0x12, 0x00);
  result += testTAY("TAY() Test 2", 0x00, 0x02);
  result += testTAY("TAY() Test 3", 0x82, 0x80);

  return result;
}
int testTSX(const char* fTestLabel, uint8_t fValue, uint8_t fExpectedSR)
{
  SP = fValue;
  TSX();

  return validateResult(fTestLabel, fExpectedSR, 0x82, eReg::RegX, fValue);
}
int test_TSX()
{
  int result = 0;

  result += testTSX("TSX() Test 1", 0x12, 0x00);
  result += testTSX("TSX() Test 2", 0x00, 0x02);
  result += testTSX("TSX() Test 3", 0x82, 0x80);

  return result;
}
int testTXA(const char* fTestLabel, uint8_t fValue, uint8_t fExpectedSR)
{
  X = fValue;
  TXA();

  return validateResult(fTestLabel, fExpectedSR, 0x82, fValue);
}
int test_TXA()
{
  int result = 0;

  result += testTXA("TXA() Test 1", 0x12, 0x00);
  result += testTXA("TXA() Test 2", 0x00, 0x02);
  result += testTXA("TXA() Test 3", 0x82, 0x80);

  return result;
}
int test_TXS()
{
  X = 0x55;
  TXS();

  if(SP != 0x55)
  {
    printf("TXS() Test 1: SP expected 0x55 actual %02X\r\n", SP);
    return 1;
  }
  return 0;
}
int testTYA(const char* fTestLabel, uint8_t fValue, uint8_t fExpectedSR)
{
  Y = fValue;
  TYA();

  return validateResult(fTestLabel, fExpectedSR, 0x82, fValue);
}
int test_TYA()
{
  int result = 0;

  result += testTYA("TYA() Test 1", 0x12, 0x00);
  result += testTYA("TYA() Test 2", 0x00, 0x02);
  result += testTYA("TYA() Test 3", 0x82, 0x80);

  return result;
}

void SBC_BCD_FullTest()
{
  for(int c = 0; c < 2; c++)
  {
    printf("\r\nCarry = %d", c);
    for(int a = 0; a < 100; a++)
    {
      int bcdA = (a / 10) * 16 + (a % 10);
      printf("\r\nA = 0x%02X", bcdA);
      for(int b = 0; b < 100; b += 20)
      {
        int bcdB = (b / 10) * 16 + (b % 10);
        printf("\r\nB 0x%02X: ", bcdB);
        
        for(int bb = 0; bb < 20; bb++)
        {
          int bcdB = ((b + bb) / 10) * 16 + ((b + bb) % 10);

          SR.Value = 0x08 + c;
          A = bcdA;
          SBC(bcdB);
          int correctAnswer = a - (b + bb) - !c;
          if(correctAnswer < 0)
            correctAnswer += 100;
          char mark[2] = " ";
          char txtResult[11];
          char txtCorrect[11];
          sprintf(txtResult, "%02X", A);
          sprintf(txtCorrect, "%02d", correctAnswer);
          if(strcmp(txtCorrect, txtResult) != 0)
            mark[0] = '*';
          printf("0x%02X%s(%2d) ", A, mark, correctAnswer);
        }
      }
    }
    CRLF();
  }
}

int RunTests(void)
{
  int nrErrors = 0;

  nrErrors += test_abs();
  nrErrors += test_absX();
  nrErrors += test_absXNoPC();
  nrErrors += test_absY();
  nrErrors += test_imm();
  nrErrors += test_indJMP();
  nrErrors += test_indX();
  nrErrors += test_indY();
  nrErrors += test_rel();
  nrErrors += test_zpg();
  nrErrors += test_zpgX();
  nrErrors += test_zpgY();

  nrErrors += test_setNZ();

  nrErrors += test_ADC();
  nrErrors += test_AND();
  nrErrors += test_ASL();
  nrErrors += test_ASLA();
  nrErrors += test_BCC();
  nrErrors += test_BCS();
  nrErrors += test_BEQ();
  nrErrors += test_BIT();
  nrErrors += test_BMI();
  nrErrors += test_BNE();
  nrErrors += test_BPL();
  nrErrors += test_BRK();
  nrErrors += test_BVC();
  nrErrors += test_BVS();
  nrErrors += test_CLC();
  nrErrors += test_CLD();
  nrErrors += test_CLI();
  nrErrors += test_CLV();
  nrErrors += test_CMP();
  nrErrors += test_CPX();
  nrErrors += test_CPY();
  nrErrors += test_DEC();
  nrErrors += test_DEX();
  nrErrors += test_DEY();
  nrErrors += test_EOR();
  nrErrors += test_INC();
  nrErrors += test_INX();
  nrErrors += test_INY();
  nrErrors += test_JMP();
  nrErrors += test_JSR();
  nrErrors += test_LDA();
  nrErrors += test_LDX();
  nrErrors += test_LDY();
  nrErrors += test_LSR();
  nrErrors += test_LSRA();
  nrErrors += test_ORA();
  nrErrors += test_PHA();
  nrErrors += test_PHP();
  nrErrors += test_PLA();
  nrErrors += test_PLP();
  nrErrors += test_ROL();
  nrErrors += test_ROLA();
  nrErrors += test_ROR();
  nrErrors += test_RORA();
  nrErrors += test_RTI();
  nrErrors += test_RTS();
  nrErrors += test_SBC();
  nrErrors += test_SEC();
  nrErrors += test_SED();
  nrErrors += test_SEI();
  nrErrors += test_STA();
  nrErrors += test_STX();
  nrErrors += test_STY();
  nrErrors += test_TAX();
  nrErrors += test_TAY();
  nrErrors += test_TSX();
  nrErrors += test_TXA();
  nrErrors += test_TXS();
  nrErrors += test_TYA();

  return nrErrors;
}
