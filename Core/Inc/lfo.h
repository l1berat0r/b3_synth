/*
 * lfo.h
 *
 *  Created on: Nov 27, 2021
 *      Author: liberator
 */

#ifndef INC_LFO_H_
#define INC_LFO_H_

#include "config.h"
#include "stm32f4xx_hal.h"
#include "../../Drivers/BSP/stm32f4_discovery.h"
#include "wave_generator.h"
#include "math.h"

#define LFO_RESOLUTION 128
#define NR_LFOS 2

int16_t lfo_sin[LFO_RESOLUTION];

void init_lfo();
void handle_lfo(uint8_t lfo);

#endif /* INC_LFO_H_ */
