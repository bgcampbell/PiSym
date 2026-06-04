#pragma once

#include "TraceData.hpp"
#include "SymbolTable.h"

class TTraceData;

TTraceData Disassemble(uint16_t fAddress, char * fDestination);
