/*
 * audio_handler.h
 *
 *  Created on: 10 paź 2021
 *      Author: liberator
 */

#ifndef INC_AUDIO_HANDLER_H_
#define INC_AUDIO_HANDLER_H_

#include "stm32f4xx_hal.h"
//#include "../../Drivers/BSP/stm32f411e_discovery_audio.h"
#include "../../Drivers/BSP/stm32f4_discovery_audio.h"
#include "config.h"
#include "notes.h"
#include "wave_generator.h"
//#include "fmath.h"

float NOTES[NOTES_IN_SCALE];

extern uint16_t BUF_GEN_START;
extern uint16_t BUF_GEN_END;

extern int16_t SOUND_BUFFER[BUFFER_SIZE];
int16_t sf_wave[SF_SIZE];

#endif /* INC_AUDIO_HANDLER_H_ */
