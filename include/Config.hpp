#pragma once

#include "PiSym.h"
#include "pico/flash.h"
#include "hardware/flash.h"
#include "FlashBase.hpp"

#define CONFIG_TARGET_OFFSET ( 0x7F000L )

class TConfig : TFlashBase
{
    public:
        bool      UseCRTHW;
        bool      UseTTYHW;
        bool      UseExternalSD;
        bool      UseInternalFlash;
        bool      SimWP;
        uint16_t  DefaultTrace;
        bool      RAEOverwrite;

        TConfig();

        inline uint8_t GetORBMask()
        {
            return 0xFF & (UseCRTHW ? 0xFF : 0x7F) & (UseTTYHW ? 0xFF : 0xBF);
        }
        inline bool UseStorage()
        {
            return UseExternalSD || UseInternalFlash;
        }
        void Save();
        bool Load();
        void Erase();

        void SetWriteProtectSimulation(bool fState);
};
