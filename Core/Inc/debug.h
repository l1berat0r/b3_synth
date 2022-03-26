/*
 * debug.h
 *
 *  Created on: Oct 7, 2021
 *      Author: liberator
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "stm32f4xx_hal.h"

#define DEMCR_TRCENA    0x01000000
#define DEMCR           (*((volatile uint32_t *)0xE000EDFC))
#define DWT_CTRL        (*(volatile uint32_t *)0xe0001000)
#define CYCCNTENA       (1<<0)
#define DWT_CYCCNT      ((volatile uint32_t *)0xE0001004)
#define CPU_CYCLES      *DWT_CYCCNT

void init_debug(void);
void clear_cycles(void);
void record_cycles(void);


#endif /* DEBUG_H_ */
