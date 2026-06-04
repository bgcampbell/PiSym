#pragma once

#include "PiSym.h"
#include "pico/flash.h"
#include "hardware/flash.h"
#include <vector>
#include "FlashBase.hpp"
#include "CommandLine.h"

struct SDebuggerDisable;

class TDebugInfo : public TFlashBase
{
    public:
        TDebugInfo();
        bool                            WriteFlash(std::vector<SDebuggerDisable> fDebugList);
        std::vector<SDebuggerDisable>   ReadFlash();
};
