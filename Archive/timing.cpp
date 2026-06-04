#include "timing.h"

volatile uint64_t _epoch = 0;

void SysTick_Handler()
{
        _epoch += 1LL << 24;
}

void InitTiming()
{
        systick_hw->csr = 0x0;
        _epoch = 0;
        // Enable SYSTICK exception
        // exception_set_exclusive_handler(SYSTICK_EXCEPTION, _SystickHandler);
        systick_hw->rvr = 0x00FFFFFF;
        systick_hw->csr = 0x5;
}
uint64_t getCycleCount64() 
{
    // return (1LL << 24) - systick_hw->cvr;

    uint64_t epoch;
    uint64_t ctr;
    do {
        epoch = _epoch;
        ctr = systick_hw->cvr;
    } while (epoch != _epoch);
    return epoch + (1LL << 24) - ctr;
}
