#include "TapeFileBase.hpp"
#include "Tape.hpp"
#include "File.hpp"

FRESULT errorTranslation[] = 
{ 
    FR_OK,                  // LFS_ERR_OK = 0
    FR_INT_ERR,             // LFS_ERR_IO = -1
    FR_INVALID_OBJECT,      // LFS_ERR_CORRUPT = -2
    FR_NO_PATH,             // LFS_ERR_NOENT = -3
    FR_EXIST,               // LFS_ERR_EXIST = -4
    FR_DENIED,              // LFS_ERR_NOTDIR, LFS_ERR_ISDIR = -5
    FR_DENIED,              // Skipped = -6
    FR_DENIED,              // LFS_ERR_NOTEMPTY = -7
    FR_NO_FILE,             // LFS_ERR_BADF = -8
    FR_DENIED,              // LFS_ERR_FBIG = -9
    FR_INVALID_PARAMETER,   // LFS_ERR_INVAL = -10
    FR_DENIED,              // LFS_ERR_NOSPC = -11
    FR_NOT_ENOUGH_CORE,     // LFS_ERR_NOMEM = -12
    FR_INVALID_OBJECT,      // LFS_ERR_NOATTR = -13
    FR_INVALID_NAME         // LFS_ERR_NAMETOOLONG = -14
};

const char* ErrorText[20] =
{
    "OK",
    "Device error",
    "Internal error",
    "Not ready",
    "No file",
    "No path",
    "Invalid name",
    "Denied",
    "Already exists",
    "Invalid file/tape",
    "Write protected",
    "Invalid device",
    "Not enabled",
    "No file system",
    "MKFS aborted",
    "Timeout",
    "Locked",
    "Not enough core",
    "Too many open files",
    "Invalid parameter"
};
    
FATFS       fs;

FRESULT TTapeFileBase::MountSD()
{
    return f_mount(&fs, "", 1);
}
FRESULT TTapeFileBase::MountFlash()
{
    return TranslateError(pico_mount(false));
}

FRESULT TTapeFileBase::UnmountSD()
{
    return f_unmount("");
}
FRESULT TTapeFileBase::UnmountFlash()
{
    return TranslateError(pico_unmount());
}

bool TTapeFileBase::isSD(eDevice fDevice)
{
    return (fDevice == eDevice::SD) || ((fDevice == eDevice::Default) && Configuration.UseExternalSD);
}
bool TTapeFileBase::isFlash(eDevice fDevice)
{
    return (fDevice == eDevice::Flash) || ((fDevice == eDevice::Default) && Configuration.UseInternalFlash);
}

FRESULT TTapeFileBase::renameFile(const char *fOldFileName, const char *fNewFileName, eDevice fDevice)
{
    TPath   from(CurrentTape.Path);
    TPath   to(CurrentTape.Path);

    from.SetFileSequenceNr(fOldFileName);
    to.SetFileSequenceNr(fNewFileName);

    if(isSD(fDevice))
    {
        MountSD();

        return f_rename(from.GetPath(true, true).c_str(), to.GetPath(true, true).c_str());
    }
    if(isFlash(fDevice))
    {
        MountFlash();
        
        return TranslateError(pico_rename(from.GetPath(true, true).c_str(), to.GetPath(true, true).c_str()));
    }
    return FR_INVALID_DRIVE;
}

FRESULT TTapeFileBase::TranslateError(int fPicoErrorCode)
{
    if((fPicoErrorCode > 0) || (fPicoErrorCode < -14))
        return (FRESULT)fPicoErrorCode;
    return errorTranslation[-fPicoErrorCode];
}
const char  *TTapeFileBase::GetErrorText(FRESULT fResult)
{
    if((fResult < 0) || (fResult > 19))
        return "Unknown error code";
    return ErrorText[(int)fResult];
}
size_t TTapeFileBase::GetFileSize(TPath fPath)
{
    if(fPath.GetDevice() == eDevice::SD)
    {
        FILINFO fileInfo;

        MountSD();
        if(f_stat(fPath.GetPath(true, true).c_str(), &fileInfo) != FR_OK)
            return 0;
        return (size_t)fileInfo.fsize;
    }
    else if(fPath.GetDevice() == eDevice::Flash)
    {
        lfs_info fileInfo;

        MountFlash();
        if(pico_stat(fPath.GetPath(true, true).c_str(), &fileInfo) < 0)
            return 0;
        return (size_t)fileInfo.size;
    }
    else
        return 0;
}

bool TTapeFileBase::tapeExists(const char *fTapeName, bool fDefaultIfError, eDevice fDevice)
{
    bool    result;
    TPath   path(fTapeName);

    if(isSD(fDevice))
    {
        MountSD();
        FILINFO file;

        switch(f_stat(path.GetTape().c_str(), &file))
        {
            case FR_OK:
                result = (file.fattrib & AM_DIR) != 0;
                break;
            case FR_NO_FILE:
            case FR_NO_PATH:
                result = false;
                break;
            default:
                result = fDefaultIfError;
                break;
        }
        return result;
    }
    if(isFlash(fDevice))
    {
        // printf("\r\nTesting is tape exists in flash\r\n");
        MountFlash();

        struct lfs_info info;
        int dir = pico_dir_open("/");
        // printf("Directory open %d\r\n", dir);

        if(dir < 0)
        {
            // printf("\r\nCannot open directory %s\r\n", pico_errmsg(dir));
            result = fDefaultIfError;
        }
        else
        {           
            while (pico_dir_read(dir, &info) > 0) 
            {
                // printf("File entry read %d\r\n", info.name);
                
                if((info.type != LFS_TYPE_DIR) || (info.name[0] == '.'))
                    continue;

                if(strcmp(path.GetTape().c_str(), info.name) == 0)
                {
                    result = true;
                    break;
                }
            }

            int pico_result = pico_dir_close(dir);
            // printf("Directory closed with result %d\r\n", pico_result);

            if(pico_result < 0)
                result = fDefaultIfError;
            return false;
        }
        return result;
    }
    return fDefaultIfError;
}

FRESULT TTapeFileBase::enumerateSD(std::vector<TInfo> *fTapeFileList, bool fTape)
{
    char        root[FULL_FILE_PATH_LENGTH];

    if(fTape)
        strcpy(root, "/");
    else
        strcpy(root, CurrentTape.Path.GetTape().c_str());

    return enumerateSD(fTapeFileList, root, fTape);
}
FRESULT TTapeFileBase::enumerateSD(std::vector<TInfo> *fTapeFileList, const char *fRoot, bool fTape)
{
    DIR         dir;
    FILINFO     file;
    FRESULT     result;
    TInfo       fileInfo;
    
    fTapeFileList->clear();

    if((result = MountSD()) != FR_OK)
        return result;

    if((result = f_opendir(&dir, fRoot)) != FR_OK)
        return result;

    for(;;)
    {
        if((result = f_readdir(&dir, &file)) != FR_OK)
        {
            f_closedir(&dir);
            return result;
        }
        if(file.fname[0] == '\0')
            break;

        if((((file.fattrib & AM_DIR) != 0) == fTape) && ((file.fattrib & AM_HID) == 0))
        {
            if(file.fname[0] != '.')
            {
                fileInfo.Name = file.fname;
                fileInfo.Size = file.fsize;

                fTapeFileList->push_back(fileInfo);
            }
        }
    }
    
    result = f_closedir(&dir);
    return result;
}
FRESULT TTapeFileBase::enumerateFlash(std::vector<TInfo> *fTapeFileList, bool fTape)
{
    struct lfs_info info;
    char            root[FULL_FILE_PATH_LENGTH];
    FRESULT         result;

    if(fTape)
        strcpy(root, "/");
    else
        strcpy(root, CurrentTape.Path.GetTape().c_str());

    return enumerateFlash(fTapeFileList, root, fTape);
}
FRESULT TTapeFileBase::enumerateFlash(std::vector<TInfo> *fTapeFileList, const char *fTapeName, bool fTape)
{
    struct lfs_info info;
    FRESULT         result;
    TInfo           fileInfo;

    fTapeFileList->clear();
    if((result = MountFlash()) != FR_OK)
    {
        printf("\r\nCannot mount flash for enumeration, error %s\r\n", GetErrorText(result));
        return result;
    }

    int dir = pico_dir_open(fTapeName);

    if(dir < 0)
        return TranslateError(dir);

    while(pico_dir_read(dir, &info) > 0)
    {
        if(info.name[0] == '.')
            continue;

        if((info.type == LFS_TYPE_DIR) == fTape)
        {
            fileInfo.Name = info.name;
            fileInfo.Size = info.size;
            fTapeFileList->push_back(fileInfo);
        }
    }
    return TranslateError(pico_dir_close(dir));
}
