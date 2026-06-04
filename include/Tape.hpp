#pragma once

#include <cstdint>
#include "pico_hal.h"
#include "FatFsSd.h"
#include "PiSym.h"
#include "Path.hpp"
#include "TapeFileBase.hpp"

class TFile;

class TTape : public TTapeFileBase
{
    public:
        uint16_t    CurrentSequenceNr;
        
    private:
        /// @brief Finds a file on the tape with the specified name and minimum sequence number
        /// @param fPath                A pointer to the path object specifying the file to find
        /// @param fMinimumSequenceNr   The minimum sequence number of the file to find
        /// @return                     The information of the found file, or an invalid TInfo if not found

        inline TInfo         findFile(TPath *fPath, uint16_t fMinimumSequenceNr)
        {
            return FindFile(fPath->GetFile().c_str(), fPath->GetTape().c_str(), fMinimumSequenceNr);
        }

        
    public:
        // Tape Functions
        
        FRESULT Load();
        FRESULT Load(const char *fTapeName);
        FRESULT LoadRoot();
        uint16_t GetNextSequenceNr(const char *fTapeName = NULL);
        
        // bool Create(std::string *fNewTape, eDevice fDevice = eDevice::Default);
        bool Create(const char *fNewTape, eDevice fDevice = eDevice::Default);
        
        // bool DeleteTape(std::string *fTapeName, bool fForce, eDevice fDevice = eDevice::Default);
        bool DeleteTape(const char *fTapeName, bool fForce, eDevice fDevice = eDevice::Default);
        
        std::vector<TInfo>          FindFiles(const char *fFilename, const char *fTapeName);

        std::vector<TInfo>          FindFileBySequenceNr(int fSequenceNr, const char *fTapeName);

        /// @brief Finds all files on the tape with the specified name
        /// @param fPath                A pointer to the path object specifying the file name and tape to search
        /// @return                     A vector containing the information of all found files
        
        inline std::vector<TInfo>   FindFiles(TPath *fPath)
        {
            return FindFiles(fPath->GetFile().c_str(), fPath->GetTape().c_str());
        }
        bool ListTapes(std::vector<TInfo> *fTapeList, eDevice fDevice = eDevice::Default);
        
        bool ListFiles(std::vector<TInfo> *fFileList, const char *fTapeName = NULL, eDevice fDevice = eDevice::Default);
        
        bool RenameTape(const char *fOldTapeName, const char *fNewTapeName, eDevice fDevice = eDevice::Default);
        bool ContainsFiles(const char *fTapeName = NULL, eDevice fDevice = eDevice::Default);
        bool Renumber(int fStart, int fStep);
        
        // File Functions : Close is in TFile Object
        TInfo   FindFile(const char *fFilename, const char *fTapeName, uint16_t fMinimumSequenceNr);
        FRESULT OpenPath(TFile *fFile, TPath *fPath, bool fWrite);
        FRESULT OpenFileName(TFile *fFile, const char *fFilename, bool fWrite, eDevice fDevice = eDevice::Default);
        
        FRESULT CreateFileName(TFile *fFile, const char *fFilename, eDevice fDevice = eDevice::Default);
        
        bool DeleteFileEntry(const char *fFilename, eDevice fDevice);
        bool DeleteFileByName(TPath *fFile);
        bool DeleteFileByName(int fFileName, const char *fTapeName, eDevice fDevice = eDevice::Default);
        bool DeleteFileBySequenceNr(TPath *fFile);
        bool DeleteFileBySequenceNr(int fSequenceNr, const char *fTapeName, eDevice fDevice = eDevice::Default);

        bool DeleteAllFiles(eDevice fDevice = eDevice::Default);
        
        bool RenameFile(const char *fOldFileName, const char *fNewFileName);
        bool RenameFileBySequenceNr(int fSequenceNr, const char *fNewFileName);
        bool RenameTape(const char *fOldTapeName, const char *fNewTapeName);
};