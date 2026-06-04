#pragma once

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    This module is needed for the reading and writing of Config and Debug range data
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#include <string>
#include <vector>
#include <stdint-gcc.h>
#include <pico.h>
#include "pico/flash.h"
#include "hardware/flash.h"
#include "hardware/regs/addressmap.h"

class TFlashBase
{
    private:
        uint32_t             flashBaseAddress;
        uint32_t             maxSize;
        uint32_t             readPtr;
        std::vector<uint8_t> fileData;

        uint8_t             flash_contents(uint32_t fAddress);

    public:
        TFlashBase(uint32_t fFlashBase, uint32_t fMaxSize);
        uint32_t    FlashBaseAddress();
        uint32_t    MaxSize();
        uint8_t    *Data();
        size_t      Size();
        bool        CheckSize(uint32_t fDataQuantity);
        void        Clear();
        void        Reset();
        void        Add(const char *fText);
        void        Add(std::string *fText);
        void        Add(uint8_t fValue);
        void        Add(bool fValue);
        void        Add(uint16_t fValue);
        void        Add(uint32_t fValue);
        void        InsertLength(uint32_t fLocation);

        void        GetString(char * fDestination);
        uint8_t     GetUint8();
        bool        GetBool();
        uint16_t    GetUint16();
        uint32_t    GetUint32();

        void        Write();
        void        Erase();

        void        WriteBlock(uint32_t fAddress, const uint8_t *fData);
        void        ReadBlock(uint32_t fAddress, uint8_t *fData, int fDataSize = FLASH_SECTOR_SIZE);
};
