#include "Tape.hpp"
#include "File.hpp"
#include <algorithm>

/// @brief Finds a file on the tape with the specified name and minimum sequence number
/// @param fFileName            The name of the file to find
/// @param fTapeName            The name of the tape to search on (optional, default is current tape)
/// @param fMinimumSequenceNr   The minimum sequence number to consider
/// @return                     The found file information or an empty TInfo if not found

TInfo TTape::FindFile(const char *fFileName, const char *fTapeName, uint16_t fMinimumSequenceNr)
{
    std::vector<TInfo>  fileList;
    TPath               path;

    if(!ListFiles(&fileList, fTapeName, CurrentTape.Path.GetDevice()))
        return TInfo();

    std::sort(fileList.begin(), fileList.end(), [](TInfo fA, TInfo fB) { return fA.Name.compare(fB.Name) <= 0; });

    for(int i = 0; i < fileList.size(); i++)
    {
        path.SetFileSequenceNr(fileList[i].Name.c_str());

        if(path.GetFile() != fFileName)
            continue;
        if(path.GetSequenceNr() >= fMinimumSequenceNr)
            return fileList[i];
    }

    return TInfo();
}

/// @brief Finds files on the tape with the specified name
/// @param fFileName            The name of the files to find
/// @param fTapeName            The name of the tape to search on (optional, default is current tape)
/// @return                     A vector of found file information or an empty vector if none found

std::vector<TInfo> TTape::FindFiles(const char *fFileName, const char *fTapeName)
{
    std::vector<TInfo>  result;
    std::vector<TInfo>  fileList;
    TPath               path;

    if(!ListFiles(&fileList, fTapeName, CurrentTape.Path.GetDevice()))
        return std::vector<TInfo>();

    std::sort(fileList.begin(), fileList.end(), [](TInfo fA, TInfo fB) { return fA.Name.compare(fB.Name) <= 0; });

    for(int i = 0; i < fileList.size(); i++)
    {
        path.SetFileSequenceNr(fileList[i].Name.c_str());

        if(path.GetFile() == fFileName)
            result.push_back(fileList[i]);
    }
    return result;
}

std::vector<TInfo> TTape::FindFileBySequenceNr(int fSequenceNr, const char *fTapeName)
{
    std::vector<TInfo>  result;
    std::vector<TInfo>  fileList;
    TPath               path;

    if(!ListFiles(&fileList, fTapeName, CurrentTape.Path.GetDevice()))
        return std::vector<TInfo>();

    std::sort(fileList.begin(), fileList.end(), [](TInfo fA, TInfo fB) { return fA.Name.compare(fB.Name) <= 0; });

    for(int i = 0; i < fileList.size(); i++)
    {
        path.SetFileSequenceNr(fileList[i].Name.c_str());

        if(path.GetSequenceNr() == fSequenceNr)
            result.push_back(fileList[i]);
    }
    return result;
}

/// @brief Gets the next available sequence number for a file on the tape
/// @param fTapeName            The name of the tape to search on (optional, default is current tape)
/// @return                     The next available sequence number

uint16_t TTape::GetNextSequenceNr(const char *fTapeName)
{
    std::vector<TInfo>  fileList;
    TPath               path(&CurrentTape.Path);

    if(fTapeName != NULL)
        path.SetTape(fTapeName);

    if(!ListFiles(&fileList, path.GetTape().c_str(), CurrentTape.Path.GetDevice()))
        return 1;    // Default if error

    if(fileList.size() == 0)
         return 1;    // Default if no files

    if(!ListFiles(&fileList, path.GetTape().c_str(), CurrentTape.Path.GetDevice()))
        return 1;

    uint16_t    sequenceNr;
 
    if(!TPath::ExtractFileSequenceNr(fileList.back().Name.c_str(), NULL, &sequenceNr))
        return 0;
    return sequenceNr + 1;
}

/// @brief Loads the tape
/// @return                     The result of the load operation

FRESULT TTape::Load()
{
    return Load(CurrentTape.Path.GetTape().c_str());
}

/// @brief Loads a specific tape by name
/// @param fTapeName            The name of the tape to load
/// @return                     The result of the load operation

FRESULT TTape::Load(const char *fTapeName)
{
    if(Path.GetDevice() == eDevice::SD)
    {
        FRESULT result;

        MountSD();
        result = f_chdir(fTapeName);
        if(result == FR_OK)
        {
            Path.SetTape(fTapeName);
            CurrentSequenceNr = 0;
        }
        return result;
    }
    if(Path.GetDevice() == eDevice::Flash)
    {
        MountFlash();
        int result = pico_dir_open(fTapeName);

        if(result >= 0)
        {
            flashDirectory = result;
            Path.SetTape(fTapeName);
            CurrentSequenceNr = 0;
            return FR_OK;
        }
        else
            return TranslateError(result);
    }
    return FR_INVALID_PARAMETER;
}

// /// @brief 
// /// @param fTapeName 
// /// @return 
// FRESULT TTape::Load(std::string *fTapeName)
// {
//     Path.SetTape(fTapeName);
//     return Load();
// }

/// @brief Loads the root tape
/// @return                     The result of the load operation

FRESULT TTape::LoadRoot()
{
    return Load("");
 }

/// @brief Creates a new tape
/// @param fNewTape             The name of the new tape to create
/// @param fDevice              The device on which to create the tape
/// @return                     True if the tape was created successfully, false otherwise

bool TTape::Create(const char *fNewTape, eDevice fDevice)
{
    if(!TPath::IsValidTapeName(fNewTape))
        return false;
    
    if(fDevice != eDevice::Default)
        CurrentTape.Path.SetDevice(fDevice);

    if(tapeExists(fNewTape, true, fDevice))
        return false;

    if(CurrentTape.Path.GetDevice() == eDevice::SD)
    {
        MountSD();
        if(f_chdir("/") != FR_OK)
            return false;
        
        if(f_mkdir(fNewTape) != FR_OK)
            return false;
    }
    else if(CurrentTape.Path.GetDevice() == eDevice::Flash)
    {
        MountFlash();
        flashDirectory = pico_dir_open("/");

        // printf("Creating Flash %s...\r\n", fNewTape.c_str());
        if(pico_mkdir(fNewTape) < 0)
            return false;

        if(pico_dir_close(flashDirectory) < 0)
            return false;
    }
    else
        return false;

    if(!CurrentTape.Path.SetTape(fNewTape))
        return false;
    return Load() == FR_OK;
}

// /// @brief Deletes a tape by name
// /// @param fTapeName            The name of the tape to delete
// /// @param fForce               Whether to force deletion even if the tape contains files
// /// @param fDevice              The device on which the tape exists
// /// @return                     True if the tape was deleted successfully, false otherwise

// bool TTape::DeleteTape(const char *fTapeName, bool fForce, eDevice fDevice)
// {
//     return DeleteTape(std::string(fTapeName), fForce, fDevice);
// }

/// @brief Checks if a tape contains any files
/// @param fTapeName            The name of the tape to check
/// @param fDevice              The device on which the tape exists
/// @return                     True if the tape contains files, false otherwise

bool TTape::ContainsFiles(const char *fTapeName, eDevice fDevice)
{
    std::vector<TInfo> fileList;

    if(!ListFiles(&fileList, fTapeName, fDevice))
         return true;    // Default if error
    return fileList.size() > 0;
}

/// @brief Deletes a tape by name
/// @param fTapeName            The name of the tape to delete
/// @param fForce               Whether to force deletion even if the tape contains files
/// @param fDevice              The device on which the tape exists
/// @return                     True if the tape was deleted successfully, false otherwise

bool TTape::DeleteTape(const char *fTapeName, bool fForce, eDevice fDevice)
{
    char    tape[FULL_TAPE_PATH_LENGTH];

    if((fTapeName == NULL) || (strlen(fTapeName) == 0) || (strcmp(fTapeName, "/") == 0))
        return false;   // Tape must be specified and cannot be root
    if(fDevice != eDevice::Default)
        CurrentTape.Path.SetDevice(fDevice);
    if(!CurrentTape.Path.SetTape(fTapeName))
        return false;
    if(fForce)
    {
        if(!DeleteAllFiles())
            return false;
    }
    // LoadRoot();
    // printf("\r\nRoot loaded ");
    strcpy(tape, CurrentTape.Path.GetTapePath(false).c_str());
    if(CurrentTape.Path.GetDevice() == eDevice::SD)
    {
        FRESULT result = f_unlink(tape);  // Will fail if !fForce and tape contains files
        
        LoadRoot();
        return result == FR_OK;
    }
    if(CurrentTape.Path.GetDevice() == eDevice::Flash)
    {
        int result = pico_remove(tape);  // Will fail if !fForce and tape contains files

        LoadRoot();
        return result >= 0;
    }
    return false;
}

// /// @brief Deletes a tape by name
// /// @param fTapeName            The name of the tape to delete
// /// @param fForce               Whether to force deletion even if the tape contains files
// /// @param fDevice              The device on which the tape exists
// /// @return                     True if the tape was deleted successfully, false otherwise

// bool TTape::DeleteTape(std::string *fTapeName, bool fForce, eDevice fDevice)
// {
//     return DeleteTape(fTapeName.c_str(), fForce, fDevice);
// }

/// @brief Lists all tapes on a given device
/// @param fTapeList            A vector to store the list of tapes
/// @param fDevice              The device on which to list tapes
/// @return                     True if the operation was successful, false otherwise

bool TTape::ListTapes(std::vector<TInfo> *fTapeList, eDevice fDevice)
{
    if(isSD(fDevice))
        return enumerateSD(fTapeList, true);
    if(isFlash(fDevice))
        return enumerateFlash(fTapeList, true);
    return false;
}

/// @brief Renames a tape
/// @param fOldTapeName         The current name of the tape
/// @param fNewTapeName         The new name for the tape
/// @param fDevice              The device on which the tape exists
/// @return                     True if the tape was renamed successfully, false otherwise

bool TTape::RenameTape(const char *fOldTapeName, const char *fNewTapeName, eDevice fDevice)
{
    FRESULT result = FR_OK;

    if(!TPath::IsValidTapeName(fNewTapeName))
    {
        printf("\r\n%s is not a valid tape name\r\n", fNewTapeName);
        return false;
    }
    if(isSD(fDevice))
    {
        MountSD();

        result = f_rename(fOldTapeName, fNewTapeName);
    }
    else if(isFlash(fDevice))
    {
        MountFlash();

        result = TranslateError(pico_rename(fOldTapeName, fNewTapeName));
    }
    else
        return false;

    if((CurrentTape.Path.GetTape() == fOldTapeName) && (result == FR_OK))
        CurrentTape.Path.SetTape(fNewTapeName);

    return result == FR_OK;
}

/// @brief Renumbers files on the tape with a specified starting sequence number and step
/// @param fStart               The starting sequence number for renumbering
/// @param fStep                The step size for renumbering
/// @return                     True if the files were renumbered successfully, false otherwise

bool TTape::Renumber(int fStart, int fStep)
{
    std::vector<TInfo>  fileList;
    TPath               path;
    char                newName[256];
    std::string         finalName;

    if(!ListFiles(&fileList))
        return false;

    // Ensure max sequence number will not be exceeded
    if(((fStep - 1) * fileList.size() + fStart) > 9999)
        return false;

    // Use temporary suffix to avoid name comflicts
    for(int i = 0, seqNr = fStart; i < fileList.size(); i++, seqNr += fStep)
    {
        path = CurrentTape.Path;
        path.SetFileSequenceNr(fileList[i].Name.c_str());
        sprintf(newName, "%04d.%sR", seqNr, path.GetFile().c_str());
        if(renameFile(path.GetPath(false, true).c_str(), newName, path.GetDevice()) != FR_OK)
            return false;
        }
    // Now remove the prefix
    if(!ListFiles(&fileList))
        return false;
    for(int i = 0; i < fileList.size(); i++)
    {
        path = CurrentTape.Path;
        path.SetFileSequenceNr(fileList[i].Name.c_str());

        finalName = path.GetPath(false, true);
        if(renameFile(finalName.c_str(), finalName.substr(0, finalName.length() - 1).c_str(), path.GetDevice()) != FR_OK)
            return false;
    }
    return true;
}
// File Functions

/// @brief Opens a file on the tape with the specified path and mode
/// @param fFile                A pointer to the file object for the opened file
/// @param fPath                A pointer to the path object specifying the file location
/// @param fWrite               Whether to open the file for writing
/// @return                     The result of the operation

FRESULT TTape::OpenPath(TFile *fFile, TPath *fPath, bool fWrite)
{
    FRESULT result;
    std::string pathName;

    fFile->Tape = this;
    fFile->sequenceNr = fPath->GetSequenceNr();

    // Load();
    pathName = fPath->GetPath(true, true, false);
    char cwd[256];
    f_getcwd(cwd, sizeof(cwd));
    if(fPath->GetDevice() == eDevice::SD)
    {
        MountSD();
        fFile->device = eDevice::SD;
        result = f_open(&(fFile->fatfsFile), pathName.c_str(), fWrite ? FA_WRITE | FA_CREATE_ALWAYS : FA_READ);
    }
    else if(fPath->GetDevice() == eDevice::Flash)
    {
        MountFlash();
        int response = pico_open(pathName.c_str(), fWrite ? LFS_O_CREAT | LFS_O_WRONLY : LFS_O_RDONLY);

        fFile->device = eDevice::Flash;
        if(response >= 0)
        {
            fFile->flashFileHandle = response;
            result = FR_OK;
        }
        else
        {
            fFile->flashDirectory = -1;
            result = TranslateError(response);
        }
    }
    else
        return FR_INT_ERR;

    if(result != FR_OK)
        return result;

    // may be other stuff required
    
    return FR_OK;    
}

/// @brief Opens a file on the tape with the specified filename and mode
/// @param fFile                A pointer to the file object for the opened file
/// @param fFilename            The name of the file to open
/// @param fWrite               Whether to open the file for writing
/// @param fDevice              The device on which the file exists
/// @return                     The result of the operation

FRESULT TTape::OpenFileName(TFile *fFile, const char *fFilename, bool fWrite, eDevice fDevice)
{
    CurrentTape.Path.SetFileSequenceNr(fFilename);

    return OpenPath(fFile, &CurrentTape.Path, fWrite);
}

/// @brief Creates a file on the tape with the specified filename
/// @param fFile                A pointer to the file object for the created file
/// @param fFileName            The name of the file to create
/// @param fDevice              The device on which to create the file
/// @return                     The result of the operation

FRESULT TTape::CreateFileName(TFile *fFile, const char *fFileName, eDevice fDevice)
{
    FRESULT result;
    TPath path = CurrentTape.Path;
    TInfo file;
    std::string pathName;

    path.SetFile(fFileName);
    path.SetSequenceNr(GetNextSequenceNr(path.GetTape().c_str()));

    printf("\r\nCreate file path = %s\r\n", path.GetPath(true, true, true).c_str());

    return OpenPath(fFile, &path, true);
}

/// @brief Lists all files on the tape with the specified tape name and device
/// @param fFileList            A pointer to the vector to store the file information
/// @param fTapeName            The name of the tape to list files from
/// @param fDevice              The device on which to list files
/// @return                     True if successful, false otherwise

bool TTape::ListFiles(std::vector<TInfo> *fFileList, const char *fTapeName, eDevice fDevice)
{
    if((fTapeName == NULL) || (strlen(fTapeName) == 0))
    {
        if(Configuration.UseInternalFlash)
    	    CurrentTape.Path.SetDevice(eDevice::Flash);
        else if(Configuration.UseExternalSD)
    	    CurrentTape.Path.SetDevice(eDevice::SD);

        if(isSD(fDevice))
        {
            if(enumerateSD(fFileList, false) != FR_OK)
                return false;
        }
        else if(isFlash(fDevice))
        {
            if(enumerateFlash(fFileList, false) != FR_OK)
                return false;
        }
        else
            return false;
            
        std::sort(fFileList->begin(), fFileList->end(), [](TInfo fA, TInfo fB) { return fA.Name.compare(fB.Name) <= 0; });
        return true;
    }
    else
    {
        if(fTapeName[0] == '/')
            fTapeName++;

        if(isSD(fDevice))
        {
            if(enumerateSD(fFileList, fTapeName, false) != FR_OK)
                return false;
        }
        else if(isFlash(fDevice))
        {
            if(enumerateFlash(fFileList, fTapeName, false) != FR_OK)
                return false;
        }
        else
            return false;

        std::sort(fFileList->begin(), fFileList->end(), [](TInfo fA, TInfo fB) { return fA.Name.compare(fB.Name) <= 0; });
        return true;
    }
    return false;
}

/// @brief Deletes a file entry from the specified device
/// @param fFilename            The name of the file to delete
/// @param fDevice              The device from which to delete the file
/// @return                     True if successful, false otherwise

bool TTape::DeleteFileEntry(const char *fFilename, eDevice fDevice)
{
    if(isSD(fDevice))
    {   
        CurrentTape.MountSD();
        return f_unlink(fFilename) == FR_OK;
    }
    if(isFlash(fDevice))
    {
        int dir;

        CurrentTape.MountFlash();
        if((dir = pico_dir_open(Path.GetTapePath(false).c_str())) < 0)
            return false;
        if(pico_remove(fFilename) < 0)
        {
            pico_dir_close(dir);
            return false;
        }
        return pico_dir_close(dir) >= 0;
    }
    return false;
}

/// @brief Deletes a file by name from the tape
/// @param fFile                A pointer to the path object specifying the file to delete
/// @return                     True if successful, false otherwise

bool TTape::DeleteFileByName(TPath *fFile)
{
    std::vector<TInfo> fileList;
    TPath              fileEntry(&CurrentTape.Path);

    if(!ListFiles(&fileList, fFile->GetTape().c_str(), fFile->GetDevice()))
        return false;
    
    for(int i = 0; i < fileList.size(); i++)
    {
         if(!fileEntry.SetFileSequenceNr(fileList[i].Name.c_str()))
            return false;

        if(fileEntry.GetFile() == fFile->GetFile())
        {
            if(!DeleteFileEntry(fileEntry.GetPath(true, true).c_str(), fileEntry.GetDevice()))
                return false;
        }
    }
    return true;
}

/// @brief Deletes a file by name from the tape
/// @param fFileName            The name of the file to delete
/// @param fTapeName            The name of the tape from which to delete the file
/// @param fDevice              The device from which to delete the file
/// @return                     True if successful, false otherwise

bool TTape::DeleteFileByName(int fFileName, const char *fTapeName, eDevice fDevice)
{
    TPath   path = CurrentTape.Path;
    bool    result = true;
    
    result &= path.SetFile(fFileName);
    result &= path.SetTape(fTapeName);
    result &= path.SetDevice(fDevice);

    return DeleteFileByName(&path);
}

/// @brief Deletes a file by its sequence number from the tape
/// @param fFile                A pointer to the path object specifying the file to delete
/// @return                     True if successful, false otherwise

bool TTape::DeleteFileBySequenceNr(TPath *fFile)
{
    std::vector<TInfo> fileList;
    TPath              fileEntry(&CurrentTape.Path);

    if(!ListFiles(&fileList, fFile->GetTape().c_str(), fFile->GetDevice()))
        return false;
    
    for(int i = 0; i < fileList.size(); i++)
    {
        if(!fileEntry.SetFileSequenceNr(fileList[i].Name.c_str()))
            return false;

        if(fileEntry.GetSequenceNr() == fFile->GetSequenceNr())
            return DeleteFileEntry(fileEntry.GetPath(true, true).c_str(), fFile->GetDevice());
    }
    return false;
}

/// @brief Deletes a file by its sequence number from the tape
/// @param fSequenceNr          The sequence number of the file to delete
/// @param fTapeName            The name of the tape from which to delete the file
/// @param fDevice              The device from which to delete the file
/// @return                     True if successful, false otherwise

bool TTape::DeleteFileBySequenceNr(int fSequenceNr, const char *fTapeName, eDevice fDevice)
{
    TPath   path;
    bool    result = true;

    result &= path.SetSequenceNr(fSequenceNr);
    result &= path.SetTape(fTapeName);
    result &= path.SetDevice(fDevice);

    return DeleteFileBySequenceNr(&path);
}

/// @brief Deletes all files from the current tape
/// @param fDevice              The device from which to delete all files
/// @return                     True if successful, false otherwise

bool TTape::DeleteAllFiles(eDevice fDevice)
{
    std::vector<TInfo>    fileList;
    bool                  result = true;

    if(!ListFiles(&fileList, NULL, fDevice))
        return false;

    for(int i = 0; i < fileList.size(); i++)
        result &= DeleteFileEntry(fileList[i].Name.c_str(), fDevice);
    return result;
}

/// @brief Renames a file on the tape
/// @param fOldFileName         The current name of the file
/// @param fNewFileName         The new name for the file
/// @param fDevice              The device on which to rename the file
/// @return                     True if successful, false otherwise

bool TTape::RenameFile(const char *fOldFileName, const char *fNewFileName)
{
    std::vector<TInfo>  fileList = CurrentTape.FindFiles(fOldFileName, CurrentTape.Path.GetTape().c_str());
    TPath               oldPath;
    TPath               newPath;
    TFile               file;
    FRESULT             result;

    if(!TPath::IsValidFileName(fNewFileName))
    {
        printf("\r\n%s is not a valid file name\r\n", fNewFileName);
        return false;
    }

    if(fileList.empty())
        return false;

    for(int i = 0; i < fileList.size(); i++)
    {
        oldPath = CurrentTape.Path;

        oldPath.SetFileSequenceNr(fileList[i].Name.c_str());
        newPath = oldPath;
        newPath.SetFile(fNewFileName);

        if((result = renameFile(fileList[i].Name.c_str(), newPath.GetPath(false, true).c_str(), Path.GetDevice())) != FR_OK)
        {
            printf("File rename error %s\r\n", GetErrorText(result));
            return false;
        }
    }
    return true;
}

bool TTape::RenameFileBySequenceNr(int fSequenceNr, const char *fNewFileName)
{
    std::vector<TInfo>  fileList = CurrentTape.FindFileBySequenceNr(fSequenceNr, CurrentTape.Path.GetTape().c_str());
    TPath               oldPath;
    TPath               newPath;
    TFile               file;
    FRESULT             result;

    if(!TPath::IsValidFileName(fNewFileName))
    {
        printf("\r\n%s is not a valid file name\r\n", fNewFileName);
        return false;
    }

    if(fileList.empty())
        return false;

    for(int i = 0; i < fileList.size(); i++)
    {
        oldPath = CurrentTape.Path;

        oldPath.SetFileSequenceNr(fileList[i].Name.c_str());
        newPath = oldPath;
        newPath.SetFile(fNewFileName);

        if((result = renameFile(fileList[i].Name.c_str(), newPath.GetPath(false, true).c_str(), Path.GetDevice())) != FR_OK)
        {
            printf("File rename error %s\r\n", GetErrorText(result));
            return false;
        }
    }
    return true;
}
/// @brief Renames a tape
/// @param fOldTapeName         The current name of the tape
/// @param fNewTapeName         The new name for the tape
/// @param fDevice              The device on which to rename the tape
/// @return                     True if successful, false otherwise

bool TTape::RenameTape(const char *fOldTapeName, const char *fNewTapeName)
{
    std::vector<TInfo>  tapeList;
    TPath               oldPath;
    TPath               newPath;
    TFile               file;
    FRESULT             result;

    CurrentTape.ListTapes(&tapeList);

    if(!TPath::IsValidTapeName(fNewTapeName))
    {
        printf("\r\n%s is not a valid tape name\r\n", fNewTapeName);
        return false;
    }

    if(tapeList.empty())
        return false;
    for(int i = 0; i < tapeList.size(); i++)
    {
        if(tapeList[i].Name != fOldTapeName)
            continue;

        oldPath = CurrentTape.Path;

        newPath = oldPath;
        newPath.SetTape(fNewTapeName);

        printf("\r\noldPath = %s, newPath = %s\r\n", oldPath.GetPath(true, true, true).c_str(), newPath.GetPath(true, true, true).c_str());

        if((result = renameFile(tapeList[i].Name.c_str(), newPath.GetPath(false, true).c_str(), Path.GetDevice())) != FR_OK)
        {
            printf("File rename error %s\r\n", GetErrorText(result));
            return false;
        }
    }
    return true;
}
