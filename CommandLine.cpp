#include <cstdint>
#include "CommandLine.h"
#include "CommandLine_FileAndTape.hpp"

bool DebuggerActive = false;        // Bootup with debugger
bool DebuggerOn = false;            // Debugger is currently on : Allows debugger to be temporarily deactivated
int  traceDepth = 100;

std::vector<SBP>                    Breakpoints;
std::vector<SVariable>              WatchVariables;
std::vector<TTraceData>             TraceData;
int                                 StepOverBreakpoint;

uint8_t SP_Location;

char    InstructionText[129];

bool IsDebug()
{
    DebuggerOn = DebuggerActive && !(MemoryTypeRead(PC) & 0x80);

    return DebuggerOn;
}
void addInstruction(TTraceData fTraceData)
{
    TraceData.push_back(fTraceData);
    if(TraceData.size() > TRACELIMIT)
        TraceData.erase(TraceData.begin());
}
void printRegisters(int fPCAdjust)
{
    TTraceData traceData = Disassemble(PC + fPCAdjust, InstructionText);
    printf("\r\n%-48s%s\r\n", InstructionText, traceData.RegisterString(fPCAdjust).c_str());
}
void prompt()
{
    printf("\r\n$ ");
}
bool IsBreakpoint(bool *fIsStepover)
{
    *fIsStepover = false;
    for(std::vector<SBP>::iterator i = Breakpoints.begin(); i != Breakpoints.end(); i++)
    {
        if(i->Address == PC)
        {
            if(i->OutputOnly)
            {
                printf("\r\nBreakpoint %04X output", i->Address);
                printRegisters();
                return false;
            }
            return true;
        }
    }
    return false;
}
void DoWatch(uint16_t fAddress, eAccess fAccess, uint8_t fValue)
{
    char instruction[129];

    for(std::vector<SVariable>::iterator i = WatchVariables.begin(); i != WatchVariables.end(); i++)
    {
        if(i->Address == fAddress)
        {
            TTraceData traceData = Disassemble(InstructionPC, instruction);
    
            if((fAccess == eAccess::Read) && ((i->Access == eAccess::Read) || (i->Access == eAccess::ReadWrite)))
                printf("\r\nRead from %04X value %02X\r\n%-48s%s", fAddress, fValue, instruction, traceData.RegisterString().c_str());
            else if((fAccess == eAccess::Write) && ((i->Access == eAccess::Write) || (i->Access == eAccess::ReadWrite)))
                printf("\r\nWrite to %04X value %02X\r\n%-48s%s", fAddress, fValue, instruction, traceData.RegisterString().c_str());
            if(i->Break)
                ActivateBreakpoint = true;
            return;
        }
    }    
}
void DoWatchWP(uint16_t fAddress, uint8_t fValue)
{
    char instruction[129];

    for(std::vector<SVariable>::iterator i = WatchVariables.begin(); i != WatchVariables.end(); i++)
    {
        if(i->Address == fAddress)
        {
            TTraceData traceData = Disassemble(InstructionPC, instruction);
    
            printf("\r\nWrite to %04X of value %02X write protected\r\n%-48s%s", fAddress, fValue, instruction, traceData.RegisterString().c_str());
            if(i->Break)
                ActivateBreakpoint = true;
            return;
        }
    }    
}
void DoWatch16(uint16_t fAddress, eAccess fAccess, uint16_t fValue)
{
    char instruction[129];

    for(std::vector<SVariable>::iterator i = WatchVariables.begin(); i != WatchVariables.end(); i++)
    {
        if(i->Address == fAddress)
        {
            Disassemble(InstructionPC, instruction);
    
            if((fAccess == eAccess::Read) && ((i->Access == eAccess::Read) || (i->Access == eAccess::ReadWrite)))
                printf("\r\nRead low byte from %04X value %04X\r\n%s", fAddress, fValue, instruction);
            else if((fAccess == eAccess::Write) && ((i->Access == eAccess::Write) || (i->Access == eAccess::ReadWrite)))
                printf("\r\nWrite low byte to %04X value %04X\r\n%s", fAddress, fValue, instruction);
            if(i->Break)
                ActivateBreakpoint = true;
            return;
        }
        if(i->Address == (fAddress + 1))
        {
            Disassemble(InstructionPC, instruction);
    
            if((fAccess == eAccess::Read) && ((i->Access == eAccess::Read) || (i->Access == eAccess::ReadWrite)))
                printf("\r\nRead high byte from %04X value %04X\r\n%s", fAddress, fValue, instruction);
            else if((fAccess == eAccess::Write) && ((i->Access == eAccess::Write) || (i->Access == eAccess::ReadWrite)))
                printf("\r\nWrite high byte to %04X value %04X\r\n%s", fAddress, fValue, instruction);
            if(i->Break)
                ActivateBreakpoint = true;
            return;
        }
    }    
}
std::string cleanup(std::string *fText)
{
    std::string result;

    for(int i = 0; i < fText->size(); i++)
    {
        if((*fText)[i] == 0x7F)
        {
            if(result.size() > 0)
                result = result.substr(0, result.size() - 1);
            else
                continue;
        }
        else
            result.append(&(*fText)[i], 1);
    }
    return result;
}
int getchar()
{
    while (true) 
    {
        // Read character with a timeout
        int c = stdio_getchar_timeout_us(-1);

        if(c >= 0)
        {
            // if(Memory[0xA653] & 0x80)   // Check TECHO flag
                 stdio_putchar(c);
            return c;
        }
    }    
}
void get_text_until_cr(std::string *fText) 
{
    fText->clear();
    while (true) 
    {
        // Read character with a timeout
        int c = getchar();

        // Check for carriage return (CR) or newline (LF)
        if (c == '\r' || c == '\n') 
        {
            // A line ending was found, return true
            *fText = cleanup(fText);
            return;
        }
        if((c == '\x08') || (c == '\x7F'))
        {
            if(fText->length() == 0)
                continue;
            fText->erase(fText->size() - 1);
            continue;
        }
        // Append valid character to the string
        *fText += (char)c;
    }
}
void dumpMemory(uint16_t fStart, uint16_t fSize)
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Make this start on XXX0 boundary. Use sapes for unrequested values.
  printf("\r\nAddr   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
  for(uint16_t i = 0; i < fSize; i += 16)
  {
    printf("\r\n%04X: ", fStart + i);
    for(uint16_t j = 0; j < 16; j++)
        printf("%02X ", get8NoSideEffect(fStart + i + j));
  }
  CRLF();
}
std::string printAddress(uint16_t fAddress)
{
    const char *label = Lookup(fAddress);
          char result[33];

    if(label == NULL)
        sprintf(result, "    %04X           ", fAddress);
    else
        sprintf(result, "    %04X : %-8s", fAddress, label);

    return std::string(result);
}

void dump()
{
    const char *startText = strtok(NULL, ", ");
    const char *sizeText  = strtok(NULL, ", ");
    char *p;
    bool result = true;

    if(strlen(startText) == 0)
    {
        printf("\r\nMissing start and size parameters");
        result = false;
    }
    else if(strlen(sizeText) == 0)
        sizeText = "100";
 
    int start = strtoul(startText, & p, 16 );

    if(*p != 0)
    {
        printf("\r\nError in start parameter %s\r\n", startText);
        result = false;
    }

    int size = strtoul(sizeText, &p, 16);

    if(*p != 0)
    {
        printf("\r\nError in size parameter %s\r\n", sizeText);
        result = false;
    }

    if(result)
    {
        dumpMemory(start, size) ;
        CRLF();
    }
}
void stack()
{
    printf("\r\nSP: 01%02X", SP);
    for(int i = 0x0101 + SP; i < 0x200; i++)
        printf("\r\n%04X: %02X", i, Memory[i]);
    printf("\r\n\r\n");
}
bool run()
{
    TTraceData traceData;
    const char *countText = strtok(NULL, ", ");
    char *p;
    uint64_t count = UINT64_MAX;
    uint64_t instructionNr = 0;

    DebuggerActive = true;
    if(countText != NULL)
    {
        count = strtoul(countText, &p, 10);
        if(*p != 0)
        {
            printf("\r\nOnly valid parameter is decimal integer");
            return false;
        }
    }

    CRLF();
    bool isStepOverBreakpoint;
    while(instructionNr++ < count)
    {
        if(IsDebug())
        {
            if(IsBreakpoint(&isStepOverBreakpoint) || ActivateBreakpoint)
            {
                if(!isStepOverBreakpoint)
                    printf("\r\nBreakpoint encountered at %04X\r\n", PC);
                ActivateBreakpoint = false;
                return true;
            }
            traceData.PC = PC;
            traceData.Snapshot(PC);
        }
        SingleStep();
        if(DebuggerOn)
            addInstruction(traceData);
    }
    return false;
}
void bp()
{
    char        *operation = strtok(NULL, ", ");
    char        *targetText  = strtok(NULL, ", ");
    char        *outputOnlyText = strtok(NULL, "");
    char        *p;

    bool        result = true;
    bool        all = false;
    bool        set = false;
    bool        outputOnly = false;
    uint16_t    address;

    if(strlen(operation) == 0)
    {
        printf("\r\nMissing operation and target parameters\r\n");
        result = false;
    }
    if(strcmp(operation, "LIST") == 0)
    {
        printf("\r\nBreakpoint List\r\n");
        for(std::vector<SBP>::iterator i = Breakpoints.begin(); i != Breakpoints.end(); i++)
        {
            std::string outputOnlyMsg = i->OutputOnly ? "Output Only" : "";

            printf("    %s    %s\r\n", printAddress(i->Address).c_str(), outputOnlyMsg.c_str());
        }
        CRLF();
        return;
    }
    // Just SET or CLEAR beyond this point
    if(strlen(targetText) == 0)
    {
        printf("\r\nMissing target parameter\r\n");
        result = false;
    }
    if(strcmp(operation, "SET") == 0)
    {
        set = true;
        if((outputOnlyText == NULL) || (strcmp(outputOnlyText, "") == 0))
            outputOnly = false;
        else if ((outputOnlyText[0] == 'o') || (outputOnlyText[0] == 'O'))
            outputOnly = true;
        else
        {
            printf("\r\nLast parameter must be 'O' to indicate output only\r\n");
            return;
        }
    }
    else if (strcmp(operation, "CLEAR") == 0)
    {
        if((outputOnlyText != NULL) && (strcmp(outputOnlyText, "") != 0))
        {
            printf("\r\nOutput only is only valid on SET\r\n");
            return;
        }
        set = false;
    }
    else
    {
        printf("\r\nOperation must be LIST, SET, or CLEAR\r\n");
        result = false;
    }
    
    if(strcmp(targetText, "ALL") == 0)
        all = true;
    else
    {
        int lookup = Lookup(targetText);

        if(lookup >= 0)
            address = (uint16_t)lookup;
        else
        {
            address = strtoul(targetText, & p, 16 );

            if(*p != 0)
            {
                printf("\r\nError in target address %s\r\n", targetText);
                result = false;
            }
        }
    }
    if(set && all)
    {
        printf("\r\nCan't do SET ALL\r\n");
        result = false;
    }
    if(result)
    {
        if(all)
        {
            Breakpoints.clear();
        }
        else if(set)
        {
            for(std::vector<SBP>::iterator i = Breakpoints.begin(); i != Breakpoints.end(); i++)
            {
                if(i->Address == address)
                {
                    printf("\r\nBreakpoint already exists for this address %04X\r\n", address);
                    return;
                }
            }    
            Breakpoints.push_back({ address, outputOnly });
        }
        else
        {
            for(std::vector<SBP>::iterator i = Breakpoints.begin(); i != Breakpoints.end(); i++)
            {
                if(i->Address == address)
                {
                    Breakpoints.erase(i);
                    break;
                }
            }
        }
        CRLF();
    }
}
void printTrace()
{
    int start = TraceData.size() > traceDepth ? TraceData.size() - traceDepth : 0;
    int end = TraceData.size();

    printf("\r\nInstruction Trace\r\n");
    for(int index = start; index < end; index++)
        TraceData[index].PrintTraceData(InstructionText);
}
void Trace()
{
    const char *operationText = strtok(NULL, ", ");
          char *p;

    if(strlen(operationText) == 0)
        printTrace();
    else if(strcmp(operationText, "CLEAR") == 0)
        TraceData.clear();
    else if(strcmp(operationText, "ALL") == 0)
    {
        traceDepth = TRACELIMIT;
        printTrace();
    }
    else
    {   
        int depth = strtoul(operationText, &p, 10);

        if(*p != 0)
            printf("\r\nOnly valid parameter is CLEAR or decimal integer");
        else if (depth > TRACELIMIT)
            printf("\r\nCannot print more than %d traces", TRACELIMIT);
        else
        {
            traceDepth = depth;
            printTrace();
        }
    }
}
std::vector<SDebuggerDisable> getDebugAsList()
{
    bool                            sequenceDisable = false;
    uint16_t                        startAddress = 0;
    std::vector<SDebuggerDisable>   result;

    for(int i = 0; i <= 0xFFFF; i++)
    {
        bool disable = MemoryTypeRead(i) & 0x80;

        if(!sequenceDisable && disable)
            startAddress = i;
        else if (sequenceDisable && !disable)
        {
            SDebuggerDisable newRecord = { startAddress, (uint16_t)(i - 1) };

            result.push_back(newRecord);
        }
        sequenceDisable = disable;
    }
    if(sequenceDisable)
    {
        SDebuggerDisable newRecord = { startAddress, 0xFFFF };

        result.push_back(newRecord);
    }
    return result;
}
void setDebugFromList(std::vector<SDebuggerDisable> fList)
{
    for(int i = 0; i <= 0xFFFF; i++)
        MemoryTypeRead(i, (eMemoryAddressResult)(MemoryTypeRead(i) & 0x7F));
    for(int i = 0; i < fList.size(); i++)
    {
        for(int addr = fList[i].StartAddress; addr <= fList[i].EndAddress; addr++)
            MemoryTypeRead(addr, (eMemoryAddressResult)(MemoryTypeRead(addr) | 0x80));
    }
}
void addDebugRange(uint16_t fStart, uint16_t fEnd, bool fDisable)
{
    for(int addr = fStart; addr <= fEnd; addr++)
    {
        if(fDisable)
            MemoryTypeRead(addr, (eMemoryAddressResult)(MemoryTypeRead(addr) | 0x80));
        else
            MemoryTypeRead(addr, (eMemoryAddressResult)(MemoryTypeRead(addr) & 0x7F));
    }
}
void debugLIST()
{
    std::vector<SDebuggerDisable> list = getDebugAsList();

    printf("\r\nDebug disabled at addresses :-\r\n");
    for(int i = 0; i < list.size(); i++)
        printf("  from %s to %s\r\n", printAddress(list[i].StartAddress).c_str(), printAddress(list[i].EndAddress).c_str());
 }
void debugSAVE()
{
    std::vector<SDebuggerDisable> debugList = getDebugAsList();
    TDebugInfo flash;

    flash.WriteFlash(debugList);
}
void debugLOAD()
{
    TDebugInfo flash;
    std::vector<SDebuggerDisable> debugRanges = flash.ReadFlash();

    for(int i = 0; i < debugRanges.size(); i++)
        addDebugRange(debugRanges[i].StartAddress, debugRanges[i].EndAddress, true);
}
void debugERASE()
{
    TDebugInfo flash;

    flash.Erase();
}
void debugONOFF(bool fDisable)
{
    const char       *startText = strtok(NULL, ", ");
    const char       *endText = strtok(NULL, ", ");
          char       *p;
          uint16_t   start;
          uint16_t   end;
          bool       success = true;
    
    if(startText == NULL)
    {
        printf("\r\nStart and end addresses are required");
        return;
    }
    if((strcmp(startText, "ALL") == 0) && !fDisable)
    {
        addDebugRange(0, 0xFFFF, false);
        return;
    }
    if(strcmp(startText, "SUPERMON") == 0)
    {
        addDebugRange(0x8000, 0x8FFF, fDisable);
        return;
    }
    if(endText == NULL)
    {
        printf("\r\nEnd addresses are required");
        return;
    }

    int labelValue = Lookup(startText);

    if(labelValue >= 0)
        start = (uint16_t)labelValue;
    else
    {
        start = strtoul(startText, &p, 16);
        if(*p != 0)
        {
            printf("\r\nStart address invalid: %s", startText);
            success = false;
        }
    }
    
    labelValue = Lookup(endText);

    if(labelValue >= 0)
        end = (uint16_t)labelValue;
    else
    {
        end = strtoul(endText, &p, 16);
        if(*p != 0)
        {
            printf("\r\nEnd address invalid: %s", endText);
            success = false;
        }
    }
    if(start > end)
    {
        uint16_t temp = end;
        end = start;
        start = temp;
    }
    if(!success)
        return;
    addDebugRange(start, end, fDisable);
}
void debug()
{
    const char *operationText = strtok(NULL, ", ");
          char *p;

    if(operationText == NULL)
        printf("\r\nNo operation specified");
    else if((strcmp(operationText, "OFF") == 0) || (strcmp(operationText, "DISABLE") == 0))
        debugONOFF(true);
    else if((strcmp(operationText, "ON") == 0) || (strcmp(operationText, "ENABLE") == 0))
        debugONOFF(false);
    else if(strcmp(operationText, "SAVE") == 0)
        debugSAVE();
    else if(strcmp(operationText, "LOAD") == 0)
        debugLOAD();
    else if(strcmp(operationText, "ERASE") == 0)
        debugERASE();
    else if(strcmp(operationText, "LIST") == 0)
        debugLIST();
}
void stepOneInstruction(bool fContinuous = false)
{
    TTraceData traceData;

    while(!IsDebug())
        Loop();

    traceData = Disassemble(PC, InstructionText);
    SingleStep();
    addInstruction(traceData);
    if(fContinuous)
        printf("\r\n%-48s%s", InstructionText, traceData.RegisterString().c_str());
    else
        printRegisters();
}
bool step(bool fAuto)
{
    uint8_t opcode;
    TTraceData traceData;
    bool continuous = false;
    uint8_t oldSP;

    if(!fAuto)
    {
        const char *contText = strtok(NULL, ", ");

        if((strcmp(contText, "CONT") == 0) || (strcmp(contText, "CONTINUOUS")) == 0)
        {
            bool slow = false;
            const char *slowText = strtok(NULL, ", ");

            if(strcmp(slowText, "SLOW") == 0)
                slow = true;
            else if((slowText != NULL) && (strlen(slowText) > 0))
            {
                printf("Parameter must be SLOW or absent '%d'", slowText);
                return false;
            }

            while(true)
            {
                stepOneInstruction(true);
                int rc = stdio_getchar_timeout_us(0);
                if(rc == 0x03)  // Ctrl-C
                    return false;
                if(slow)
                    sleep_ms(100);
            }
            return false;
        }
        if((contText != NULL) && (strlen(contText) > 0))
        {
            printf("Parameter must be CONT, CONTINUOUS or absent");
            return false;
        }
    }
 
    CRLF();
    while(true)
    {
        printf(":");
        char c = toupper(getchar());

        switch(c)
        {
        case '\x1B':    // Escape
            prompt();
            return false;
        case ' ':       // Step
            stepOneInstruction();
            break;
        case 'C':
            SingleStep();
            return true;
        case '?':
            printRegisters();
            break;
        case 'S':
            stack();
            break;
        case 'T':
            Trace();
            break;
        case 'X':       // Jump over
            if(Memory[MapAddress(PC)] != 0x20)
                stepOneInstruction();
            else
            {
                StepOverBreakpoint = PC + 3;    // Instruction upon return
                while(!IsDebug())
                    Loop();

                while(PC != StepOverBreakpoint)
                {
                    Disassemble(PC, InstructionText);
                    opcode = SingleStep();
                }
                printRegisters();
            }
            break;
        case 'R':   // Run to return
            while(!IsDebug())
                Loop();

            oldSP = SP;
            while(!((Memory[MapAddress(PC)] == 0x40) || (Memory[MapAddress(PC)] == 0x60)) || (SP != oldSP))
            {
                Disassemble(PC, InstructionText);
                opcode = SingleStep();
            }
            printRegisters();
            break;
        default:
            break;
        }
    }
}
void watchClear(const char *fTargetText)
{
    char*p;

    if(strlen(fTargetText) == 0)
    {
        printf("\r\nUnknown target. CLEAR what?");
        return;
    }
    if(strcmp(fTargetText, "ALL") == 0)
    {
        WatchVariables.clear();
        return;
    }

    uint16_t    addr;
    int         labelValue = Lookup(fTargetText);

    if(labelValue >= 0)
        addr = (uint16_t)labelValue;
    else
    {
        addr = strtol(fTargetText, & p, 16 );

        if(*p != 0)
        {
            printf("\r\nError in address parameter %s\r\n", fTargetText);
            return;
        }
    }
    for(std::vector<SVariable>::iterator i = WatchVariables.begin(); i != WatchVariables.end(); i++)
    {
        if(i->Address == addr)
        {
            WatchVariables.erase(i);
            return;
        }
    }    
}
void watchList()
{
    printf("\r\nWatch List\r\n");
        
    for(std::vector<SVariable>::iterator i = WatchVariables.begin(); i != WatchVariables.end(); i++)
        printf("    %s  %s %s\r\n", printAddress( i->Address).c_str(), (i->Access == eAccess::Read ? "Read" : (i->Access == eAccess::Write ? "Write" : "Read/Write")), (i->Break ? "Break" : ""));
}
void watchSet(eAccess fAccess, const char *fTargetText)
{
    char        *p;
    uint16_t    addr;
    int         labelValue = Lookup(fTargetText);
    const char *breakText = strtok(NULL, ", ");

    if(labelValue >= 0)
        addr = (uint16_t)labelValue;
    else
    {
        addr = strtol(fTargetText, & p, 16 );

        if(*p != 0)
        {
            printf("\r\nError in address parameter %s\r\n", fTargetText);
            return;
        }
    }   
    for(std::vector<SVariable>::iterator i = WatchVariables.begin(); i != WatchVariables.end(); i++)
    {
        if(i->Address == addr)
        {
            i->Access = fAccess;
            return;
        }
    }
    bool autoBreak = false;
    if(strcmp(breakText, "BREAK") == 0)
        autoBreak = true;
    else if((breakText != NULL) && (strlen(breakText) > 0))
    {
        printf("\r\nLast parameter must be BREAK or nothing '%s'\r\n", breakText);
        return;
    }
    SVariable watch({ addr, fAccess, autoBreak });
    WatchVariables.push_back(watch);
}
void watchRange(eAccess fAccess, const char *fTargetText)
{
    char        *p;
    uint16_t    startAddr;
    uint16_t    endAddr;
    const char *endLabelText = strtok(NULL, "");
    int         startLabelValue = Lookup(fTargetText);
    int         endLabelValue = Lookup(endLabelText);

    if(startLabelValue >= 0)
        startAddr = (uint16_t)startLabelValue;
    else
    {
        startAddr = strtol(fTargetText, & p, 16 );

        if(*p != 0)
        {
            printf("\r\nError in address parameter %s\r\n", fTargetText);
            return;
        }
    }   
    if(endLabelValue >= 0)
        endAddr = (uint16_t)endLabelValue;
    else
    {
        endAddr = strtol(endLabelText, & p, 16 );

        if(*p != 0)
        {
            printf("\r\nError in address parameter %s\r\n", endLabelText);
            return;
        }
    }
    if(startAddr > endAddr)
    {
        uint16_t temp = startAddr;

        startAddr = endAddr;
        endAddr = temp;
    }

    std::vector<SVariable> newWatches;

    // Eliminate any existing watches in the range
    for(std::vector<SVariable>::iterator i = WatchVariables.begin(); i != WatchVariables.end(); i++)
    {
        if((i->Address < startAddr) || (i->Address > endAddr))
            newWatches.push_back(*i);
    }
    WatchVariables.clear();
    WatchVariables = newWatches;
    if(fAccess == eAccess::Clear)
        return;
    for(uint16_t address = startAddr; address <= endAddr; address++)
    {
        SVariable watch({ address, fAccess, false });

        WatchVariables.push_back(watch);
    }
}
void watch()
{
    const char *operationText = strtok(NULL, ", ");
    const char *targetText = strtok(NULL, ", ");
    
    if((strcmp(operationText, "CLEAR") == 0) || (strcmp(operationText, "C") == 0))
        watchClear(targetText);
    else if((strcmp(operationText, "LIST") == 0) || (strcmp(operationText, "L") == 0))
        watchList();
    else if(strcmp(operationText, "RANGE") == 0)
    {
        operationText = targetText;
        targetText = strtok(NULL, ", ");

        if((strcmp(operationText, "READ") == 0) || (strcmp(operationText, "R") == 0))
            watchRange(eAccess::Read, targetText);
        else if((strcmp(operationText, "WRITE") == 0) || (strcmp(operationText, "W") == 0))
            watchRange(eAccess::Write, targetText);
        else if((strcmp(operationText, "READWRITE") == 0) || (strcmp(operationText, "RW") == 0))
            watchRange(eAccess::ReadWrite, targetText);
        else if(strcmp(operationText, "CLEAR") == 0)
            watchRange(eAccess::Clear, targetText);
        else
        {
            printf("\r\nInvalid operation %s", operationText);
            return;
        }
    }
    else if((strcmp(operationText, "READ") == 0) || (strcmp(operationText, "R") == 0))
        watchSet(eAccess::Read, targetText);
    else if((strcmp(operationText, "WRITE") == 0) || (strcmp(operationText, "W") == 0))
        watchSet(eAccess::Write, targetText);
    else if((strcmp(operationText, "READWRITE") == 0) || (strcmp(operationText, "RW") == 0))
        watchSet(eAccess::ReadWrite, targetText);
    else
        printf("\r\nInvalid operation %s", operationText);
    CRLF();
}
int setRegister(bool fSetNZ)
{
   const char *valueText = strtok(NULL, "= ");
          char *p;

   uint16_t value = strtoul(valueText, &p, 16 );

    if(*p != 0)
    {
        printf("\r\nError in value %s\r\n", valueText);
        return -1;
    }
    if(fSetNZ)
        setNZ(value);
    return value;
}
void setPC()
{
    int addr = setRegister(false);

    if(addr >= 0)
        PC = (uint16_t)addr;
}
void setSP()
{
    int value = setRegister(false);

    if(value >= 0)
        SP = (uint8_t)value;
}
void setA(bool fSR)
{
    int value = setRegister(fSR);

    if(value >= 0)
        A = (uint8_t)value;
}
void setX(bool fSR)
{
    int value = setRegister(fSR);

    if(value >= 0)
        X = (uint8_t)value;
}
void setY(bool fSR)
{
    int value = setRegister(fSR);

    if(value >= 0)
        Y = (uint8_t)value;
}
void setSR()
{
    const char *bit = strtok(NULL, ", ");

    while (bit != NULL)
    {
        int value = 1;
        int flagBit = 0;

        if(bit[0] == '!')
        {
            flagBit = 1;
            value = 0;
            if(strlen(bit) != 2)
            {
                printf("\r\nError in bit %s\r\n", bit);
                continue;
            }
        }
        else
        {
            if(strlen(bit) != 1)
            {
                printf("\r\nError in bit %s\r\n", bit);
                continue;
            }

        }
        switch(toupper(bit[flagBit]))
        {
            case 'N':
                SR.Bits.N = value;
                break;
            case 'V':
                SR.Bits.V = value;
                break;
            case 'D':
                SR.Bits.D = value;
                break;
            case 'I':
                SR.Bits.I = value;
                break;
            case 'Z':
                SR.Bits.Z = value;
                break;
            case 'C':
                SR.Bits.C = value;
                break;
        }
        bit = strtok(NULL, ", ");
    }    
}
void lookup()
{
    const char *paramText = strtok(NULL, ", ");
          char *p;

    if((paramText == NULL) || (strlen(paramText) == 0))
    {
        printf("\r\nMust have address or label as a parameter\r\n");
        return;
    }
       
    uint16_t value = strtoul(paramText, &p, 16);

    if(*p != 0)
    {   // Assume it is a label
        int address = Lookup(paramText);

        if(address < 0)
        {
            printf("\r\nLabel '%s' not found\r\n", paramText);
            return;           
        }
        printf("\r\nLabel '%s' is at address %04X\r\n", paramText, (uint16_t)address);
    }
    else
    {
        const char *label = Lookup(value);

        if(label == NULL)
        {
            printf("\r\nAddress '%04X' not found\r\n", value);
            return;           
        }
        printf("\r\nLabel '%s' is at address %04X\r\n", label, value);
    }
}
void configList()
{
    const char *enabled = "enabled";
    const char *disabled = "disabled";

    printf("\r\nCurrent Configuration\r\n");
    printf("\r\n%-30s %-15s %s", "Parameter", "Name", "Value");
    printf("\r\n%-30s %-15s %s", "------------------------------", "---------------", "--------------------");
    printf("\r\n%-30s %-15s %s", "CRT Hardware", "CRT", Configuration.UseCRTHW ? enabled : disabled);
    printf("\r\n%-30s %-15s %s", "TTY Hardware", "TTY", Configuration.UseTTYHW ? enabled : disabled);
    printf("\r\n%-30s %-15s %s", "External SD Card", "SD", Configuration.UseExternalSD ? enabled : disabled);
    printf("\r\n%-30s %-15s %s", "Flash storage", "FLASH", Configuration.UseInternalFlash ? enabled : disabled);
    printf("\r\n%-30s %-15s %s", "Simulate Write Protect RAM", "SIMWP", Configuration.SimWP ? enabled : disabled);
    printf("\r\n%-30s %-15s %s", "RAE Overwrite", "RAEOVERWRITE", Configuration.RAEOverwrite ? enabled : disabled);
    printf("\r\n%-30s %-15s %d\r\n", "Default Trace Print", "TRACE", Configuration.DefaultTrace);
    return;
}
void configuration()
{
    const char *paramText = strtok(NULL, ", ");

    if((paramText == NULL) || (strlen(paramText) == 0))
    {
        printf("\r\nMissing parameters\r\n");
        return;
    }
    if(strcmp(paramText, "SAVE") == 0)
    {
        Configuration.Save();
        printf("\r\nConfiguration saved\r\n");
        return;
    }
    if(strcmp(paramText, "LOAD") == 0)
    {
        if(Configuration.Load())
            printf("\r\nConfiguration reloaded\r\n");
        else
            printf("\r\nConfiguration not saved\r\n");
        return;
    }
    if(strcmp(paramText, "ERASE") == 0)
    {
        Configuration.Erase();
        printf("\r\nConfiguration erased and set to default\r\n");
        return;
    }
    if(strcmp(paramText, "LIST") == 0)
    {
        configList();
        return;
    }
    bool crt = (strcmp(paramText, "CRT") == 0);
    bool tty = (strcmp(paramText, "TTY") == 0);
    bool sd = (strcmp(paramText, "SD") == 0);
    bool flash = (strcmp(paramText, "FLASH") == 0);
    bool simwp = (strcmp(paramText, "SIMWP") == 0);
    bool raeOverwrite = (strcmp(paramText, "RAEOVERWRITE") == 0);
    bool defaultTrace = (strcmp(paramText, "TRACE") == 0);
    
    if(defaultTrace)
    {
        const char *traceValueText = strtok(NULL, ", ");
        char *p;

        if((traceValueText == NULL) || (strlen(traceValueText) == 0))
        {
            printf("\r\nMissing trace value\r\n");
            return;
        }
        int traceValue = strtoul(traceValueText, &p, 10);

        if(*p != 0)
        {
            printf("\r\nError in trace value %s\r\n", traceValueText);
            return;
        }
        if(traceValue < 0 || traceValue > TRACELIMIT)
        {
            printf("\r\nTrace value must be between 0 and %d\r\n", TRACELIMIT);
            return;
        }
        Configuration.DefaultTrace = traceValue;
        printf("\r\nDefault trace print set to %d\r\n", traceValue);
        return;
    }
    if(crt || tty || sd || flash || simwp || raeOverwrite)
    {
        const char *instrText = strtok(NULL, ", ");
        
        if((instrText == NULL) || (strlen(instrText) == 0))
        {
            printf("\r\nMissing ON, OFF, or COPY\r\n");
            return;
        }
        if(strcmp(instrText, "ON") == 0)
        {
            if(crt)
            {
                Configuration.UseCRTHW = true;
                printf("\r\nCRT HW enabled\r\n");
                return;
            }
            if(tty)
            {
                Configuration.UseTTYHW = true;
                printf("\r\nTTY HW enabled\r\n");
                return;
            }
            if(sd)
            {
                Configuration.UseExternalSD = true;
                Configuration.UseInternalFlash = false;
                CurrentTape.LoadRoot();
                printf("\r\nSD card enabled\r\n");
                return;
            }
            if(flash)
            {
                Configuration.UseInternalFlash = true;
                Configuration.UseExternalSD = false;
                CurrentTape.LoadRoot();
                printf("\r\nFlash storage enabled\r\n");
                return;
            }
            if (simwp)
            {
                Configuration.SimWP = true;
                Configuration.SetWriteProtectSimulation(true);
                printf("\r\nSimulate Write Protect System RAM\r\n");
                return;
            }
            if (raeOverwrite)
            {
                Configuration.RAEOverwrite = true;
                printf("\r\nRAE Overwrite enabled\r\n");
                return;
            }
        }
        if(strcmp(instrText, "OFF") == 0)
        {
            if(crt)
            {
                Configuration.UseCRTHW = false;
                printf("\r\nCRT HW disabled\r\n");
                return;
            }
            if(tty)
            {
                Configuration.UseTTYHW = false;
                printf("\r\nTTY HW disabled\r\n");
                return;
            }
            if(sd)
            {
                Configuration.UseExternalSD = false;
                printf("\r\nSD card disabled\r\n");
                return;
            }
            if(flash)
            {
                Configuration.UseInternalFlash = false;
                printf("\r\nFlash storage disabled\r\n");
                return;
            }
            if (simwp)
            {
                Configuration.SimWP = false;
                Configuration.SetWriteProtectSimulation(false);
                SystemRAMWriteProtected = false;    // Ensure not protected
                RAMPage1WriteProtected = false;
                RAMPage2WriteProtected = false;
                RAMPage3WriteProtected = false;
                printf("\r\nDon't simulate Write Protect RAM\r\n");
                return;
            }
            if (raeOverwrite)
            {
                Configuration.RAEOverwrite = false;
                printf("\r\nRAE Overwrite disabled\r\n");
                return;
            }
        }
        if(strcmp(instrText, "COPY") == 0)
        {
            if(sd)
            {
                // TODO : Copy from flash storage to SD card
                return;
            }
        }
        printf("\r\nInvalid instruction %s\r\n", instrText);
        return;
    }
    printf("\r\nInvalid parameter %s\r\n", paramText);  
}

void CommandLine(bool fColdStart)
{
    std::string input_line;
    bool skipPrintRegisters = false;
    bool autoExecuteStep = false;
    bool autoExecuteRun = false;

    DebuggerActive = true;
    DebuggerOn = DebuggerActive;
    StepOverBreakpoint = -1;
    traceDepth = Configuration.DefaultTrace;

    if(fColdStart)
    {
        getchar();      // Get character from USB to confirm port is open

        debugLOAD();
    }
    
    printf("\r\nCommand Line Ready\r\n");
    while(true)
    {
        if(skipPrintRegisters)
            skipPrintRegisters = false;
        else
            printRegisters();

        while ((autoExecuteRun || autoExecuteStep))
        {
            if(autoExecuteRun)
            {
                autoExecuteRun = false;
                autoExecuteStep = run();
                skipPrintRegisters = true;
            }
            if(autoExecuteStep)
            {
                autoExecuteStep = false;
                autoExecuteRun = step(true);
                skipPrintRegisters = true;
            }
        }

        prompt();
        get_text_until_cr(&input_line);
        std::transform(input_line.begin(), input_line.end(), input_line.begin(), ::toupper);

        char text[input_line.size() + 1];

        strcpy(text, input_line.c_str());
        
        char *token = strtok(text, ", ");

        if(strlen(token) == 0)
        {
            skipPrintRegisters = true;
            continue;
        }
        else if((strcmp(token, "REG") == 0) || (strcmp(token, "?") == 0))
        {
            CRLF();
            continue;
        }
        else if(strcmp(token, "CLS") == 0)
        {
            printf("\x0C");
            continue;
        }
        else if((strcmp(token, "DEBUG") == 0) || (strcmp(token, "DB") == 0))
            debug();
        else if(strcmp(token, "STACK") == 0)
            stack();
        else if(strcmp(token, "DUMP") == 0)
            dump();
        else if(((strcmp(token, "EXEC") == 0) || (strcmp(token, "EXECUTE") == 0)))
        {
            DebuggerActive = false;
            break;
        }
        else if((strcmp(token, "RUN") == 0) || (strcmp(token, "R") == 0))
            autoExecuteStep = run();
        else if(strcmp(token, "STEP") == 0)
            autoExecuteRun = step(false);
        else if((strcmp(token, "BREAKPOINT") == 0) || (strcmp(token, "BP") == 0))
            bp();
        else if(strcmp(token, "TRACE") == 0)
            Trace();
        else if(strcmp(token, "WATCH") == 0)
            watch();
        else if(strcmp(token, "PC") == 0)
            setPC();
        else if(strcmp(token, "SP") == 0)
            setSP();
        else if(strcmp(token, "A") == 0)
            setA(false);
        else if(strcmp(token, "ASR") == 0)
            setA(true);
        else if(strcmp(token, "X") == 0)
            setX(false);
        else if(strcmp(token, "XSR") == 0)
            setX(true);
        else if(strcmp(token, "Y") == 0)
            setY(false);
        else if(strcmp(token, "YSR") == 0)
            setY(true);
        else if(strcmp(token, "SR") == 0)
            setSR();
        else if(strcmp(token, "RESET") == 0)
            reset();
        else if((strcmp(token, "LOOKUP") == 0) || (strcmp(token, "LU") == 0))
            lookup();
        else if(strcmp(token, "CONFIG") == 0)
            configuration();
        else if(strcmp(token, "FILE") == 0)
            fileCommand();
        else if(strcmp(token, "FLASH") == 0)
            flashCommand();
        else if(strcmp(token, "SD") == 0)
            sdCommand();
        else if(strcmp(token, "TAPE") == 0)
            tapeCommand();
        else if(strcmp(token, "COPYTO") == 0)
            copyTo();
        else
            printf("\r\nUnknown Command\r\n");
    }
}
