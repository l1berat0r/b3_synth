/*
 * synth.c
 *
 *  Created on: 1 paź 2021
 *      Author: liberator
 */

#include "synth.h"

e_application_state application_state;
USBH_HandleTypeDef hUSB_Host;

s_synth_control synth_control = {1,1};
s_wg_settings wg_settings;

float WF_PIANO[SF_SIZE];

void init_synth(void) {
	wg_settings.adsr_attack = 5;
	wg_settings.adsr_decay = 5;
	wg_settings.adsr_release = 5;
	wg_settings.adsr_sustain = 127;

	wg_settings.arpeggio = 0;
	wg_settings.distortion = 0;
	wg_settings.filter_cutoff_idx = 0;
	wg_settings.filter_ripple_idx = 0;

	wg_settings.filter_attack = 1;
	wg_settings.filter_decay = 1;
	wg_settings.filter_release = 1;

	wg_settings.filter_cutoff_decay_idx = 0;
	wg_settings.filter_cutoff_sustain_idx = 0;
	wg_settings.filter_cutoff_release_idx = 0;

	wg_settings.fmod = 0;
	wg_settings.master_volume = 256;

	wg_settings.dbars[0] = 8;
	wg_settings.dbars[1] = 0;
	wg_settings.dbars[2] = 0;
	wg_settings.dbars[3] = 0;
	wg_settings.dbars[4] = 0;
	wg_settings.dbars[5] = 0;
	wg_settings.dbars[6] = 0;
	wg_settings.dbars[7] = 0;

	wg_settings.dbars_harmonics[0] = 0;
	wg_settings.dbars_harmonics[1] = 19;
	wg_settings.dbars_harmonics[2] = 12;
	wg_settings.dbars_harmonics[3] = 24;
	wg_settings.dbars_harmonics[4] = 31;
	wg_settings.dbars_harmonics[5] = 36;
	wg_settings.dbars_harmonics[6] = 40;
	wg_settings.dbars_harmonics[7] = 43;

	wg_settings.square_fill[0] = 0.5;
	wg_settings.square_fill[1] = 0.5;
	wg_settings.square_fill[2] = 0.5;
	wg_settings.square_fill[3] = 0.5;
	wg_settings.square_fill[4] = 0.5;
	wg_settings.square_fill[5] = 0.5;
	wg_settings.square_fill[6] = 0.5;
	wg_settings.square_fill[7] = 0.5;

	wg_dynamic_values.freq = 0;

	init_notes();
	generate_sine();
	generate_scale(NOTES, EQUAL);
	generate_sf(sf_wave, wg_settings);
	init_lfo();

	BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, 80, SAMPLE_FREQ);
}
void synth_handle(void) {
	switch(application_state) {
		case SYNTH_INIT:
			BSP_AUDIO_OUT_Play(SOUND_BUFFER, BUFFER_SIZE * 2);
			USBH_MIDI_Receive(&hUSB_Host, (uint8_t*)MIDI_RX_BUFFER, MIDI_RX_BUFF_SIZE * sizeof(s_midi_message));
			application_state = SYNTH_MIDI;
			break;
		case SYNTH_MIDI:
			synh_midi_state_handle();
			break;
		case SYNTH_IDLE:
			break;
		default:
			break;
	}
}

void synh_midi_state_handle(void) {
	if(synth_control.regenerate_sf) {
		BSP_LED_On(LED5);
		synth_control.regenerate_sf = 0;
		generate_sf(sf_wave, wg_settings);
		BSP_LED_Off(LED5);
	}

	if(synth_control.regenerate_taps) {
		BSP_LED_On(LED5);
		synth_control.regenerate_taps = 0;
		//generate_taps(filter_coef, wg_settings.filter_cutoff, wg_settings.filter_taps);
		BSP_LED_Off(LED5);
	}
}

void midi_handle(void) {

	for(s_midi_message* msg=MIDI_RX_BUFFER;msg->ent0 != NULL;++msg) {
		handle_midi_settings_change(msg, &wg_settings, &synth_control);
		if(msg->ent0 == 9) {
			if(msg->ent1 == 145) {
				play_note(msg->ent2, msg->ent3);
			}
		}
		if(msg->ent0 == 8) {
			release_note(msg->ent2);
		}

		(*(uint32_t*)msg) = 0;
	}
}

void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost) {
	BSP_LED_On(LED5);
	midi_handle();
	USBH_MIDI_Receive(&hUSB_Host, (uint8_t*)MIDI_RX_BUFFER, MIDI_RX_BUFF_SIZE * sizeof(s_midi_message)); // start a new reception
	BSP_LED_Off(LED5);
}
