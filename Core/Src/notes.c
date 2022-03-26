/*
 * notes.c
 *
 *  Created on: Oct 7, 2021
 *      Author: liberator
 */

#include "notes.h"
#include "wave_generator.h"

void init_notes(void) {
	for(int i=0;i<=MAX_NOTES;++i) {
		PLAYED_NOTES[i] = NULL;
	}
}

void generate_scale(float* scale, e_temperament temper) {
	float note_ratio;

	switch(temper) {
		case JUST:
			break;
		case EQUAL:
			note_ratio = powf(2, 1.0/12.0);
			for(int i=0;i<NOTES_IN_SCALE;++i) {
				*(scale++)=BASE_C * powf(note_ratio, i);
			}
			break;
		case HAMMOND:
		default:
			break;
	}

}

void play_note(uint8_t note, uint16_t volume) {
	s_note** note_ptr;
	int i;
	for(note_ptr=PLAYED_NOTES, i=0;i<MAX_NOTES && (*note_ptr)!=NULL && (*note_ptr)->note != note;++i,++note_ptr);

	if(*note_ptr == NULL) {
		(*note_ptr) = malloc(sizeof(s_note));
		(*note_ptr)->note = note;
		(*note_ptr)->freq = (uint32_t)(NOTES[note] * 16);
		(*note_ptr)->phase = 0;
		(*note_ptr)->internal_osc_phase = 0;
		(*note_ptr)->volume = volume;
		(*note_ptr)->envelope_stage = ATTACK;
		(*note_ptr)->filter_envelope_stage = ATTACK;
		(*note_ptr)->envelope_phase = 0;
		(*note_ptr)->filter_envelope_phase = 0;

		for(int f_idx = 0;f_idx <= IIR_FILTER_ORDER; ++f_idx) {
			(*note_ptr)->iir_x[f_idx] = 0;
			(*note_ptr)->iir_y[f_idx] = 0;
		}

		(*note_ptr)->rel_volume = 0;

		//(*note_ptr)->fltr_cutoff_dir = (wg_settings.filter_cutoff_idx - wg_settings.filter_cutoff_trf_idx) < 0 ? -1 : 1;

		(*note_ptr)->fltr_cutoff = wg_settings.filter_cutoff_idx;
		(*note_ptr)->fltr_ripple = wg_settings.filter_ripple_idx;

		//(*note_ptr)->fltr_cutoff_sustain = wg_settings.filter_cutoff_idx + ((wg_settings.filter_cutoff_idx - wg_settings.filter_cutoff_trf_idx) * wg_settings.filter_sustain / 127);
		//(*note_ptr)->fltr_cutoff_release;

//		(*note_ptr)->fltr_cutoff_decay;
//		(*note_ptr)->fltr_cutoff_sustain;
//		(*note_ptr)->fltr_cutoff_release = wg_settings.filter_cutoff_release_idx;


		//(*note_ptr)->attack_fltr_cycles_per_step = wg_settings.adsr_attack * 100;
		//(*note_ptr)->decay_fltr_cycles_per_step = wg_settings.adsr_attack * 100;

		//wg_settings
	} else if((*note_ptr)->note == note) {
		(*note_ptr)->envelope_stage = ATTACK;
		(*note_ptr)->filter_envelope_stage = ATTACK;
		(*note_ptr)->envelope_phase = 0;
		(*note_ptr)->filter_envelope_phase = 0;
		(*note_ptr)->internal_osc_phase = 0;
		(*note_ptr)->phase = 0;
		(*note_ptr)->rel_volume = 0;
		(*note_ptr)->fltr_cutoff = wg_settings.filter_cutoff_idx;
		(*note_ptr)->fltr_ripple = wg_settings.filter_ripple_idx;

		for(int f_idx = 0;f_idx <= IIR_FILTER_ORDER; ++f_idx) {
			(*note_ptr)->iir_x[f_idx] = 0;
			(*note_ptr)->iir_y[f_idx] = 0;
		}
	}


}

void release_note(uint8_t note) {
	s_note** note_ptr;
	int i;
	for(note_ptr=PLAYED_NOTES, i=0;i<MAX_NOTES && (*note_ptr)!=NULL && (*note_ptr)->note != note;++i,++note_ptr);

	if(note_ptr != NULL) {
		(*note_ptr)->envelope_stage = RELEASE;
		//(*note_ptr)->filter_envelope_stage = RELEASE;
		(*note_ptr)->envelope_phase = 0;
		//(*note_ptr)->filter_envelope_phase = 0;
		//(*note_ptr)->fltr_cutoff = wg_settings.filter_cutoff_sustain_idx;
	}
}

void remove_note(uint8_t note) {
	s_note** note_ptr;
	int i;
	for(note_ptr=PLAYED_NOTES, i=0;i<MAX_NOTES && *note_ptr!=NULL && (*note_ptr)->note != note;++i,++note_ptr);

	if(note_ptr != NULL && i < 10) {
		free(*note_ptr);
		for(;i<MAX_NOTES-1;++i, ++note_ptr) {
			*note_ptr = *(note_ptr+1);
		}
		//*(note_ptr+1) = NULL;
	}
}

void update_filter_params() {
	s_note** note_ptr;
	int i;
	for(note_ptr=PLAYED_NOTES, i=0;i<MAX_NOTES && *note_ptr!=NULL;++i,++note_ptr) {
		(*note_ptr)->fltr_cutoff = wg_settings.filter_cutoff_idx;
		(*note_ptr)->fltr_ripple = wg_settings.filter_ripple_idx;
	}
}

void reset_filter_data(void) {
	s_note** note_ptr;
	int i;
	for(note_ptr=PLAYED_NOTES, i=0;i<MAX_NOTES && *note_ptr!=NULL;++i,++note_ptr) {
		(*note_ptr)->iir_y[0] = 0;
		(*note_ptr)->iir_y[1] = 0;
		(*note_ptr)->iir_y[2] = 0;
		(*note_ptr)->iir_x[0] = 0;
		(*note_ptr)->iir_x[1] = 0;
		(*note_ptr)->iir_x[2] = 0;
	}
}
