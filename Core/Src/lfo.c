/*
 * lfo.c
 *
 *  Created on: Nov 27, 2021
 *      Author: liberator
 */

#include "lfo.h"

int32_t lfo_osc[NR_LFOS];
s_wg_dynamic_values wg_dynamic_values;

void init_lfo() {
	for(int i=0;i<LFO_RESOLUTION;++i) {
		lfo_sin[i] = (int16_t)(sinf(2.0f * PI * i / LFO_RESOLUTION) * 127.0);
	}

	for(int i=0;i<NR_LFOS;++i) {
		lfo_osc[i] = 0;
	}
}

void handle_lfo(uint8_t lfo) {
	int32_t val = lfo_sin[lfo_osc[lfo]++ % LFO_RESOLUTION];

	wg_dynamic_values.freq = (int32_t)(val * 1024);
	wg_dynamic_values.final_filter_cutoff = val / 4;

	if(val > 0) {
		BSP_LED_On(LED5);
	} else {
		BSP_LED_Off(LED5);
	}
}
