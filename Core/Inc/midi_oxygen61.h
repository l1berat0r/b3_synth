/*
 * midi_oxygen61.h
 *
 *  Created on: 16 lis 2021
 *      Author: liberator
 */

#ifndef INC_MIDI_OXYGEN61_H_
#define INC_MIDI_OXYGEN61_H_

#include "synth.h"
#include "notes.h";
#include "wave_generator.h"

enum SETTINGS_SECTION {
	SECTION_BASIC,
	SECTION_ENVELOPES,
	SECTION_FILTER,
	SECTION_LFO1,
	SECTION_LFO2,
	SECTION_WAVEFORMS,
	SECTION_SQUARE_FILL,
	SECTION_FM,
};

enum SETTINGS_STATE {
	STATE_DEFAULT,
	STATE_AWAIT_KEY
};

enum KEYBOARD_SECTIONS {
	KB_SECTION_NULL,
	KB_SECTION_DBARS,
	KB_SECTION_DBAR_BTNS,
	KB_SECTION_KNOBS,
	KB_SECTION_BTNS,
	KB_SECTION_KEYS,
};

typedef enum SETTINGS_SECTION e_settings_section;
typedef enum SETTINGS_STATE e_settings_state;
typedef enum KEYBOARD_SECTIONS e_keyboard_sections;

e_settings_section settings_section;
e_settings_state settings_state;

void handle_midi_settings_change(s_midi_message* msg, s_wg_settings* wg_settings, s_synth_control* synth_control);
void handle_midi_settings_change_decoded(e_keyboard_sections section, uint8_t section_idx, uint8_t val, s_wg_settings* wg_settings, s_synth_control* synth_control);

#endif /* INC_MIDI_OXYGEN61_H_ */
