#include "CommandLine_FileAndTape.hpp"
#include "Path.hpp"

// Create two static instances. At most, only two files will ever be open at once

TFile File1;
TFile File2;

void fileLIST(const char *fTape = NULL);

const char* BASICKeywords[] =
{
    "END",      // 80
    "FOR",      // 81
    "NEXT",     // 82
    "DATA",     // 83
    "INPUT",    // 84
    "DIM",      // 85
    "READ",     // 86
    "LET",      // 87
    "GOTO",     // 88
    "RUN",      // 89
    "IF",       // 8A
    "RESTORE",  // 8B
    "GOSUB",    // 8C
    "RETURN",   // 8D
    "REM",      // 8E
    "STOP",     // 8F
    "ON",       // 90
    "NULL",     // 91
    "WAIT",     // 92
    "LOAD",     // 93
    "SAVE",     // 94
    "DEF",      // 95
    "POKE",     // 96
    "PRINT",    // 97
    "CONT",     // 98
    "LIST",     // 99
    "CLEAR",    // 9A
    "GET",      // 9B
    "NEW",      // 9C

    "TAB(",     // 9D
    "TO",       // 9E
    "FN",       // 9F
    "SPC(",     // A0
    "THEN",     // A1
    "NOT",      // A2
    "STEP",     // A3
    "+",        // A4
    "-",        // A5
    "*",        // A6
    "/",        // A7
    "^",        // A8
    "AND",      // A9
    "OR",       // AA
    ">",        // AB
    "=",        // AC
    "<",        // AD
    "SGN",      // AE
    "INT",      // AF
    "ABS",      // B0
    "USR",      // B1
    "FRE",      // B2
    "POS",      // B3
    "SQR",      // B4
    "RND",      // B5
    "LOG",      // B6
    "EXP",      // B7
    "COS",      // B8
    "SIN",      // B9
    "TAN",      // BA
    "ATN",      // BB
    "PEEK",     // BC
    "LEN",      // BD
    "STR$",     // BE
    "VAL",      // BF
    "ASC",      // C0
    "CHR$",     // C1
    "LEFT$",    // C2
    "RIGHT$",   // C3
    "MID$",     // C4
    "GO"        // C5
};

bool reportCassetteOverride()
{
    if(!Configuration.UseExternalSD && !Configuration.UseInternalFlash)
    {
        printf("\r\nNeither SD nor Flash in use\r\n");
        return false;
    }
    return true;
}
bool getFileOrSequenceNumber(const char *fText, int *fNumber)
{
    char *numberStart = (char *)fText;
    bool isSequenceNumber = false;
    char *p;

    *fNumber = -1;

    if(strlen(fText) == 0)
    {
        printf("\r\nMissing file or sequence number parameter\r\n");
        return false;
    }
    if((fText[0] == 'S') || (fText[0] == 's'))
    {
        isSequenceNumber = true;
        numberStart++;
        if(strlen(numberStart) == 0)
        {
            printf("\r\nMissing sequence number parameter\r\n");
            return false;
        }
    }

    uint16_t number;

    if(isSequenceNumber)
        number = strtoul(numberStart, &p, 10);
    else
        number = strtoul(numberStart, &p, 16);

    if(*p != 0)
    {
        printf("\r\nError in file or sequence number parameter %s\r\n", fText);
        return false;
    }
    *fNumber = number;
    return isSequenceNumber;
} 
void copyFile(TPath *fFrom, TPath *fTo)
{
    char    buffer[1024];
    UINT    bytesRead;
    UINT    bytesWritten;
    FRESULT result;

    if(fTo->GetSequenceNr() == 0)
        fTo->SetSequenceNr(CurrentTape.GetNextSequenceNr(fTo->GetTape().c_str()));

    result = CurrentTape.OpenPath(&File1, fFrom, false);
    if(!reportError(result, 0, 0))
        return;
    result = CurrentTape.OpenPath(&File2, fTo, true);
    if(!reportError(result, 0, 0))
    {
        File1.Close();
        return;
    }

    do
    {
        result = File1.Read(buffer, 1024, &bytesRead);

        if(!reportError(result, 0, 0))
        {
            File1.Close();
            File2.Close();
            return;
        }
        if(bytesRead == 0)
            break;
        result = File2.Write(buffer, bytesRead, &bytesWritten);
        if(!reportError(result, bytesRead, bytesWritten))
        {
            File1.Close();
            File2.Close();
            return;
        }
    } while (bytesRead == 1024);
    result = File1.Close();
    if(!reportError(result, 0, 0))
    {
        File2.Close();
        return;
    }
    result = File2.Close();
    if(!reportError(result, 0, 0))
        return;
}
void copyFile(const char *fFileName, const char *fToTapeName)
{
    FRESULT result;
    TPath   pathFrom(CurrentTape.Path);
    TPath   pathTo;


    pathFrom.SetFileSequenceNr(fFileName);

    pathTo = TPath(pathFrom);
    pathTo.SetTape(fToTapeName);

    copyFile(&pathFrom, &pathTo);
}

FRESULT flashFORMAT(bool fSkipAreYouSure = false)
{
    FRESULT result;

    if(!fSkipAreYouSure)
    {
        if(!AreYouSure(false))
        {
            printf("\r\nFormat abandoned\r\n");
            return (FRESULT)-15;    // Own code for "Abandoned"
        }
    }
    if((result = CurrentTape.TranslateError(pico_mount(true))) == FR_OK)
        printf("\r\nFormat successful\r\n");
    else
        printf("\r\nFormat failed\r\n");
    return result;
}
FRESULT sdFORMAT(bool fSkipAreYouSure = false)
{
    if(!fSkipAreYouSure)
    {
        if(!AreYouSure(false))
        {
            printf("\r\nFormat abandoned\r\n");
            return (FRESULT)-15;    // Own code for "Abandoned"
        }
    }
    printf("\r\nBe patient. This takes a little time...");

    BYTE        work[FF_MAX_SS];
    MKFS_PARM   opt;
    FRESULT     result;

    opt.fmt = FM_FAT32 | FM_SFD;
    opt.n_fat = 2;
    opt.align = 0;
    opt.n_root = 512;
    opt.au_size = 0;
 
    TTape::MountSD();
    if((result = f_mkfs("", &opt, work, FF_MAX_SS)) == FR_OK)
        printf("\r\nFormat successful\r\n");
    else
        printf("\r\nFormat failed: %s\r\n", CurrentTape.GetErrorText(result));
    return result;
}

void tapeRENUMBER()
{
    const char *startSequenceNrText = strtok(NULL, ", ");
    const char *stepText = strtok(NULL, "");
          char *p;
          int   startSequenceNr;
          int   step;

    if(startSequenceNrText == NULL)
    {
        printf("\r\nStart and/or step number must be specified\r\n");
        return;
    }
    startSequenceNr = strtol(startSequenceNrText, &p, 10);
    if(*p != 0)
    {
        printf("\r\nStart sequence number invalid\r\n");
        return;        
    }

    if(stepText == NULL)
        step = startSequenceNr;
    else
    {
        step = strtol(stepText, &p, 10);
        if(*p != 0)
        {
            printf("\r\nStep value invalid\r\n");
            return;        
        }
    }
    
    if(step <= 0)
    {
        printf("\r\nStep must be 1 or greater\r\n");
        return;
    }
    if(CurrentTape.Renumber(startSequenceNr, step))
        CurrentTape.CurrentSequenceNr = 1;
    else
        printf("\r\nCannot renumber from %d in steps of %d\r\n", startSequenceNr, step);
}
void tapeLIST()
{
    bool all = false;
    const char *paramText = strtok(NULL, ", ");

    if((paramText != NULL) && (strcmp(paramText, "ALL") == 0))
        all = true;

    std::vector<TInfo> tapeList;

    printf("\r\nTapes for %s\r\n", CurrentTape.Path.GetDeviceName().c_str());
    CurrentTape.ListTapes(&tapeList);

    if(all)
        fileLIST("/");
    else
        printf("  /\r\n");
    for(int i = 0; i < tapeList.size(); i++)
    {
        if(all)
            fileLIST(tapeList[i].Name.c_str());
        else
            printf("  %s\r\n", tapeList[i].Name.c_str());
    }
    CRLF();
    return;
}
void tapeCREATE(const char* fTapeNameText)
{
    if(!CurrentTape.Create(fTapeNameText))
        printf("\r\nCannot create %s\r\n", fTapeNameText);
}
void tapeDELETE(const char* fTapeNameText)
{
    if(strcmp(fTapeNameText, "/") == 0)
    {
        printf("\r\nCannot delete root tape");
        return;
    }
    if(CurrentTape.ContainsFiles(fTapeNameText))
    {
        printf("\r\nTape contains files. Delete files and tape?\r\n");
        if(!AreYouSure(false))
        {
            printf("\r\nDelete tape abandoned\r\n");
            return;
        }
    }
    if(strcmp(CurrentTape.Path.GetPath(false).c_str(), fTapeNameText) == 0)
        CurrentTape.Path.SetTape("");

    if(!CurrentTape.DeleteTape(fTapeNameText, true))
        printf("\r\nCannot delete %s\r\n", fTapeNameText);
}
void tapeOPEN(const char* fTapeNameText)
{
    if(CurrentTape.Load(fTapeNameText) == FR_OK)
        printf("\r\nTape %s loaded.\r\n", CurrentTape.Path.GetTapePath(true).c_str());
    else
        printf("\r\nCan't load tape %s\r\n", fTapeNameText);
}
void tapeNEXT()
{
    const char *paramText = strtok(NULL, ", ");
          char *p;

    if((paramText == NULL) || (strlen(paramText) == 0))
    {
        printf("\r\nMust have sequence number as a parameter\r\n");
        return;
    }
       
    uint16_t value = strtoul(paramText, & p, 10);

    if(*p != 0)
    {
        printf("\r\nMust have sequence number as a parameter\r\n");
        return;
    }
    CurrentTape.CurrentSequenceNr = value;
}

void fileLIST(const char *fTape)
{
    std::vector<TInfo>  fileList;
    TPath               path(CurrentTape.Path);
    char                seqMarker[2];
    bool                markerDisplayed = false;
    char               *requestedTape = (char *)fTape;

    if(requestedTape == NULL)
        requestedTape = (char *)CurrentTape.Path.GetTapePath(false, false).c_str();

    CurrentTape.ListFiles(&fileList, requestedTape);
    if(requestedTape[0] == '/')
        requestedTape++;

    strcpy(seqMarker, " ");

    printf("\r\nFiles for tape /%s\r\n", requestedTape);

    if(strcmp(CurrentTape.Path.GetTapePath(false, false).c_str(), requestedTape) != 0)
        markerDisplayed = true;
 
    for(int i = 0; i < fileList.size(); i++)
    {
        seqMarker[0] = ' ';
        path.SetFileSequenceNr(fileList[i].Name.c_str());
        if(!markerDisplayed && (path.GetSequenceNr() >= CurrentTape.CurrentSequenceNr))
        {
            markerDisplayed = true;
            seqMarker[0] = '>';
        }
        printf("%s%04d  %-3s  %5d\r\n",seqMarker, path.GetSequenceNr(), path.GetFile().c_str(), fileList[i].Size);
    }
    return;
}
void fileDELETE()
{
    const char *fileText = strtok(NULL, ", ");

    if((fileText == NULL) || (strlen(fileText) == 0))
    {
        printf("\r\nMissing filename\r\n");
        return;
    }
    if((strcmp(fileText, "*") == 0) || (strcmp(fileText, "ALL") == 0))
    {
        if(!AreYouSure(false))
        {
            printf("\r\nDelete all abandoned\r\n");
            return;
        }
        
        if(CurrentTape.DeleteAllFiles())
            printf("\r\nAll files from %s deleted\r\n", CurrentTape.Path.GetPath(true).c_str());
        else
            printf("\r\nFiles from %s could not be deleted\r\n", CurrentTape.Path.GetPath(true).c_str());
    }
    else
    {
        int         fileOrSequenceNr;
        bool        isSequenceNumber;

        isSequenceNumber = getFileOrSequenceNumber(fileText, &fileOrSequenceNr);

        if(fileOrSequenceNr < 0)
            return;

        if(isSequenceNumber)
        {
            if(CurrentTape.DeleteFileBySequenceNr(fileOrSequenceNr, CurrentTape.Path.GetTapePath(false).c_str()))
                printf("\r\nFile with sequence number %d from %s deleted\r\n", fileOrSequenceNr, CurrentTape.Path.GetTapePath(true).c_str());
            else
                printf("\r\nFile with sequence number %d from %s could not be deleted\r\n", fileOrSequenceNr, CurrentTape.Path.GetTapePath(true).c_str());
        }
        else
        {
            if(CurrentTape.DeleteFileByName(fileOrSequenceNr, CurrentTape.Path.GetTapePath(false).c_str()))
                printf("\r\nFile %02X from %s deleted\r\n", fileOrSequenceNr, CurrentTape.Path.GetTapePath(true).c_str());
            else
                printf("\r\nFile %02X from %s could not be deleted\r\n", fileOrSequenceNr, CurrentTape.Path.GetTapePath(true).c_str());
        }
    }
}
void fileRENAME()
{
    const char *fromFileText = strtok(NULL, ", ");
    const char *toFileText = strtok(NULL, ", ");
    int         fileOrSequenceNr;
    bool        isSequenceNumber;
    char        toFileName[3];

    if(!TPath::IsValidFileName(toFileText))
    {
        printf("\r\n%s is not a valid file name\r\n", toFileText);
        return;
    }

    sprintf(toFileName, "%02X", strtol(toFileText, NULL, 16) & 0xFF);

    isSequenceNumber = getFileOrSequenceNumber(fromFileText, &fileOrSequenceNr);

    if(fileOrSequenceNr < 0)
        return;

    if(isSequenceNumber)
    {
        if(!CurrentTape.RenameFileBySequenceNr(fileOrSequenceNr, toFileName))
            printf("\r\nCannot rename file with sequence number %d\r\n", fileOrSequenceNr);
    }
    else
    {
        char fromFileName[3];

        sprintf(fromFileName, "%02X", fileOrSequenceNr & 0xFF);

        if(!CurrentTape.RenameFile(fromFileName, toFileName))
            printf("\r\nCannot rename %s to %s\r\n", fromFileText, toFileText);
    }
}
void filePOSITION()
{
    const char                 *fromSequenceNrText = strtok(NULL, ", ");
    const char                 *toSequenceNrText = strtok(NULL, "");
          char                 *p;
          int                   fromSequenceNr;
          int                   toSequenceNr;
          std::vector<TInfo>    fileList;
          bool                  oldSeqNrFound;
          std::string           filename;
          TPath                 path = CurrentTape.Path;

    if(fromSequenceNrText == NULL)
    {
        printf("\r\nFrom sequence number number must be specified\r\n");
        return;
    }
    fromSequenceNr = strtol(fromSequenceNrText, &p, 10);
    if(*p != 0)
    {
        printf("\r\nFrom sequence number invalid\r\n");
        return;        
    }
    if(toSequenceNrText == NULL)
    {
        printf("\r\nTo sequence number number must be specified\r\n");
        return;
    }
    toSequenceNr = strtol(toSequenceNrText, &p, 10);
    if(*p != 0)
    {
        printf("\r\nTo sequence number invalid\r\n");
        return;        
    }

    if(!CurrentTape.ListFiles(&fileList))
        return;

    oldSeqNrFound = false;
    for(int i = 0; i < fileList.size(); i++)
    {
        path.SetFileSequenceNr(fileList[i].Name.c_str());

        if(!oldSeqNrFound && (path.GetSequenceNr() == fromSequenceNr))
        {
            oldSeqNrFound = true;
            filename = fileList[i].Name;
        }
        if(path.GetSequenceNr() == toSequenceNr)
        {
            printf("\r\nSequence Nr %d already exists\r\n", toSequenceNr);
            return;
        }
    }
    if(!oldSeqNrFound)
    {
        printf("\r\nFile with sequence Nr %d not found\r\n", fromSequenceNr);
        return;
    }
    path.SetFileSequenceNr(filename.c_str());
    path.SetSequenceNr(toSequenceNr);

    if(!CurrentTape.renameFile(filename.c_str(), path.GetPath(false, true).c_str(), CurrentTape.Path.GetDevice()) == FR_OK)
        printf("\r\nCannot reposition %d as %d\r\n", fromSequenceNr, toSequenceNr);
}
int fileShowBASICLine(TFile *fFile, uint16_t fBaseAddress, uint8_t *fBuffer)
{
    UINT        bytesRead;
    FRESULT     result;

    result = fFile->Read((char *)fBuffer, 2, &bytesRead);
    if(!reportError(result, bytesRead, 2))
        return -1;
           
    uint16_t startAddress = (fBuffer[0] | (fBuffer[1] << 8));

    if(startAddress == 0)
        return 0;

    result = fFile->Read((char *)fBuffer, startAddress - fBaseAddress - 2, &bytesRead);
    if(!reportError(result, bytesRead, startAddress - fBaseAddress - 2))
        return -1;
    
    int lineNumber = fBuffer[0] | (fBuffer[1] << 8);

    printf("\r\n%5d ", lineNumber);

    for(int i = 2; i < bytesRead; i++)
    {
        if(fBuffer[i] >= 0x80)
            printf("%s", BASICKeywords[fBuffer[i] - 0x80]);
        else if(fBuffer[i] == 0x00)
            break;
        else
            stdio_putchar(fBuffer[i]);
    }

    return startAddress;
}
int fileShowRAELine(TFile *fFile)
{
    const uint8_t column[] = { 10, 20, 30 };

    uint8_t columnNr = 0;
    uint8_t cursor = 0;
    uint8_t buffer[2];
    UINT    bytesRead;
    FRESULT result;
    bool    isComment = false;

    // Read Line Number
    result = fFile->Read((char *)buffer, 2, &bytesRead);
    if(bytesRead == 0)
        return 0;
    if(!reportError(result, bytesRead, 2))
        return -1;

    if(bytesRead != 2)
        return 0;
        
    int lineNumber = (buffer[1] >> 4) * 1000 + (buffer[1] & 0xF) * 100 + (buffer[0] >> 4) * 10 + (buffer[0] & 0xF);

    printf("\r\n%5d ", lineNumber);
    while(true)
    {
        result = fFile->Read((char *)buffer, 1, &bytesRead);
        if(!reportError(result, bytesRead, 1))
            return -1;

        if(buffer[0] == ';')
            isComment = true;
        if((buffer[0] == ' ') && !isComment)
        {
            if(columnNr < sizeof(column))
            {
                while(cursor++ < column[columnNr])
                    stdio_putchar(' ');
                columnNr++;
                do
                {
                    result = fFile->Read((char *)buffer, 1, &bytesRead);
                    if(!reportError(result, bytesRead, 1))
                        return -1;
                    if(buffer[0] == ';')
                        isComment = true;
                } while (buffer[0] == ' ');
                stdio_putchar(buffer[0]);
            }
            else
            {
                stdio_putchar(' ');
                cursor++;
            }
        }
        else
        {
            stdio_putchar(buffer[0] & 0x7F);
            cursor++;
        }
        if((buffer[0] & 0x80) != 0)
            break;
    }
    return 1;
}
void fileType(TPath *fPath, bool fBasic, bool fRAE)
{
    uint8_t     buffer[1024];
    UINT        bytesRead = 0;
    int         baseAddress = 0;
    uint16_t    startAddress = 0x201;
    uint16_t    endAddress = 0;
    FRESULT     result;
    int         fileSize = TTape::GetFileSize(fPath);

    if((result = CurrentTape.OpenPath(&File1, fPath, false)) != FR_OK)
    {
        printf("Cannot open %s : %s\r\n", fPath->GetPath(true, true, true).c_str(), TTapeFileBase::GetErrorText(result));
        return;   
    }

    result = File1.Read((char *)buffer, 4, &bytesRead);
    
    if(bytesRead == 0)
    {
        printf("\r\nEmpty file\r\n");
        return;
    }
    if(!reportError(result, bytesRead, 4))
        return;

    startAddress = buffer[0] | (buffer[1] << 8);
    endAddress = buffer[2] | (buffer[3] << 8);
    printf("\r\nStart Address %04X, End Address %04X", startAddress, endAddress);

    if(fBasic)
    {
        baseAddress = startAddress;
        CRLF();
        while(true)
        {
            baseAddress = fileShowBASICLine(&File1, baseAddress, buffer);
            if(baseAddress <= 0)
            {
                File1.Close();
                return;
            }
            int c = stdio_getchar_timeout_us(0);

            if((c == 0x1B) || (c == 0x03))
                break;
        }
    }
    else if(fRAE)
    {
        CRLF();
        if(fileSize <= 9)
        {   // Initial file
            result = File1.Read((char *)buffer, 5, &bytesRead);
            if(!reportError(result, bytesRead, 5))
                return;
            printf("File ID = %02X, Load Start Address = %04X, Load End Address = %04X\r\n", buffer[0], *((uint16_t *)(&buffer[1])), *((uint16_t *)(&buffer[3])));
        }
        else
        {
            while(fileShowRAELine(&File1) > 0)
            {
                int c = stdio_getchar_timeout_us(0);

                if((c == 0x1B) || (c == 0x03))
                    break;
            }
        }
    }
    else
    {
        int bfrPtr = -1;
        bool lastBlock = false;
        char text[17];

        printf("\r\n       0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F   0123456789ABCDEF");
        for(int masterAddress = startAddress & 0xFFF0; masterAddress <= endAddress; )
        {
            printf("\r\n%04X: ", masterAddress);
            memset(text, 0, sizeof(text));

            for(int offsetAddress = 0; offsetAddress < 16; offsetAddress++)
            {
                if(bfrPtr < 0)
                {
                    if(!lastBlock)
                    {
                        result = File1.Read((char *)buffer, 1024, &bytesRead);
                        if(!reportError(result, 0, 0))
                        {
                            File1.Close();
                            return;
                        }
                        bfrPtr = 0;
                        if(bytesRead < 1024)
                            lastBlock = true;
                    }
                }
                if((masterAddress + offsetAddress < startAddress) || (masterAddress + offsetAddress >= endAddress))
                {
                    printf("   ");
                    text[offsetAddress] = ' ';
                    continue;
                }
                else
                {
                    printf("%02X ", buffer[bfrPtr]);
                    text[offsetAddress] = (buffer[bfrPtr] < 0x20) || (buffer[bfrPtr] > 0x7E) ? '.' : buffer[bfrPtr];
                    if(++bfrPtr >= bytesRead)
                        bfrPtr = -1;
                }
            }
            printf("  %s", text);
            masterAddress += 16;
            if(masterAddress > endAddress)
                break;
        }
    }
    CRLF();
    File1.Close();
}
void fileTYPE()
{
    const char *fileText = strtok(NULL, ", ");

    if((fileText == NULL) || (strlen(fileText) == 0))
    {
        printf("\r\nMissing filename\r\n");
        return;
    }
    const   char       *textMode = strtok(NULL, ", ");
            bool        basic = false;
            bool        rae = false;
            FRESULT     result;

    if((textMode != NULL) && (strlen(textMode) != 0))
    {
        if((strcmp(textMode, "B") == 0) || (strcmp(textMode, "BASIC") == 0))
            basic = true;
        else if((strcmp(textMode, "R") == 0) || (strcmp(textMode, "RAE") == 0))
            rae = true;
        else
        {
            printf("\r\nUnknown format\r\n");
            return;
        }
    }

    std::vector<TInfo> fileList = CurrentTape.FindFiles(fileText, CurrentTape.Path.GetTape().c_str());
    TPath path(&(CurrentTape.Path));

    CRLF();
    for(int i = 0; i < fileList.size(); i++)
    {
        path.SetFileSequenceNr(fileList[i].Name.c_str());
        fileType(&path, basic, rae);
    }
}
void fileCOPY()
{
    const char         *fileSequenceNrText = strtok(NULL, ", ");
    const char         *toTapeName = strtok(NULL, ", ");
    std::vector<TInfo>  fileList;
    TPath               toPath;
    char                filename[5];
    int                 fileOrSequenceNr;
    bool                isSequenceNumber;

    if((fileSequenceNrText == NULL) || (strlen(fileSequenceNrText) == 0))
    {
        printf("\r\nMissing file and tapename\r\n");
        return;
    }
    if((toTapeName == NULL) || (strlen(toTapeName) == 0))
    {
        printf("\r\nMissing tape name\r\n");
        return;
    }

    isSequenceNumber = getFileOrSequenceNumber(fileSequenceNrText, &fileOrSequenceNr);

    if(fileOrSequenceNr < 0)
        return;

    if(!CurrentTape.ListFiles(&fileList, CurrentTape.Path.GetTape().c_str()))
    {
        printf("\r\nNo files found\r\n");
        return;
    }

    if(isSequenceNumber)
        sprintf(filename, "%04d", fileOrSequenceNr);
    else
        sprintf(filename, "%02X", fileOrSequenceNr & 0xFF);

    for(int i = 0; i < fileList.size(); i++)
    {
        TPath path(&(CurrentTape.Path));

        path.SetFileSequenceNr(fileList[i].Name.c_str());

        if((isSequenceNumber  && (path.GetSequenceNr() == fileOrSequenceNr)) ||
           (!isSequenceNumber && (path.GetFileInt()    == (fileOrSequenceNr))))
        {
            TPath toPath(path);

            toPath.SetTape(toTapeName);
            toPath.SetSequenceNr(0);    // Use next sequence number

            copyFile(&path, &toPath);
        }
    }
    CurrentTape.CurrentSequenceNr = 0;
}

void copyFile(const char * fTapeName, const char *fFileName, eDevice fFrom, eDevice fTo)
{
    char    buffer[1024];
    UINT    bytesRead;
    UINT    bytesWritten;
    FRESULT result;
    TPath   pathFrom;
    TPath   pathTo;

    pathFrom.SetDevice(fFrom);
    pathFrom.SetTape(fTapeName);
    pathFrom.SetFileSequenceNr(fFileName);

    pathTo = TPath(pathFrom);
    pathTo.SetDevice(fTo);

    copyFile(&pathFrom, &pathTo);
}
void copyTape(const char * fTapeName, eDevice fFrom, eDevice fTo)
{
    std::vector<TInfo> fileList;

    CurrentTape.ListFiles(&fileList, fTapeName, fFrom);

    for(int fileIndex = 0; fileIndex < fileList.size(); fileIndex++)
        copyFile(fTapeName, fileList[fileIndex].Name.c_str(), fFrom, fTo);
}
void copy(eDevice fFrom, eDevice fTo)
{
    std::vector<TInfo>  tapeList;
    TPath               path;

    CurrentTape.LoadRoot();
    // Copy root files
    copyTape("", fFrom, fTo);
        
    CurrentTape.ListTapes(&tapeList, fFrom);
    
    for(int tapeIndex = 0; tapeIndex < tapeList.size(); tapeIndex++)
    {
        std::string tapeName = tapeList[tapeIndex].Name;

        path.SetDevice(fTo);

        if(!CurrentTape.Create(tapeName.c_str(), fTo))
        {
            printf("\r\nCannot create tape %s\r\n", tapeName.c_str());
            return;
        }
        path.SetTape(tapeName.c_str());
        copyTape(tapeName.c_str(), fFrom, fTo);
    }
    CurrentTape.LoadRoot();
    CurrentTape.CurrentSequenceNr = 0;
    printf("\r\nCopy complete\r\n");
}
void copyTo()
{
    const char  *device = strtok(NULL, ", ");

    if(strcmp(device, "SD") == 0)
    {
        printf("\r\nCopy from Flash to SD\r\nAll tapes and files will be deleted\r\n");
        if(!AreYouSure(false))
            return;
        if(sdFORMAT(true) == FR_OK)
            copy(eDevice::Flash, eDevice::SD);
    }
    else if(strcmp(device, "FLASH") == 0)
    {
        printf("\r\nCopy from SD to Flash\r\nAll tapes and files will be deleted\r\n");
        if(!AreYouSure(false))
            return;
        if(flashFORMAT(true) == FR_OK)
            copy(eDevice::SD, eDevice::Flash);
    }
    else
        printf("\r\nDevice must be SD or FLASH\r\n");
}

void flashCommand()
{
    const char *paramText = strtok(NULL, ", ");

    if((paramText == NULL) || (strlen(paramText) == 0))
    {
        printf("\r\nMissing parameter in FLASH command\r\n");
        return;
    }
    if(strcmp(paramText, "FORMAT") == 0)
    {
        flashFORMAT();
        return;
    }
    if(strcmp(paramText, "ON") == 0)
    {
        Configuration.UseInternalFlash = true;
        Configuration.UseExternalSD = false;
        CurrentTape.Path.SetDevice(eDevice::Flash);
        CurrentTape.LoadRoot();
        return;
    }
    if(strcmp(paramText, "OFF") == 0)
    {
        Configuration.UseInternalFlash = false;
        if(Configuration.UseExternalSD)
            CurrentTape.Path.SetDevice(eDevice::SD);
        else
            CurrentTape.Path.SetDevice(eDevice::Default);
        return;
    }
    printf("\r\nUnknown FLASH command\r\n");
}
void sdCommand()
{
    const char *paramText = strtok(NULL, "");

    if((paramText == NULL) || (strlen(paramText) == 0))
    {
        printf("\r\nMissing parameter in SD command\r\n");
        return;
    }
    if(strcmp(paramText, "FORMAT") == 0)
    {
        sdFORMAT();
        return;
    }
    if(strcmp(paramText, "ON") == 0)
    {
        Configuration.UseExternalSD = true;
        Configuration.UseInternalFlash = false;
        CurrentTape.Path.SetDevice(eDevice::SD);
        CurrentTape.LoadRoot();
        return;
    }
    if(strcmp(paramText, "OFF") == 0)
    {
        Configuration.UseExternalSD = false;
        if(Configuration.UseInternalFlash)
            CurrentTape.Path.SetDevice(eDevice::Flash);
        else
            CurrentTape.Path.SetDevice(eDevice::Default);
        return;
    }
    printf("\r\nUnknown SD command\r\n");
}
void tapeCommand()
{
    if(!reportCassetteOverride())
        return;

        const char *cmdText = strtok(NULL, ", ");

    if((cmdText == NULL) || (strlen(cmdText) == 0))
    {
        printf("\r\nTAPE command required\r\n");
        return;
    }

    if(strcmp(cmdText, "?") == 0)
    {
        printf("\r\nCurrent tape %s\r\n", CurrentTape.Path.GetTapePath(true).c_str());
        return;
    }
    if(strcmp(cmdText, "LIST") == 0)
    {
        tapeLIST();
        return;
    }
    if((strcmp(cmdText, "RENUMBER") == 0) || (strcmp(cmdText, "RENUM") == 0))
    {
        tapeRENUMBER();
        return;
    }
    if(strcmp(cmdText, "REWIND") == 0)
    {
        CurrentTape.CurrentSequenceNr = 1;
        return;
    }
    if(strcmp(cmdText, "NEXT") == 0)
    {
        tapeNEXT();
        return;
    }

    const char *tapeNameText = strtok(NULL, "");

    if((tapeNameText == NULL) || (strlen(tapeNameText) == 0))
    {
        printf("\r\nTAPE name required\r\n");
        return;
    }
    if(strlen(tapeNameText) > 255)
    {
        printf("\r\nMaximum TAPE name length is 255 characters\r\n");
        return;
    }

    if(strcmp(cmdText, "CREATE") == 0)
        tapeCREATE(tapeNameText);
    else if(strcmp(cmdText, "DELETE") == 0)
        tapeDELETE(tapeNameText);
    else if(strcmp(cmdText, "LOAD") == 0)
        tapeOPEN(tapeNameText);

    else
    {
        printf("\r\nTAPE LIST, CREATE, DELETE, or OPEN required\r\n");
        return;      
    }
}
void fileCommand()
{
    if(!reportCassetteOverride())
        return;

    const char *paramText = strtok(NULL, ", ");

    if((paramText == NULL) || (strlen(paramText) == 0))
    {
        printf("\r\nMissing parameter\r\n");
        return;
    }
    if(strcmp(paramText, "LIST") == 0)
    {
        fileLIST();
        return;
    }
    if(strcmp(paramText, "DELETE") == 0)
    {
        fileDELETE();
        return;
    }
    if(strcmp(paramText, "TYPE") == 0)
    {
        fileTYPE();
        return;
    }
    if(strcmp(paramText, "RENAME") == 0)
    {
        fileRENAME();
        return;
    }
    if((strcmp(paramText, "POSITION") == 0) || (strcmp(paramText, "POS") == 0))
    {
        filePOSITION();
        return;
    }
    if(strcmp(paramText, "COPY") == 0)
    {
        fileCOPY();
        return;
    }

    printf("\r\nUnknown FILE instruction\r\n");
}