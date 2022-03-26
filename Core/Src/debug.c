/*
 * debug.c
 *
 *  Created on: 15 lis 2021
 *      Author: liberator
 */

#include "debug.h"

int cycles_counter[100];
int last_cycles;
int cycles_pos;

void init_debug(void) {
    /* Enable DWT */
    DEMCR |= DEMCR_TRCENA;
    *DWT_CYCCNT = 0;
    /* Enable CPU cycle counter */
    DWT_CTRL |= CYCCNTENA;
    for(int i=0;i<cycles_pos;++i) {
		cycles_counter[i] = 0;
	}
    clear_cycles();
}


void clear_cycles(void) {


	cycles_pos = 0;
	last_cycles = CPU_CYCLES;
}

void record_cycles(void) {
	cycles_counter[cycles_pos++] = CPU_CYCLES - last_cycles;
	last_cycles = CPU_CYCLES;
}
