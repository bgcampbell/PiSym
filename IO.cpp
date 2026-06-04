#include "IO.h"

uint8_t get_output_Levels(int fPortB)
{
    uint8_t result = 0;
    int     offset = fPortB ? 8 : 0;

    for(int i = 0; i < 8; i++)
        result |= (gpio_get_out_level(i + offset) << i);

    return result;
}
uint8_t get_inputs(int fPortB)
{
    uint8_t result = 0;
    int     offset = fPortB ? 8 : 0;

    for(int i = 0; i < 8; i++)
        result |= (gpio_get(i + offset) << i);

    return result;
}
uint8_t get_direction(int fPortB)
{
    uint8_t result = 0;
    int     offset = fPortB ? 8 : 0;

    for(int i = 0; i < 8; i++)
        result |= (gpio_get_dir(i + offset) << i);

    return result;
}

uint8_t readORA()
{
    uint8_t ddr = Memory[SY6532BASE + SY6532_DDRA];
    uint8_t result = (get_output_Levels(0) & ddr) | (get_inputs(0) & ~ddr);

    return result;
}

uint8_t readORB()
{
    uint8_t ddr = Memory[SY6532BASE + SY6532_DDRB];
    uint8_t result = (get_output_Levels(1) & ddr) | (get_inputs(1) & ~ddr);

    result = result & Configuration.GetORBMask();
    // printf("\r\nORB = %02X", result);
    // printf("\r\nDDRB = %02X, In = %02X, Out = %02X", get_direction(1), get_inputs(1), get_output_Levels(1));
    return result;
}


void writeDDRA(uint16_t fAddr, uint8_t fValue)
{
    Memory[fAddr] = fValue;
    for(int i = 0; i < 8; i++)
    {
        int value = ((fValue & (1 << i)) != 0);

        gpio_set_dir(i, value);
        if(value == 0)
            gpio_pull_up(i);
    }
}

void writeDDRB(uint16_t fAddr, uint8_t fValue)
{
    // printf("\r\nDDRB = %02X", fValue);
    Memory[fAddr] = fValue;
    for(int i = 0; i < 8; i++)
    {
        int value = ((fValue & (1 << i)) != 0);

        gpio_set_dir(i + 8, value);
        if(value == 0)
        {
            if(i >= 6)
                gpio_pull_down(i + 8);
            else
                gpio_pull_up(i + 8); 
        }
    }
}
void writeORA(uint16_t fAddr, uint8_t fValue)
{
    for(int i = 0; i < 8; i++)
        gpio_put(i, (fValue & (1 << i)) != 0);
}
void writeORB(uint16_t fAddr, uint8_t fValue)
{
    for(int i = 0; i < 8; i++)
        gpio_put(i + 8, (fValue & (1 << i)) != 0);
}
