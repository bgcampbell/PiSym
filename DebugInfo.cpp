#include "DebugInfo.hpp"

TDebugInfo::TDebugInfo()
: TFlashBase(0x80000L, 0x10000L) {}

bool TDebugInfo::WriteFlash(std::vector<SDebuggerDisable> fDebugList)
{
    if(!CheckSize(4 + fDebugList.size() * 4))
        return false;

    Clear();
    Add((uint16_t)0xAA55);  // Magic
    Add((uint16_t)fDebugList.size());
    for(int i = 0; i < fDebugList.size(); i++)
    {
        Add((uint16_t)fDebugList[i].StartAddress);
        Add((uint16_t)fDebugList[i].EndAddress);
    }
    Write();
    return true;
}

std::vector<SDebuggerDisable> TDebugInfo::ReadFlash()
{
    std::vector<SDebuggerDisable> result;
 
    Reset();

    if(GetUint16() == 0xAA55)
    {
        uint16_t nrEntries = GetUint16();

        for(int i = 0; i < nrEntries; i++)
        {
            SDebuggerDisable newEntry;

            newEntry.StartAddress = GetUint16();
            newEntry.EndAddress = GetUint16();
            result.push_back(newEntry);
        }
    }
    return result;
}