#include "FlashBase.hpp"

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    This module is needed for the reading and writing of Config and Debug range data
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

// This function will be called when it's safe to call flash_range_erase
static void call_flash_range_erase(void *fParam)
{
    TFlashBase* conf = (TFlashBase *)fParam;
    uint32_t base = conf->FlashBaseAddress();
    size_t size = FLASH_SECTOR_SIZE * (((conf->Size() - 1) / FLASH_SECTOR_SIZE) + 1);

    flash_range_erase(base, size);
}
static void call_flash_all_erase(void *fParam)
{
    TFlashBase* conf = (TFlashBase *)fParam;
    uint32_t base = conf->FlashBaseAddress();
    size_t size = FLASH_SECTOR_SIZE * (((conf->MaxSize() - 1) / FLASH_SECTOR_SIZE) + 1);

    flash_range_erase(base, size);
}
// This function will be called when it's safe to call flash_range_program
static void call_flash_range_program(void *fParam) 
{
    TFlashBase* conf = (TFlashBase*)fParam;
    uint32_t base = conf->FlashBaseAddress();
    uint8_t *dataPtr = conf->Data();
    size_t size = FLASH_PAGE_SIZE * (((conf->Size() - 1) / FLASH_PAGE_SIZE) + 1);

    flash_range_program(base, dataPtr, size);
}
// This function will be called when it's safe to call flash_range_program
static void call_flash_block_program(void *fParam) 
{
    TFlashBase* conf = (TFlashBase*)fParam;
    uint32_t base = conf->FlashBaseAddress();
    uint8_t *dataPtr = conf->Data();
    size_t size = FLASH_PAGE_SIZE * (((conf->Size() - 1) / FLASH_PAGE_SIZE) + 1);

    flash_range_program(base, dataPtr, size);
}

uint8_t TFlashBase::flash_contents(uint32_t fAddress)
{
    uint32_t *address = (uint32_t *)(XIP_BASE + flashBaseAddress + fAddress);

    return (uint8_t)(*address);
}
        
TFlashBase::TFlashBase(uint32_t fFlashBase, uint32_t fMaxSize)
{
    flashBaseAddress = fFlashBase; // Base address in flash memory
    maxSize = fMaxSize;     // Maximum size of this zone
}
uint32_t TFlashBase::FlashBaseAddress()
{
    return flashBaseAddress;
}
uint32_t TFlashBase::MaxSize()
{
    return maxSize;
}
uint8_t *TFlashBase::Data()
{
    return fileData.data();
}
size_t TFlashBase::Size()
{
    return fileData.size();
}
bool TFlashBase::CheckSize(uint32_t fDataQuantity)
{
    return fDataQuantity < maxSize;
}

void TFlashBase::Clear()
{
    fileData.clear();
}
void TFlashBase::Reset()
{
    readPtr = 0;
}

void TFlashBase::Add(const char *fText)
{
    for(const char *p = fText; *p != '\0'; p++)
        fileData.push_back((uint8_t)*p);
    fileData.push_back((uint8_t)0);
}
// void TFlashBase::Add(std::string *fText)
// {
//     Add(fText.c_str());
// }

void TFlashBase::Add(uint8_t fValue)
{
    fileData.push_back(fValue);
}
void TFlashBase::Add(bool fValue)
{
    Add(fValue ? (uint8_t)0xFF : (uint8_t)0x00);
}
void TFlashBase::Add(uint16_t fValue)
{
    fileData.push_back((uint8_t)(fValue & 0xFF));
    fileData.push_back((uint8_t)(fValue >> 8));
}
void TFlashBase::Add(uint32_t fValue)
{
    fileData.push_back((uint8_t)(fValue & 0xFF));
    fileData.push_back((uint8_t)((fValue >> 8) & 0xFF));
    fileData.push_back((uint8_t)((fValue >> 16) & 0xFF));
    fileData.push_back((uint8_t)((fValue >> 24) & 0xFF));
}
void TFlashBase::InsertLength(uint32_t fLocation)
{
    fileData[fLocation    ] = (uint8_t)(Size() & 0xFF);
    fileData[fLocation + 1] = (uint8_t)((Size() >> 8) & 0xFF);
    fileData[fLocation + 2] = ((uint8_t)((Size() >> 16) & 0xFF));
    fileData[fLocation + 3] = ((uint8_t)((Size() >> 24) & 0xFF));
}

void TFlashBase::GetString(char * fDestination)
{
    char *destPtr = fDestination;
    while(readPtr < maxSize)
    {
        *destPtr++ = flash_contents(readPtr);
        if(flash_contents(readPtr++) == '\0')
            break;
    }
}
uint8_t TFlashBase::GetUint8()
{
    if(readPtr >= maxSize)
        return 0;
    return flash_contents(readPtr++);
}
bool TFlashBase::GetBool()
{
    return GetUint8() != 0x00;
}
uint16_t TFlashBase::GetUint16()
{
    if(readPtr >= (maxSize - 1))
        return 0;

    uint16_t result = flash_contents(readPtr) | (flash_contents(readPtr + 1) << 8);
    readPtr += 2;
    return result;
}
uint32_t TFlashBase::GetUint32()
{
    if(readPtr >= (maxSize - 3))
        return 0L;
    uint32_t result = ((uint32_t)fileData[readPtr]) | ((uint32_t)fileData[readPtr + 1] << 8) | ((uint32_t)fileData[readPtr + 2] << 16) | ((uint32_t)fileData[readPtr + 3] << 24);
    readPtr += 4;
    return result;
}

void TFlashBase::Write()
{
    int rc = flash_safe_execute(call_flash_range_erase, this, UINT32_MAX);

    hard_assert(rc == PICO_OK);
    rc = flash_safe_execute(call_flash_range_program, this, UINT32_MAX);
    hard_assert(rc == PICO_OK);
}

void TFlashBase::Erase()
{
    int rc = flash_safe_execute(call_flash_all_erase, this, UINT32_MAX);

    hard_assert(rc == PICO_OK);
}

void TFlashBase::WriteBlock(uint32_t fAddress, const uint8_t *fData)
{
    Clear();
    for(uint32_t i = 0; i < FLASH_SECTOR_SIZE; i++)
        fileData.push_back(fData[i]);
    Write();
}

void TFlashBase::ReadBlock(uint32_t fAddress, uint8_t *fData, int fDataSize)
{
    readPtr = fAddress;
    for(uint32_t i = 0; i < fDataSize; i++)
        fData[i] = GetUint8();
}
