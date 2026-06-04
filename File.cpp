#include "File.hpp"
#include "Tape.hpp"

// void dumpFIL(FIL *f, const char *prefix = "")
// {
//     printf("%sFIL: buf = %p, cltbl = %p, fptr = %ld, sect = %ld, clust = %ld, clust = %04X, dir_ptr = %ld, dir_sect = %ld, flag = %02X, err = %02X\r\n", 
//             prefix, (void *)f->buf, (void *)f->cltbl, f->fptr, f->sect, f->clust, f->dir_ptr, f->dir_sect, 
//             f->flag, f->err);
// }

FRESULT TFile::Close()
{
    Tape->CurrentSequenceNr = sequenceNr + 1;
    
    if(device == eDevice::SD)
        return f_close(&fatfsFile);
    if(device == eDevice::Flash)
    {
        if(flashFileHandle >= 0)
            return TranslateError(pico_close(flashFileHandle));
    }
    return FR_OK;
}
FRESULT TFile::Read(char *fBuffer, int fBufferSize, UINT *fNrBytesRead)
{
    if(device == eDevice::SD)
        return f_read(&fatfsFile, fBuffer, fBufferSize, fNrBytesRead);
    if(device == eDevice::Flash)
    {
        if(flashFileHandle >= 0)
        {
            *fNrBytesRead = pico_read(flashFileHandle, fBuffer, fBufferSize);
            if(*fNrBytesRead < 0)
            {
                pico_close(flashDirectory);
                return TranslateError(*fNrBytesRead);
            }
        }
    }
    return FR_OK;
}
FRESULT TFile::Write(char *fBuffer, int fBufferSize, UINT *fNrBytesWrite)
{
    if(device == eDevice::SD)
    {
        return f_write(&fatfsFile, fBuffer, fBufferSize, fNrBytesWrite);
    }
    if(device == eDevice::Flash)
    {
        if(flashFileHandle >= 0)
        {
            *fNrBytesWrite = pico_write(flashFileHandle, fBuffer, fBufferSize);
            if(*fNrBytesWrite < 0)
            {
                pico_close(flashDirectory);
                return TranslateError(*fNrBytesWrite);
            }
        }
    }
    return FR_OK;
}
// TODO: Move to somewhere more appropriate
bool AreYouSure(bool fDefault)
{
    char y = fDefault ? 'Y' : 'y';
    char n = fDefault ? 'n' : 'N';
    while(true)
    {
        printf("\r\nAre you sure [%c/%c]? ", n, y);
        int c = getchar();
        if((c == '\r') || (c == '\n'))
            return fDefault;
        if((c == 'n') || (c == 'N'))
            return false;
        if((c == 'y') || (c == 'Y'))
            return true;
    }
}
