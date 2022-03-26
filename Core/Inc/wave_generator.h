/*
 * wave_generator.h
 *
 *  Created on: Oct 7, 2021
 *      Author: liberator
 */

#ifndef SRC_WAVE_GENERATOR_H_
#define SRC_WAVE_GENERATOR_H_

#include "stm32f4xx_hal.h"
#include "config.h"
#include "notes.h"
#include "wave_forms.h"
#include "math.h"
#include "debug.h"
#include "lookup_tables.h"

#define ABS(x) (x<0 ? (x*-1):x)

enum WAVE_TYPE {
	WAVE_SINE,
	WAVE_SQUARE,
	WAVE_TRIANGLE,
	WAVE_SAWTOOTH,
	WAVE_PIANO,
	WAVE_TRUMPET,
	WAVE_PERCUSSION
};

typedef enum WAVE_TYPE e_wave_type;

extern int16_t sf_wave[SF_SIZE];
extern float pure_sine[SF_SIZE];

typedef struct {
	int32_t adsr_attack;
	int32_t adsr_decay;
	int32_t adsr_release;
	int32_t adsr_sustain;

	int8_t final_filter_cutoff_idx;
	int8_t final_filter_ripple_idx;

	int8_t filter_cutoff_idx;
	int8_t filter_ripple_idx;

	int8_t filter_cutoff_decay_idx;
	int8_t filter_cutoff_sustain_idx;
	int8_t filter_cutoff_release_idx;

	int8_t filter_cutoff_attack_dir;
	int8_t filter_cutoff_decay_dir;
	int8_t filter_cutoff_release_dir;

	int32_t filter_attack;
	int32_t filter_decay;
	int32_t filter_release;

	int16_t distortion;
	int16_t tremolo;
	int16_t fmod;
	int16_t arpeggio;

	int16_t master_volume;

	uint8_t dbars[8];
	uint8_t dbars_harmonics[8];
	float square_fill[8];
	e_wave_type waveform[8];
}s_wg_settings;

extern s_wg_settings wg_settings;

typedef struct {
	int32_t freq;
	int8_t final_filter_cutoff;

}s_wg_dynamic_values;

extern s_wg_dynamic_values wg_dynamic_values;

float NOTES[NOTES_IN_SCALE];

#define FILTER_LENGTH 43
int64_t filter_coef[MAX_FILTER_LENGTH];
int64_t filter_history[MAX_FILTER_LENGTH];
int32_t sample_history[MAX_FILTER_LENGTH];
uint32_t filter_phase;

//extern uint8_t draw_bars_harmonics[DRAW_BARS];

void generate_sine(void);
void generate_sf(int16_t* wave_ptr, s_wg_settings settings);
void generate_taps(int64_t* taps, int16_t cutoff, int8_t nr_taps);
int16_t get_sample(uint32_t angle);
int32_t apply_envelope(int32_t v, s_note* note, uint8_t* note_off);
void apply_filter_envelope(s_note* note);
int16_t OLD_apply_envelope(int16_t v, int* p, uint8_t* end);

int16_t write_samples(int16_t* buffer, int16_t size);

float sinc(float angle);

#endif /* SRC_WAVE_GENERATOR_H_ */
