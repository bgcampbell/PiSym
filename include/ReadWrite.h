#pragma once

#include "IO.h"
#include "InstructionTest.h"
#include "pico_hal.h"
#include "Tape.hpp"
#include "File.hpp"

extern int Directory;

uint8_t __not_in_flash_func(get8)(uint16_t *fAddr, bool fInstructionFetch = false);
uint8_t __not_in_flash_func(get8)(uint16_t fAddr);
uint8_t __not_in_flash_func(get8NoSideEffect)(uint16_t fAddr);
uint8_t __not_in_flash_func(set8)(uint16_t fAddr, uint8_t fValue);
uint16_t __not_in_flash_func(get16)(uint16_t fAddr);
uint16_t __not_in_flash_func(set16)(uint16_t fAddr, uint16_t fValue);
uint8_t __not_in_flash_func(getOperand8)(uint16_t fAddr);
uint16_t __not_in_flash_func(getOperand16)(uint16_t fAddr);
int __not_in_flash_func(FlashOpen)(bool fRoot = false);
bool __not_in_flash_func(FlashClose)();
void __not_in_flash_func(GetPathname)(const char *fFilename, char *fPathname);
bool reportError(FRESULT fResult, UINT fBytesRead, UINT fExpectedBytesRead);
