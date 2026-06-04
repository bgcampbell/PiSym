#pragma once

#include "PiSym.h"

const uint MaxTapeLength = 255;
const uint MaxFileLength = 8;

enum eDevice { Default, SD, Flash };

class TPath
{
    private:
        eDevice     device;
        std::string tape;
        std::string file;
        uint16_t    sequenceNr;

    public:
        TPath(TPath *fSource);
    /// @brief Gets the device type
    /// @return The device type
        inline eDevice GetDevice()
        {
            return device;
        }

        /// @brief Sets the device type
        /// @param fDevice      The device type to set
        /// @return             True if the device was set successfully, false otherwise

        inline bool SetDevice(eDevice fDevice)
        {
            device = fDevice;
            return true;
        }

        /// @brief Gets the tape name
        /// @return The tape name

        inline std::string GetTape()
        {
            return tape;
        }

        /// @brief Sets the tape name
        /// @param fTapeName    The tape name to set
        /// @return             True if the tape name was set successfully, false otherwise

        bool SetTape(const char *fTapeName);

        // inline bool SetTape(std::string *fTapeName)
        // {
        //     if(!IsValidTapeName(fTapeName))
        //         return false;
        //     tape = fTapeName;
        //     return true;
        // }

        /// @brief Gets the file name
        /// @return The file name

        inline std::string GetFile()
        {
            return file;
        }

        /// @brief Sets the file name
        /// @param fFileName    The file name to set
        /// @return             True if the file name was set successfully, false otherwise

        inline bool SetFile(const char *fFileName)
        {
            if(!IsValidFileName(fFileName))
                return false;
            file = std::string(fFileName);
            return true;
        }

        inline bool SetFile(int fFileName)
        {
            char fileName[5];

            sprintf(fileName, "%02X", fFileName & 0xFF);
            file = std::string(fileName);
            return true;
        }

        /// @brief Gets the sequence number
        /// @return The sequence number

        inline uint16_t GetSequenceNr()
        {
            return sequenceNr;
        }

        /// @brief Sets the sequence number
        /// @param fSequenceNr      The sequence number to set
        /// @return                 True if the sequence number was set successfully, false otherwise

        inline bool SetSequenceNr(uint16_t fSequenceNr)
        {
            if(fSequenceNr > 9999)
                return false;
            sequenceNr = fSequenceNr;
            return true;
        }

        /// @brief Gets the filename with the sequence number prepended to use as a filename on the device
        /// @return The filename with the sequence number

        inline std::string GetDriveFilename()
        {
            return GetPath(false, true, false);
        }

        // inline const char *FullPathString()
        // {
        //     return GetPath(true, true, true).c_str();
        // }
        
        // inline const char *FullTapeString()
        // {
        //     return GetTapePath(true).c_str();
        // }

        inline std::string GetDeviceName()
        {
            return GetDeviceName(device);
        }

        TPath(const char *Pathname = NULL);
        TPath(const TPath *fSource);

        std::string GetDeviceName(eDevice fDevice);
        int         GetFileInt();
        bool        SetFileSequenceNr(const char *fFileName);
        static bool ExtractFileSequenceNr(const char *fDriveFilename, std::string *fFilename, uint16_t *fSequenceNr);

        std::string GetPath(bool fIncludeTape, bool fIncludeSeqNr = false, bool fIncludeDevice = false);
        std::string GetTapePath(bool fIncludeDevice, bool fIncludeSlash = true);
        static bool IsValidTapeName(const char *fTapeName);
        static bool IsValidFileName(const char *fFileName);
};
