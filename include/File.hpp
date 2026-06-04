#pragma once

#include <cstdint>
#include "pico_hal.h"
#include "FatFsSd.h"
#include "PiSym.h"
#include "TapeFileBase.hpp"

class TTape;

class TFile : public TTapeFileBase
{
    private:
        FIL     fatfsFile;
        int     flashFileHandle;
        eDevice device;
        int     sequenceNr;

        FRESULT setFileID(const char *fFileID);

    public:
        friend class TTape;
        friend class TTapeFileBase;

        TTape   *Tape;

        FRESULT     Close();
        FRESULT     Read(char *fBuffer, int fBufferSize, UINT *fNrBytesRead);
        FRESULT     Write(char *fBuffer, int fBufferSize, UINT *fNrBytesRead);
};

bool AreYouSure(bool fDefault);
