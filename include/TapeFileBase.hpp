#pragma once

#include <cstdint>
#include "pico_hal.h"
#include "FatFsSd.h"
#include "PiSym.h"
#include "Config.hpp"
#include "Path.hpp"

#define FULL_TAPE_PATH_LENGTH   257
#define FULL_FILE_PATH_LENGTH   (FULL_TAPE_PATH_LENGTH + 6)

extern FATFS fs;

class TFile;

struct TInfo
{
    std::string     Name;
    int             Size;
};

class TTapeFileBase
{
    protected:
                int         flashDirectory;
                DIR         sdDirectory;

    protected:

                bool        isSD(eDevice fDevice);
                bool        isFlash(eDevice fDevice);

                bool        tapeExists(const char *fTapeName, bool fDefaultIfError = true, eDevice fDevice = eDevice::Default);
                FRESULT     enumerateSD(std::vector<TInfo> *fTapeFileList, bool fTape);
                FRESULT     enumerateSD(std::vector<TInfo> *fTapeFileList, const char *fRoot, bool fTape);
                FRESULT     enumerateFlash(std::vector<TInfo> *fTapeFileList, bool fTape);
                FRESULT     enumerateFlash(std::vector<TInfo> *fTapeFileList, const char *fRoot, bool fTape);
                // FRESULT     openForRW(TFile *fFile, TPath *fPath);
    public:
                TPath       Path;
                
        static  FRESULT     TranslateError(int fPicoErrorCode);
        static  const char *GetErrorText(FRESULT fResult);
        static  size_t      GetFileSize(TPath fPath);
                FRESULT     renameFile(const char *fOldFileName, const char *fNewFileName, eDevice fDevice);
 
        static  FRESULT     MountSD();
        static  FRESULT     MountFlash();
        static  FRESULT     UnmountSD();
        static  FRESULT     UnmountFlash();
};
