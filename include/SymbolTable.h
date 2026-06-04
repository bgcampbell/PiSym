#pragma once

#include "PiSym.h"

#define LABELSIZE     6

struct SSymbol
{
  uint16_t  Value;
  char      Name[LABELSIZE + 1];
};

extern const SSymbol SymbolTable[];
extern       int     NrSymbols;

const char *Lookup(uint16_t fAddress);
int         Lookup(const char *fSymbol);
