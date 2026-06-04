#pragma once

#include "PiSym.h"

/*
    Modified from RP2040 utility class

    Copyright (c) 2021 Earle F. Philhower, III <earlephilhower@yahoo.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

void InitTiming();

/*
    CPU cycle counter epoch (24-bit cycle).  For internal use
*/
extern volatile uint64_t _epoch;

/*
    Get the count of CPU clock cycles since power on.

    The 32-bit count will overflow every 4 billion cycles, so consider using ``getCycleCount64`` for
    longer measurements

    @returns CPU clock cycles since power up
*/
// inline uint32_t getCycleCount() 
// {
//     // Get CPU cycle count.  Needs to do magic to extend 24b HW to something longer
//     uint32_t epoch;
//     uint32_t ctr;
//     do {
//         epoch = (uint32_t)_epoch[get_core_num()];
//         ctr = systick_hw->cvr;
//     } while (epoch != (uint32_t)_epoch[get_core_num()]);
//     return epoch + (1 << 24) - ctr; /* CTR counts down from 1<<24-1 */
// }
/*
    Get the count of CPU clock cycles since power on as a 64-bit quantrity

    Returns CPU clock cycles since power up
*/
uint64_t getCycleCount64();
