#include "ReadWrite.h"

bool USBRedirect = false;
struct lfs_info info;
uint8_t RAEFileNo;
int Directory = -1;

eMemoryAddressResult __not_in_flash_func(testAddress)(uint16_t *fAddr, int fWrite)
{
	*fAddr = MapAddress(*fAddr);

	if (fWrite)
		return (eMemoryAddressResult)(MemoryTypeWrite(*fAddr) & 0x7F);
	return (eMemoryAddressResult)(MemoryTypeRead(*fAddr) & 0x7F);
}
uint8_t __not_in_flash_func(get8)(uint16_t fAddr)
{
  uint16_t addr = fAddr;

  return get8(&addr);
}

bool isRAE()
{
	// stack();
	// printf("\r\n(0x0101 + SP) = (%04X) = %02X", (uint16_t)(0x0101 + SP), get8((uint16_t)(0x0101 + SP)));
	// printf("\r\n(0x0102 + SP) = (%04X) = %02X", (uint16_t)(0x0102 + SP), get8((uint16_t)(0x0102 + SP)));
	return ((int)get8((uint16_t)(0x0101 + SP)) | ((int)(get8((uint16_t)(0x0102 + SP)) << 8)) == 0xEFBA);

	// printf("\r\nReturn address = %04X\r\n", returnAddress);
	// return true;
}
int flashRead(int fFileHandle, uint8_t *fAddress, size_t fNrBytes)
{
	int result = pico_read(fFileHandle, fAddress, fNrBytes);

	if(result < 0)
	{
		printf("\r\nWrite failed %s", pico_errmsg(result));
		pico_close(fFileHandle);
	}
	return result;
}
int flashWrite(int fFileHandle, const uint8_t *fAddress, size_t fNrBytes)
{
	int result = pico_write(fFileHandle, fAddress, fNrBytes);

	if(result < 0)
	{
		printf("\r\nWrite failed %s", pico_errmsg(result));
		pico_close(fFileHandle);
	}
	return result;
}
bool reportError(FRESULT fResult, UINT fBytesRead, UINT fExpectedBytesRead)
{
	if(fResult != FR_OK)
	{
		printf("\r\nRead error\r\n");
		return false;
	}
	if(fBytesRead != fExpectedBytesRead)
	{
		printf("\r\nEnd of file encountered\r\n");
		return false;
	}
	return true;
}
bool readFile(const char * fFilename, bool fRelocate)
{
	TFile		file;
	UINT 		bytesRead = 0;
	uint16_t 	baseAddress = 0;
	uint8_t  	fileID;
	uint16_t 	start;
	uint16_t	end;
	FRESULT 	result;
	char	   *p;

	fileID = CurrentTape.Path.GetFileInt();

	result = CurrentTape.OpenFileName(&file, fFilename, false);
	if(!reportError(result, 0, 0))
		return false;

	result = file.Read((char *)(&start), 2, &bytesRead);
	if(!reportError(result, bytesRead, 2))
		return false;

	result = file.Read((char *)(&end), 2, &bytesRead);
	if(!reportError(result, bytesRead, 2))
		return false;

	size_t readSize = end - start;

	if(fRelocate)
	{
		start = getOperand16(FILE_START);
		set16(0xFE, start);
		end   = getOperand16(FILE_END);

		if(end - start < readSize)
			readSize = end - start;

		// printf("Relocate: Start = %04X, End = %04X\r\n", start, end);
	}

	result = file.Read((char *)(&Memory[start]), readSize, &bytesRead);
	if(!reportError(result, bytesRead, readSize))
		return false;

	// printf("Data read %04X bytes, Memory[0x0128] = %02X\r\n\r\n", bytesRead, Memory[0x0128]);

	set16(FILE_START, start);
	set16(BUFAD, start);
	set16(FILE_END, end);
	file.Close();

	return true;
}
int writeFile(const char * fFilename, uint8_t fID, uint16_t fStart, uint16_t fEnd)
{
	TFile	file;
	FRESULT	result;
	UINT 	bytesWritten = 0;

	if(fEnd < fStart)
	{
		printf("\r\nEnd address before start : start = %04X, end = %04X\r\n", fStart, fEnd);
		return -1;
	}
	CurrentTape.OpenFileName(&file, fFilename, true);
	if(!reportError(result, 0, 0))
		return false;

	result = file.Write((char *)(&fStart), 2, &bytesWritten);
	if(!reportError(result, bytesWritten, 2))
		return -1;

	result = file.Write((char *)(&fEnd), 2, &bytesWritten);
	if(!reportError(result, bytesWritten, 2))
		return -1;

	result = file.Write((char *)(&Memory[fStart]), fEnd - fStart, &bytesWritten);
	if(!reportError(result, bytesWritten, fEnd - fStart))
		return -1;

	result = file.Close();
	if(!reportError(result, 0, 0))
		return -1;

	return bytesWritten;
}

int selectCRT(uint16_t *fAddr)
{
    int rc;

	rc = stdio_getchar_timeout_us(0);
	if(rc >= 0)
	{
		SR.Bits.N = 1;
		PC += 3;
		*fAddr = MapAddress(PC - 1);
	}

	return Memory[*fAddr];
}
int baudCRT(uint16_t *fAddr)
{
	A = 'Q';
	setNZ(A);
	Memory[0xA651] = 0x01;  // 4800 Baud
	PC = popWord() + 1;
	*fAddr = MapAddress(PC);
	PC++;   // When it returns, PC must point to the instruction operand as it would in the early part of SingleStep
	USBRedirect = true;
	printf("\r\nRedirecting I/O via USB\r\n");
	// if(DebuggerOn)
	// 	DoWatch(*fAddr, eAccess::Read, Memory[*fAddr]);
	return Memory[*fAddr];
}
int readCRT(uint16_t *fAddr)
{
    int rc;

	do
	{
		rc = stdio_getchar_timeout_us(0);
		if(gpio_get(RESETPin) == 0)
		{
			while(gpio_get(RESETPin) == 0);
			reset();
			*fAddr = MapAddress(PC);
			PC++;
			if(DebuggerOn)
				DoWatch(*fAddr, eAccess::Read, Memory[*fAddr]);
			return Memory[*fAddr];
		}
	} while (rc <= 0);

	// Test for F1 key. Any other character will be processed as a normal key.
	if(rc == 0x1B)
	{
		int rc1 = stdio_getchar_timeout_us(0);
		if(rc1 == '[')
		{
			rc1 = stdio_getchar_timeout_us(0);
			if(rc1 == '1')
			{
				rc1 = stdio_getchar_timeout_us(0);
				if(rc1 == '1')
				{
					rc1 = stdio_getchar_timeout_us(0);
					if(rc1 == '~')
					{
						// F1 found. Call Debugger.
						CommandLine(false);
						// Returns here on Debugger CONTINUE command
						CRLF();
						// Process a key for processing : Cannot be another F1.
						do
						{
							rc = stdio_getchar_timeout_us(0);
							if(gpio_get(RESETPin) == 0)
							{
								while(gpio_get(RESETPin) == 0);
								reset();
								*fAddr = MapAddress(PC);
								PC++;
								if(DebuggerOn)
									DoWatch(*fAddr, eAccess::Read, Memory[*fAddr]);
								return Memory[*fAddr];
							}
						} while (rc <= 0);
					}
				}
			}
		}
	}
					
		if(Memory[0xA653] & 0x80)
			stdio_putchar(rc & 0x7F);	// Excho character if TECHO set
		A = rc;
		setNZ(A);
		PC = popWord() + 1;
		*fAddr = MapAddress(PC);
		PC++;
	// if(DebuggerOn)
	// 	DoWatch(*fAddr, eAccess::Read, Memory[*fAddr]);

	return Memory[*fAddr];
}
int writeCRT(uint16_t *fAddr)
{
	stdio_putchar(A & 0x7F);
	if(DebuggerOn)
		DoWatch(*fAddr, eAccess::Read, Memory[*fAddr]);
	
	return Memory[*fAddr];
}
int tstatCRT(uint16_t *fAddr)
{
	if(stdio_getchar_timeout_us(0) == 0x1B)
	{
		A = 0x00;
		setNZ(A);
		SR.Bits.C = 1;
		PC = popWord() + 1;
	}
	else
	{
		if(gpio_get(RESETPin) == 0)
		{
			while(gpio_get(RESETPin) == 0);
			reset();
		}
		else
		{
			A = 0x80;
			setNZ(A);
			SR.Bits.C = 0;
			PC = popWord() + 1;
		}
	}
	*fAddr = MapAddress(PC++);
	return Memory[*fAddr];
}

FRESULT findNextFile(std::string *fFilename)
{
	// printf("\r\nfindNextFileID()\r\n");
	std::vector<TInfo>  fileList;
    TPath               path(&CurrentTape.Path);
	std::string			currentFilename = path.GetFile();
	
	*fFilename = "";
	if(!CurrentTape.ListFiles(&fileList))
	{
		printf("No files found\r\n");
		return FR_NO_FILE;
	}
    for(int i = 0; i < fileList.size(); i++)
    {
        path.SetFileSequenceNr(fileList[i].Name.c_str());
	// printf("Checking %s\r\n", fileList[i].Name.c_str());
		if(path.GetSequenceNr() >= CurrentTape.CurrentSequenceNr)
		{
		// printf("File name ACCEPTED %s : Current filename = %s\r\n", path.GetFile().c_str(), currentFilename	.c_str());
				CurrentTape.Path.SetSequenceNr(path.GetSequenceNr() + 1);
				*fFilename = path.GetFile();
				return FR_OK;
		}
    }
	printf("End of tape\r\n");
	return FR_NO_FILE;
}
int loadFromTape(uint16_t *fAddr)
{
	if(Configuration.UseStorage())
	{
		uint8_t 		id = Memory[FILE_ID];
		bool 			relocate = (id == 0xFF);
		std::string		fileID;
		char			filename[10];
		TInfo 			fileInfo;

		PC = popWord() + 1;
		*fAddr = MapAddress(PC++);
		SR.Bits.C = 1;

		if((id == 0x00) || (id == 0xFF))
		{ // Load next
			std::string		fileID;
			
			if(findNextFile(&fileID) != FR_OK)
			{
				if(DebuggerOn)
					DoWatch(*fAddr, eAccess::Read, Memory[*fAddr]);
				return Memory[*fAddr];	
			}

			id = std::stoi(fileID, 0, 16);
		}
		sprintf(filename, "%02X", id);
		fileInfo = CurrentTape.FindFile(filename, CurrentTape.Path.GetTapePath(false).c_str(), CurrentTape.CurrentSequenceNr);
		Memory[FILE_ID] = id;
		readFile(fileInfo.Name.c_str(), relocate);
		SR.Bits.C = 0;
		SR.Bits.Z = 1;
		set8(TAPE_MODE, 0x80);
	}

	return Memory[*fAddr];	
}
int saveToTape(uint16_t *fAddr)
{
	if(Configuration.UseStorage())
	{
		uint8_t 			result;
		uint8_t 			fileID = getOperand8(FILE_ID);
		uint16_t 			start = getOperand16(FILE_START);
		uint16_t 			end = getOperand16(FILE_END);
		std::vector<TInfo> 	fileList;
		char 				filename[10];
		bool				calledFromRAE = isRAE();
		bool 				fileFound = false;

		PC = popWord() + 1;
		*fAddr = MapAddress(PC++);
		SR.Bits.C = 1;

		// Default to append tape
		CurrentTape.CurrentSequenceNr = CurrentTape.GetNextSequenceNr();

		if(fileID == 0xFF)
		{
			printf("\r\nFile ID cannot be %02X", fileID);
			return Memory[*fAddr];
		}
		else if(fileID == 0x00)
		{
			if(calledFromRAE)
			{
				fileID = Memory[0x0128];

				if(Configuration.RAEOverwrite)
				{
					CurrentTape.ListFiles(&fileList);
					for(int i = 0; i < fileList.size(); i++)
					{
						TPath path(&CurrentTape.Path);

						path.SetFileSequenceNr(fileList[i].Name.c_str());
						if(path.GetFileInt() == fileID)
						{
							if (((start == 0x0128) && (fileList[i].Size == 9))
							|| ((start != 0x0128) && (fileList[i].Size > 9)))
							{
								if(!fileFound)
								{
									CurrentTape.DeleteFileEntry(fileList[i].Name.c_str(), CurrentTape.Path.GetDevice());
									CurrentTape.CurrentSequenceNr = path.GetSequenceNr();
									fileFound = true;	
								}
								break;
							}
						}
					}
				}
			}		
		}

		if(!CurrentTape.ListFiles(&fileList))
		{
			printf("\r\nFiles inaccessible");
			return Memory[*fAddr];
		}
		sprintf(filename, "%02X", fileID);  // P1L is filename

		if(fileList.size() > 0)
		{
			CurrentTape.Path.SetFileSequenceNr(fileList.back().Name.c_str());
			CurrentTape.Path.SetSequenceNr(CurrentTape.CurrentSequenceNr);
		}
		else
			CurrentTape.Path.SetSequenceNr(1);

		CurrentTape.Path.SetFile(filename);

		result = writeFile(CurrentTape.Path.GetPath(true, true).c_str(), fileID, start, end);

		if(result < 0)
			return Memory[*fAddr];

		SR.Bits.C = 0;
	}

	return Memory[*fAddr];
}

int setGPIO25LED(uint16_t *fAddr, uint8_t fValue)
{
	UserPin25LED = true;
	gpio_put(TimingIndicatorPin, fValue & 0x01);
		
	PC = popWord() + 1;
	*fAddr = MapAddress(PC++);

	return Memory[*fAddr];
}

uint8_t __not_in_flash_func(processCommon)(eMemoryAddressResult readType, uint16_t *fAddr)
{
	uint8_t result;
    int rc;

	switch (readType)
    {
        case eMemoryAddressResult::NOTHING:
        default:
           if(DebuggerOn)
                DoWatch(*fAddr, eAccess::Read, 0xFF);
            return 0xFF;
        case eMemoryAddressResult::MEMORY:
        case eMemoryAddressResult::SelectCRT:
        case eMemoryAddressResult::BaudCRT:
        case eMemoryAddressResult::ReadCRT:
        case eMemoryAddressResult::WriteCRT:
        case eMemoryAddressResult::TSTAT_CRT:
		case eMemoryAddressResult::LoadTape:
		case eMemoryAddressResult::SaveTape:
           if(DebuggerOn)
                DoWatch(*fAddr, eAccess::Read, Memory[*fAddr]);
             return Memory[*fAddr];
        case eMemoryAddressResult::ReadORA:
            result = readORA();
           if(DebuggerOn)
                DoWatch(*fAddr, eAccess::Read, result);
            return result;
        case eMemoryAddressResult::ReadORB:
            result = readORB();
           if(DebuggerOn)
                DoWatch(*fAddr, eAccess::Read, result);
            return result;
        case eMemoryAddressResult::ReadIRQFlags:
            result = SY6532_TimerIRQTriggered | SY6532_PA7Triggered;
            SY6532_PA7IRQRequest = 0;
           if(DebuggerOn)
                DoWatch(*fAddr, eAccess::Read, result);
            return result;
        case eMemoryAddressResult::ReadTimerIRQON:
            result = SY6532_TimerValue;
            SY6532_TimerIRQRequest = 1;
            SY6532_TimerIRQTriggered = 0;
           if(DebuggerOn)
                DoWatch(*fAddr, eAccess::Read, result);
            return result;
        case eMemoryAddressResult::ReadTimerIRQOFF:
            result = SY6532_TimerValue;
            SY6532_TimerIRQRequest = 0;
            SY6532_TimerIRQTriggered = 0;
           if(DebuggerOn)
                DoWatch(*fAddr, eAccess::Read, result);
            return result;
	}

    return 0xFF;
}
int __not_in_flash_func(processExecuteOnly)(eMemoryAddressResult readType, uint16_t *fAddr)
{
	uint8_t result;
    int rc;

	switch (readType)
    {
        case eMemoryAddressResult::SelectCRT:
			return selectCRT(fAddr);

         case eMemoryAddressResult::BaudCRT:
			return baudCRT(fAddr);

       case eMemoryAddressResult::ReadCRT:
			if(USBRedirect)
				return readCRT(fAddr);
			else
				return Memory[*fAddr];

        case eMemoryAddressResult::WriteCRT:
			return writeCRT(fAddr);

        case eMemoryAddressResult::TSTAT_CRT:
			return tstatCRT(fAddr);

		case eMemoryAddressResult::IgnoreSubroutine:
			{
				PC = popWord() + 1;
				*fAddr = MapAddress(PC++);
				// printf("\r\nAfter Ignore %04X = %02X, PC = %04X\r\n", *fAddr, Memory[*fAddr], PC);
			}
			return Memory[*fAddr];
			
		case eMemoryAddressResult::LoadTape:
			return loadFromTape(fAddr);

		case eMemoryAddressResult::SaveTape:
			return saveToTape(fAddr);

		case eMemoryAddressResult::SetGPIO25LED:
			return setGPIO25LED(fAddr, A);
	}

    return -1;
}

uint8_t __not_in_flash_func(get8)(uint16_t *fAddr, bool fInstructionFetch)
{
	eMemoryAddressResult readType = testAddress(fAddr, 0);

	if(fInstructionFetch)
	{
		int result = processExecuteOnly(readType, fAddr);

		if(result != -1)
			return (uint8_t)result;
	}
	return processCommon(readType, fAddr);
}
uint8_t __not_in_flash_func(get8NoSideEffect)(uint16_t fAddr)
{
    uint8_t result;
    int rc;
    uint16_t addr = fAddr;

    switch (testAddress(&addr, 0))
    {
        case eMemoryAddressResult::NOTHING:
        default:
        	return 0xFF;
        case eMemoryAddressResult::MEMORY:
        case eMemoryAddressResult::SelectCRT:
        case eMemoryAddressResult::BaudCRT:
        case eMemoryAddressResult::ReadCRT:
        case eMemoryAddressResult::WriteCRT:
        case eMemoryAddressResult::TSTAT_CRT:
            return Memory[addr];
        case eMemoryAddressResult::ReadORA:
            return readORA();
        case eMemoryAddressResult::ReadORB:
            return readORB();
        case eMemoryAddressResult::ReadIRQFlags:
            return SY6532_TimerIRQTriggered | SY6532_PA7Triggered;
        case eMemoryAddressResult::ReadTimerIRQON:
            return SY6532_TimerValue;
        case eMemoryAddressResult::ReadTimerIRQOFF:
            return SY6532_TimerValue;
    }
    return 0xFF;
}
uint8_t __not_in_flash_func(set8)(uint16_t fAddr, uint8_t fValue)
{
  uint16_t addr = fAddr;

  switch (testAddress(&addr, 1))
  {
    case eMemoryAddressResult::NOTHING:
    default:
	    fValue = 0xFF;
		break;
    case eMemoryAddressResult::MEMORY:
     	Memory[addr] = fValue;
      	break;
    case eMemoryAddressResult::WriteDDRA:
       writeDDRA(addr, fValue);
       break;
    case eMemoryAddressResult::WriteDDRB:
       	writeDDRB(addr, fValue);
		break;
    case eMemoryAddressResult::WriteORA:
      writeORA(addr, fValue);
      break;
    case eMemoryAddressResult::WriteORB:
        writeORB(addr, fValue);
      	break;
  case eMemoryAddressResult::WriteTimerDiv1IRQON:
		SY6532_TimerValue = fValue;
		SY6532_TimerIRQRequest = 1;
		SY6532_Timer_Prescaler = 1;
		SY6532_TimerIRQTriggered = false;
		break;
    case eMemoryAddressResult::WriteTimerDiv8IRQON:
		SY6532_TimerValue = fValue;
		SY6532_Timer_Prescaler = 8;
		SY6532_TimerIRQRequest = 1;
		SY6532_TimerIRQTriggered = false;
		break;
    case eMemoryAddressResult::WriteTimerDiv64IRQON:
		SY6532_TimerValue = fValue;
		SY6532_Timer_Prescaler = 64;
		SY6532_TimerIRQRequest = 1;
		SY6532_TimerIRQTriggered = false;
		break;
    case eMemoryAddressResult::WriteTimerDiv1024IRQON:
		SY6532_TimerValue = fValue;
		SY6532_Timer_Prescaler = 1024;
		SY6532_TimerIRQRequest = 1;
		SY6532_TimerIRQTriggered = false;
		break;
    case eMemoryAddressResult::WriteTimerDiv1IRQOFF:
		SY6532_TimerValue = fValue;
		SY6532_TimerIRQRequest = 0;
		SY6532_Timer_Prescaler = 1;
		SY6532_TimerIRQTriggered = false;
		break;
    case eMemoryAddressResult::WriteTimerDiv8IRQOFF:
		SY6532_TimerValue = fValue;
		SY6532_Timer_Prescaler = 8;
		SY6532_TimerIRQRequest = 0;
		SY6532_TimerIRQTriggered = false;
		break;
    case eMemoryAddressResult::WriteTimerDiv64IRQOFF:
		SY6532_TimerValue = fValue;
		SY6532_Timer_Prescaler = 64;
		SY6532_TimerIRQRequest = 0;
		SY6532_TimerIRQTriggered = false;
		break;
    case eMemoryAddressResult::WriteTimerDiv1024IRQOFF:
		SY6532_TimerValue = fValue;
		SY6532_Timer_Prescaler = 1024;
		SY6532_TimerIRQRequest = 0;
		SY6532_TimerIRQTriggered = false;
		break;
    case eMemoryAddressResult::PA7IRQOFFNEG:
		SY6532_PA7IRQRequest = 0;
		SY6532_PA7NegativeEdge = 1;
		break;
    case eMemoryAddressResult::PA7IRQOFFPOS:
		SY6532_PA7IRQRequest = 0;
		SY6532_PA7NegativeEdge = 0;
		break;
    case eMemoryAddressResult::PA7IRQONNEG:
		SY6532_PA7IRQRequest = 1;
		SY6532_PA7NegativeEdge = 1;
		break;
    case eMemoryAddressResult::PA7IRQONPOS:
		SY6532_PA7IRQRequest = 1;
		SY6532_PA7NegativeEdge = 0;
		break;
    case eMemoryAddressResult::WriteProtect:
		if((addr >= 0x0400) && (addr < 0x0800))
		{	// RAM Page 1
			if(RAMPage1WriteProtected && Configuration.SimWP)
			{
  				if(DebuggerOn)
    				DoWatchWP(addr, fValue);
				return 0xFF;
			}
		}
		else if((addr >= 0x0800) && (addr < 0x0C00))
		{	// RAM Page 2
			if(RAMPage2WriteProtected && Configuration.SimWP)
			{
  				if(DebuggerOn)
    				DoWatchWP(addr, fValue);
				return 0xFF;
			}
		}
		else if((addr >= 0x0C00) && (addr < 0x1000))
		{	// RAM Page 3
			if(RAMPage3WriteProtected && Configuration.SimWP)
			{
  				if(DebuggerOn)
    				DoWatchWP(addr, fValue);
				return 0xFF;
			}
		}
		else
		{	// System RAM
			if(SystemRAMWriteProtected && Configuration.SimWP)
			{
  				if(DebuggerOn)
    				DoWatchWP(addr, fValue);
				return 0xFF;
			}
		}
		Memory[addr] = fValue;
		break;
    case eMemoryAddressResult::WriteProtectPort:
		Memory[addr] = fValue;
		SystemRAMWriteProtected = ((fValue & 0x01) == 0);
		RAMPage1WriteProtected = ((fValue & 0x02) == 0);
		RAMPage2WriteProtected = ((fValue & 0x04) == 0);
		RAMPage3WriteProtected = ((fValue & 0x08) == 0);
		break;
  }
  if(DebuggerOn)
    DoWatch(addr, eAccess::Write, fValue);
  return fValue;
}
uint16_t __not_in_flash_func(get16)(uint16_t fAddr)
{
	uint16_t addr = fAddr;

	switch (testAddress(&addr, 0))
	{
		case eMemoryAddressResult::NOTHING:
		default:
			return 0xFFFF;
		case eMemoryAddressResult::MEMORY:
		{
			uint16_t value = (uint16_t)(Memory[addr + 1] << 8) | (uint16_t)Memory[addr];

			if (DebuggerOn)
				DoWatch16(addr, eAccess::Read, value);

			return value;
		}
	}
}
uint16_t __not_in_flash_func(set16)(uint16_t fAddr, uint16_t fValue)
{
	uint16_t addr = fAddr;

	switch (testAddress(&addr, 0))
	{
	case eMemoryAddressResult::MEMORY:
		Memory[addr] = fValue & 0xFF;
		Memory[addr + 1] = fValue >> 8;

		if (DebuggerOn)
			DoWatch16(addr, eAccess::Write, fValue);

		break;
	}
	return fValue;
}
uint8_t __not_in_flash_func(getOperand8)(uint16_t fAddr)
{
	if (DebuggerOn)
		DoWatch(fAddr, eAccess::Read, Memory[fAddr]);
	return Memory[fAddr];
}
uint16_t __not_in_flash_func(getOperand16)(uint16_t fAddr)
{
	int16_t value = Memory[fAddr] | (Memory[fAddr + 1] << 8);

	if (DebuggerOn)
		DoWatch16(fAddr, eAccess::Read, value);
	return value;
}