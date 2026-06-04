#include <algorithm>
#include "Path.hpp"

/// @brief Constructor for creating a TPath object from a path string
/// @param fPathname The full path string, which may include device, tape, file, and sequence number information

TPath::TPath(const char *fPathname)
{
    device = eDevice::Default;
    tape = "";
    file = "";
    sequenceNr = 0;

    if(Configuration.UseExternalSD)
        device = eDevice::SD;
    if(Configuration.UseInternalFlash)
        device = eDevice::Flash;

    if (fPathname == NULL)
        return;

    std::string pathname = fPathname;
    size_t      colonPos;
    size_t      slashPos;
    size_t      dotPos;
    char       *p;

    std::transform(pathname.begin(), pathname.end(), pathname.begin(), [](unsigned char c)
    {
         return std::tolower(c); 
    });    
    
    if((colonPos = pathname.find(":")) > 0)
    {
        if(pathname.find("sd:") == 0)
            device = eDevice::SD;
        else if(pathname.find("flash:") == 0)
            device = eDevice::Flash;
        pathname = pathname.substr(colonPos + 1);
    }
    if((slashPos = pathname.find("/")) >= 0)
    {
        if(slashPos > 0)
            tape = pathname.substr(0, slashPos);
        pathname = pathname.substr(slashPos + 1);
    }
    if((dotPos = pathname.find(".")) >= 0)
    {
        if(dotPos == 0)
            file = pathname;
        else
        {
            file = pathname.substr(dotPos + 1);
            sequenceNr = strtol(pathname.substr(0, dotPos).c_str(), &p, 10);
        }
    }
}

/// @brief Copy constructor for creating a TPath object from another TPath object
/// @param fSource A pointer to the TPath object to copy

TPath::TPath(TPath *fSource)
{
    device = fSource->device;
    tape = fSource->tape;
    file = fSource->file;
    sequenceNr = fSource->sequenceNr;
}

/// @brief Sets the tape name for the path
/// @param fTapeName The name of the tape
/// @return True if successful, false otherwise

bool TPath::SetTape(const char *fTapeName)
{
    std::string tapename = std::string(fTapeName);

    if(!tapename.empty() && (tapename[0] == '/'))
        tapename = tapename.substr(1);
    if(!IsValidTapeName(tapename.c_str()))
        return false;
    tape = tapename;
    return true;
}

int TPath::GetFileInt()
{
    char    *p;
    int     result = strtol(GetFile().c_str(), &p, 16);

    if(*p != 0)
        return -result;
    return result;
}

/// @brief Sets the file name and sequence number for the path based on a given file name string
/// @param fFileName The full file name string
/// @return True if successful, false otherwise

bool TPath::SetFileSequenceNr(const char *fFileName)
{
    std::string filename;
    uint16_t    sequenceNr;

    // printf("\r\nTPath::SetFileSequenceNr() [1]: fFilename = %s, Path = %s\r\n", fFileName.c_str(), FullPathString());

    if(!ExtractFileSequenceNr(fFileName, &filename, &sequenceNr))
        return false;
    
        SetSequenceNr(sequenceNr);
    SetFile(filename.c_str());

    return true;
}

/// @brief Extracts the file name and sequence number from a given file name string
/// @param fDriveFilename The full file name string
/// @param fFilename A pointer to store the extracted file name
/// @param fSequenceNr A pointer to store the extracted sequence number
/// @return True if successful, false otherwise

bool TPath::ExtractFileSequenceNr(const char *fDriveFilename, std::string *fFilename, uint16_t *fSequenceNr)
{
    std::string filename = std::string(fDriveFilename);

    if(!filename.empty() && (filename[0] == '/'))
        filename = filename.substr(1);

    int dotPos = filename.find(".");

    if(dotPos < 0)
    {
        if(fSequenceNr != NULL)
            *fSequenceNr = 0;
        if(fFilename != NULL)
            *fFilename = filename;
        return true;
    }
    if(dotPos == 0)
    {
        if(fSequenceNr != NULL)
            *fSequenceNr = 0;
        if(fFilename != NULL)
            *fFilename = filename.substr(1);
        return true;   
    }
    else
    {
        std::string seqNrText = filename.substr(0, dotPos);
        std::string fileText  = filename.substr(dotPos + 1);
        char *p;
        bool result = true;

        if(fSequenceNr != NULL)
        {
            *fSequenceNr = strtol(seqNrText.c_str(), &p, 10);
            result &= (*p == 0);
        }
        result &= IsValidFileName(fileText.c_str());
        
        if(fFilename != NULL)
            *fFilename = fileText;

        return result;
    }
}

/// @brief Gets the device name as a string based on the device type
/// @param fDevice The device type
/// @return The device name as a string

std::string TPath::GetDeviceName(eDevice fDevice)
{
    std::string     result;

    eDevice dev = fDevice;

    if(dev == eDevice::Default)
    {
        if(Configuration.UseExternalSD)
            dev = eDevice::SD;
        else if (Configuration.UseInternalFlash)
            dev = eDevice::Flash;
    }
    switch (dev)
    {
        case eDevice::Default:
            // Error case : No cassette redirect
            result = std::string("cassette:");
            break;
        case eDevice::SD:
            result = std::string("sd:");
            break;
        case eDevice::Flash:
            result = std::string("flash:");
            break;
    }
    return result;
}

/// @brief Gets the full path string based on the specified inclusion flags
/// @param fIncludeTape         Whether to include the tape name in the path
/// @param fIncludeSeqNr        Whether to include the sequence number in the path
/// @param fIncludeDevice       Whether to include the device name in the path
/// @return                     The full path string

std::string TPath::GetPath(bool fIncludeTape, bool fIncludeSeqNr, bool fIncludeDevice)
{
    std::string result = file;
    char        seqNrText[5];

    if(fIncludeSeqNr)
    {
        sprintf(seqNrText, "%04d.", sequenceNr % 9999);
        result = std::string(seqNrText) + file;
    }
    if(fIncludeTape)
    {
        if(tape.empty())
            result = std::string("/") + result;
        else
            result = std::string("/") + tape + std::string("/") + result;
    }
    if(fIncludeDevice)
        result = GetDeviceName() + result;

    return result;
}

/// @brief Gets the tape path string based on the specified inclusion flag
/// @param fIncludeDevice       Whether to include the device name in the path
/// @return                     The tape path string

std::string TPath::GetTapePath(bool fIncludeDevice, bool fIncludeSlash)
{
    std::string result = std::string(fIncludeSlash ? "/" : "") + tape;

    if(fIncludeDevice)
        result = GetDeviceName() + result;
    return result;
}

// /// @brief Determines whether a given tape name is valid based on length and character restrictions
// /// @param fTapeName    The tape name to validate
// /// @return             True if the tape name is valid, false otherwise

// bool TPath::IsValidTapeName(std::string *fTapeName)
// {
//     if(fTapeName.length() > MaxTapeLength)
//         return false;

//     for(int i = 0; i < fTapeName.length(); i++)
//     {
//         char c = fTapeName[i];

//         if(isalnum(c))
//             continue;
//         if((c != '_') && (c != ' '))
//             return false;
//     }
//     return true;
// }

/// @brief Determines whether a given tape name is valid based on length and character restrictions
/// @param fTapeName    The tape name to validate
/// @return             True if the tape name is valid, false otherwise

bool TPath::IsValidTapeName(const char *fTapeName)
{
    if(strlen(fTapeName) > MaxTapeLength)
        return false;

    for(int i = 0; i < strlen(fTapeName); i++)
    {
        char c = fTapeName[i];

        if(isalnum(c))
            continue;
        if(c != '_')
            return false;
    }
    return true;
}

/// @brief Determines whether a given file name is valid based on length and character restrictions
/// @param fFileName    The file name to validate
/// @return             True if the file name is valid, false otherwise

bool TPath::IsValidFileName(const char *fFileName)
{
    if((strlen(fFileName) < 1) || (strlen(fFileName) > MaxFileLength))
        return false;

    for(int i = 0; i < strlen(fFileName); i++)
    {
        char c = fFileName[i];

        if(isalnum(c))
            continue;
        if(c != '_')
            return false;
    }
    return true;
}