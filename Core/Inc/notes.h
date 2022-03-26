/*
 * notes.h
 *
 *  Created on: Oct 7, 2021
 *      Author: liberator
 */

#ifndef INC_NOTES_H_
#define INC_NOTES_H_

#include "stm32f4xx_hal.h"
#include "config.h"
#include "math.h"


#define BASE_C 16.35f
#define NOTES_IN_SCALE 97

extern float NOTES[NOTES_IN_SCALE];

enum Temperament {
	EQUAL = 0,
	HAMMOND = 1,
	JUST = 2
};

enum EnvelopeStage {
	ATTACK = 0,
	DECAY = 1,
	SUSTAIN = 2,
	RELEASE = 3
};

typedef enum Temperament e_temperament;
typedef enum EnvelopeStage e_envelope_stage;

typedef struct {
	char note;
	uint32_t freq;
	uint64_t phase;
	uint64_t internal_osc_phase;
	uint16_t volume;
	uint16_t rel_volume;
	e_envelope_stage envelope_stage;
	e_envelope_stage filter_envelope_stage;
	int32_t envelope_phase;
	int32_t filter_envelope_phase;

	int8_t fltr_cutoff;
	int8_t fltr_ripple;
	int8_t fltr_cutoff_decay;
	int8_t fltr_cutoff_sustain;
	int8_t fltr_cutoff_release;

	int64_t iir_x[IIR_FILTER_ORDER + 1];
	int64_t iir_y[IIR_FILTER_ORDER + 1];
}s_note;

extern s_note* PLAYED_NOTES[MAX_NOTES+1];

void init_notes(void);
void generate_scale(float* scale, e_temperament temper);
void play_note(uint8_t note, uint16_t volume);
void release_note(uint8_t note);
void remove_note(uint8_t note);
void update_filter_params();
void reset_filter_data(void);


#endif /* INC_NOTES_H_ */
