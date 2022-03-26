/*
 * midi_oxygen61.c
 *
 *  Created on: 16 lis 2021
 *      Author: liberator
 */

#include "midi_oxygen61.h"

e_settings_section settings_section = SECTION_BASIC;
e_settings_state settings_state = STATE_DEFAULT;

TIM_HandleTypeDef htim4;

void handle_midi_settings_change(s_midi_message* msg, s_wg_settings* wg_settings, s_synth_control* synth_control) {
	e_keyboard_sections section = KB_SECTION_NULL;
	uint8_t section_idx=0;

	if(msg->ent0 == 9) {
		if(msg->ent1 == 145) {
			//Key pressed
			section = KB_SECTION_KEYS;
			section_idx = msg->ent2;
		}
		if(msg->ent1 == 153) {
			section = KB_SECTION_BTNS;
			switch(msg->ent2 ) {
				case 36: section_idx = 0; break;
				case 38: section_idx = 1; break;
				case 42: section_idx = 2; break;
				case 46: section_idx = 3; break;
				case 50: section_idx = 4; break;
				case 45: section_idx = 5; break;
				case 51: section_idx = 6; break;
				case 49: section_idx = 7; break;
			}
		}
	}

	if(msg->ent0 == 11) {
		if(msg->ent1 == 177) {
			switch(msg->ent2) {
				case 20: section_idx = 0; section = KB_SECTION_DBARS; break;
				case 21: section_idx = 1; section = KB_SECTION_DBARS; break;
				case 71: section_idx = 2; section = KB_SECTION_DBARS; break;
				case 72: section_idx = 3; section = KB_SECTION_DBARS; break;
				case 25: section_idx = 4; section = KB_SECTION_DBARS; break;
				case 73: section_idx = 5; section = KB_SECTION_DBARS; break;
				case 74: section_idx = 6; section = KB_SECTION_DBARS; break;
				case 70: section_idx = 7; section = KB_SECTION_DBARS; break;
				case 63: section_idx = 8; section = KB_SECTION_DBARS; break;

				case 22: section_idx = 0; section = KB_SECTION_KNOBS; break;
				case 23: section_idx = 1; section = KB_SECTION_KNOBS; break;
				case 61: section_idx = 2; section = KB_SECTION_KNOBS; break;
				case 24: section_idx = 3; section = KB_SECTION_KNOBS; break;
				case 26: section_idx = 4; section = KB_SECTION_KNOBS; break;
				case 27: section_idx = 5; section = KB_SECTION_KNOBS; break;
				case 62: section_idx = 6; section = KB_SECTION_KNOBS; break;
				case 95: section_idx = 7; section = KB_SECTION_KNOBS; break;

				case 96: section_idx = 0; section = KB_SECTION_DBAR_BTNS; break;
				case 97: section_idx = 1; section = KB_SECTION_DBAR_BTNS; break;
				case 98: section_idx = 2; section = KB_SECTION_DBAR_BTNS; break;
				case 99: section_idx = 3; section = KB_SECTION_DBAR_BTNS; break;
				case 100: section_idx = 4; section = KB_SECTION_DBAR_BTNS; break;
				case 101: section_idx = 5; section = KB_SECTION_DBAR_BTNS; break;
				case 102: section_idx = 6; section = KB_SECTION_DBAR_BTNS; break;
				case 103: section_idx = 7; section = KB_SECTION_DBAR_BTNS; break;
				case 104: section_idx = 8; section = KB_SECTION_DBAR_BTNS; break;
			}
		}
	}
	handle_midi_settings_change_decoded(section, section_idx, msg->ent3, wg_settings, synth_control);
}

void handle_midi_settings_change_decoded(e_keyboard_sections section, uint8_t section_idx, uint8_t val, s_wg_settings* wg_settings, s_synth_control* synth_control) {
	switch(section) {
		case KB_SECTION_DBARS:
			if(section_idx<8) {
				wg_settings->dbars[section_idx] = (uint8_t) (val / 16);
				synth_control->regenerate_sf = 1;
			} else {
				wg_settings->master_volume = val * 4;
			}
			break;
		case KB_SECTION_DBAR_BTNS:
			settings_state = STATE_AWAIT_KEY;
			synth_control->dbar_selected = section_idx;
			break;
		case KB_SECTION_KNOBS:
			switch(settings_section) {
				case SECTION_BASIC:
					switch(section_idx) {
						//case 0: wg_settings->filter_cutoff = val * 100; synth_control->regenerate_taps = 1; break;
						case 0: wg_settings->final_filter_cutoff_idx = val; break;
						case 1: wg_settings->final_filter_ripple_idx = val; break;
						case 5: wg_settings->distortion = val * 32; break;
						case 4: wg_settings->arpeggio = val; break;

					}
					break;
				case SECTION_ENVELOPES:
					switch(section_idx) {
						case 0: wg_settings->adsr_attack = (val * 5) + 1; break;
						case 4: wg_settings->adsr_decay = (val * 5) + 1; break;
						case 1: wg_settings->adsr_release = (val * 5) + 1; break;
						case 5: wg_settings->adsr_sustain = val; break;
					}
					break;
				case SECTION_FILTER:
					switch(section_idx) {
						case 0: wg_settings->filter_cutoff_idx = val; update_filter_params(); break;
						case 1: wg_settings->filter_ripple_idx = val; update_filter_params(); break;
						case 2: wg_settings->filter_attack = (val * 10) + 1;  break;
						case 3: wg_settings->filter_release = (val * 10) + 1;  break;
						case 4: wg_settings->filter_cutoff_decay_idx = val;  break;
						case 5: wg_settings->filter_cutoff_sustain_idx = val;  break;
						case 6: wg_settings->filter_cutoff_release_idx = val;  break;
						case 7: wg_settings->filter_decay = (val * 10) + 1;  break;
					}

					wg_settings->filter_cutoff_attack_dir = 0;
					if(wg_settings->filter_cutoff_decay_idx > 0 && wg_settings->filter_cutoff_decay_idx != wg_settings->filter_cutoff_idx) {
						wg_settings->filter_cutoff_attack_dir = (wg_settings->filter_cutoff_idx - wg_settings->filter_cutoff_decay_idx < 0) ? 1 : -1;
					}

					if(wg_settings->filter_cutoff_sustain_idx > 0 && wg_settings->filter_cutoff_decay_idx != wg_settings->filter_cutoff_sustain_idx) {
						wg_settings->filter_cutoff_decay_dir = (wg_settings->filter_cutoff_decay_idx - wg_settings->filter_cutoff_sustain_idx < 0) ? 1 : -1;
					}

					if(wg_settings->filter_cutoff_release_idx > 0 && wg_settings->filter_cutoff_sustain_idx != wg_settings->filter_cutoff_release_idx) {
						wg_settings->filter_cutoff_release_dir = (wg_settings->filter_cutoff_sustain_idx - wg_settings->filter_cutoff_release_idx < 0) ? 1 : -1;
					}

					break;
				case SECTION_LFO1:
					switch(section_idx) {
						case 0: htim4.Init.Period = val; break;
					}
					if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
					  {
					    Error_Handler();
					  }
					break;
				case SECTION_LFO2:
					break;
				case SECTION_WAVEFORMS:
					break;
				case SECTION_SQUARE_FILL:
					wg_settings->square_fill[section_idx] = (float)val / 128.0;
					synth_control->regenerate_sf = 1;
					break;
				case SECTION_FM:
					switch(section_idx) {
						case 0: wg_settings->fmod = val; break;
					}
					break;
			}
			break;
		case KB_SECTION_BTNS:
			settings_section = (e_settings_section)section_idx;
			break;
		case KB_SECTION_KEYS:
			if(settings_state == STATE_AWAIT_KEY) {
				wg_settings->waveform[synth_control->dbar_selected] = section_idx;
				synth_control->regenerate_sf = 1;
				settings_state = STATE_DEFAULT;
			}
			break;
		case KB_SECTION_NULL:
			break;
	}
}
