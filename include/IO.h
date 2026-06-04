#pragma once

#include "InstructionTest.h"
#include "Config.hpp"

uint8_t readORA();
uint8_t readORB();

void writeDDRA(uint16_t fAddr, uint8_t fValue);
void writeDDRB(uint16_t fAddr, uint8_t fValue);

void writeORA(uint16_t fAddr, uint8_t fValue);
void writeORB(uint16_t fAddr, uint8_t fValue);
