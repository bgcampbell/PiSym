#include "Config.hpp"
#include "Tape.hpp"

const bool DEFAULT_USE_CRTHW = false;
const bool DEFAULT_USE_TTYHW = false;
const bool DEFAULT_USE_EXTERNAL_SD = false;
const bool DEFAULT_USE_INTERNAL_FLASH = true;
const bool DEFAULT_SIM_WP = false;
const uint16_t DEFAULT_TRACE = 100;
const bool DEFAULT_RAE_OVERWRITE = false;

TConfig::TConfig()
    : TFlashBase(0x7F000L, 0x1000L)
{
    UseCRTHW = DEFAULT_USE_CRTHW;
    UseTTYHW = DEFAULT_USE_TTYHW;
    UseExternalSD = DEFAULT_USE_EXTERNAL_SD;
    UseInternalFlash = DEFAULT_USE_INTERNAL_FLASH;
    SimWP = DEFAULT_SIM_WP;
    DefaultTrace = DEFAULT_TRACE;
    RAEOverwrite = DEFAULT_RAE_OVERWRITE;
}
void TConfig::Save()
{
    Clear();
    Reset();
    Add((uint16_t)0xAA55);
    Add(UseCRTHW);
    Add(UseTTYHW);
    Add(UseExternalSD);
    Add(UseInternalFlash);
    Add(SimWP);
    Add(DefaultTrace);
    Add(RAEOverwrite);

    Write();
}

bool TConfig::Load()
{
    Reset();
    UseCRTHW = DEFAULT_USE_CRTHW;
    UseTTYHW = DEFAULT_USE_TTYHW;
    UseExternalSD = DEFAULT_USE_EXTERNAL_SD;
    UseInternalFlash = DEFAULT_USE_INTERNAL_FLASH;
    SimWP = DEFAULT_SIM_WP;
    DefaultTrace = DEFAULT_TRACE;
    RAEOverwrite = DEFAULT_RAE_OVERWRITE;

    bool result = false;
    uint16_t signature = GetUint16();
    if(signature == 0xAA55)
    {
        UseCRTHW = GetBool();
        UseTTYHW = GetBool();
        UseExternalSD = GetBool();
        UseInternalFlash = GetBool();
        SimWP = GetBool();
        DefaultTrace = GetUint16();
        RAEOverwrite = GetBool();
        result = true;
    }
    else
        printf("\r\nNo saved configuration (Signature %04X)\r\n", signature);
    SetWriteProtectSimulation(SimWP);
    CurrentTape.Path.SetDevice(UseExternalSD ? eDevice::SD : (UseInternalFlash ? eDevice::Flash : eDevice::Default));
    return result;
 }

void TConfig::SetWriteProtectSimulation(bool fState)
{
    SystemRAMWriteProtected = false;
    RAMPage1WriteProtected = false;
    RAMPage2WriteProtected = false;
    RAMPage3WriteProtected = false;
    for(uint16_t addr = 0xAC00; addr < 0xAFFF; addr += 0x10)
    {
        for(uint16_t reg = 0; reg < 0x10; reg++)
        {
            MapAddress(addr + reg, 0xAC00 + reg);
            MemoryTypeRead(addr + reg, eMemoryAddressResult::MEMORY);
            switch(reg)
            {
                case 0x1:   // ORA
                    MemoryTypeWrite(addr + reg, fState ? eMemoryAddressResult::WriteProtectPort : eMemoryAddressResult::MEMORY);
                    break;
                default:
                    MemoryTypeWrite(addr + reg, eMemoryAddressResult::MEMORY);
                    break;
            }
        }
    }
}

void TConfig::Erase()
{
    TFlashBase::Erase();
}