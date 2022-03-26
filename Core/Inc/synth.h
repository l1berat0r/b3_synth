/*
 * synth.h
 *
 *  Created on: 1 paź 2021
 *      Author: liberator
 */

#ifndef INC_SYNTH_H_
#define INC_SYNTH_H_

#include "stm32f4xx_hal.h"
//#include "../../Drivers/BSP/stm32f411e_discovery_audio.h"
#include "../../Drivers/BSP/stm32f4_discovery_audio.h"
#include "usbh_core.h"
#include "usbh_MIDI.h"
#include "config.h"
#include "notes.h"
#include "wave_generator.h"
#include "main.h"
#include "lfo.h"


typedef struct {
	uint8_t ent0;
	uint8_t ent1;
	uint8_t ent2;
	uint8_t ent3;
}s_midi_message;

typedef struct {
	uint8_t regenerate_sf;
	uint8_t regenerate_taps;

	uint8_t dbar_selected;
}s_synth_control;

int16_t SOUND_BUFFER[BUFFER_SIZE];
s_midi_message MIDI_RX_BUFFER[MIDI_RX_BUFF_SIZE];

s_synth_control synth_control;

void init_synth(void);
void synth_handle(void);
void synh_midi_state_handle(void);
void midi_handle(void);

void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost);



#endif /* INC_SYNTH_H_ */
