/*
 * wave_generator.c
 *
 *  Created on: Oct 7, 2021
 *      Author: liberator
 */

#include "wave_generator.h"

float pure_sine[SF_SIZE];

int64_t filter_coef[MAX_FILTER_LENGTH];

s_note* PLAYED_NOTES[MAX_NOTES+1];

int32_t wg_tremolo_phase = 0;
int32_t wg_arpeggio_phase = 0;
int32_t wg_arpeggio_note = 0;
int32_t wg_final_filter_phase = 0;

int64_t wg_iir_x[IIR_FILTER_ORDER + 1];
int64_t wg_iir_y[IIR_FILTER_ORDER + 1];

int variable_vol = 256;

void generate_sine(void) {
	for(int i=0;i<SF_SIZE;++i) {
		pure_sine[i] = sinf(2.0f * PI * i / SF_SIZE);
	}
}

void generate_sf(int16_t* wave_ptr, s_wg_settings settings) {
	uint8_t dbars_sum;
	float f_rand;

	for(int i=0;i<SF_SIZE;++i) {
		float val = 0;
		dbars_sum = 1;
		for(int j=0;j<DRAW_BARS;++j) {
			float period_f = (i * NOTES[settings.dbars_harmonics[j]] / (NOTES[0] * SF_SIZE));

			dbars_sum += settings.dbars[j];

			switch(settings.waveform[j] % 6) {
				case WAVE_SINE:
					val += settings.dbars[j] * sinf(2.0f * PI * i * NOTES[settings.dbars_harmonics[j]] / (NOTES[0] * SF_SIZE));
					break;
				case WAVE_SQUARE:
					val += settings.dbars[j] * ( (period_f - (int)period_f) > settings.square_fill[j] ? -1 : 1 );
					break;
				case WAVE_TRIANGLE:
					if(period_f <  settings.square_fill[j]) {
						val += settings.dbars[j] * ((((period_f - (int)period_f) / settings.square_fill[j]) - 0.5) * 2);
					} else {
						val += settings.dbars[j] * ((((1 - (period_f - (int)period_f)) / (1 - settings.square_fill[j])) - 0.5) * 2);
					}
					break;
				case WAVE_SAWTOOTH:
					val += settings.dbars[j] * ((period_f - (int)period_f) - 1) * 2;
					break;
				case WAVE_PIANO:
					val += settings.dbars[j] * WF_PIANO[(int)(i * NOTES[settings.dbars_harmonics[j]] / NOTES[0]) % SF_SIZE];
					break;
				case WAVE_TRUMPET:
					val += settings.dbars[j] * WF_TRUMPET[(int)(i * NOTES[settings.dbars_harmonics[j]] / NOTES[0]) % SF_SIZE];
					break;
				case WAVE_PERCUSSION:
					f_rand = ABS((float)rand() / (float)((1 << 16) - 1));
					val += settings.dbars[j] * sinf(2.0f * PI * i * NOTES[settings.dbars_harmonics[j]] / (NOTES[0] * SF_SIZE)) * f_rand;
					break;
			}
		}
		wave_ptr[i] = val * pow(2,10);
	}
}

float sinc(float angle) {
	return angle == 0 ? 1  : (sinf(angle) / angle);
}

void generate_taps(int64_t* taps, int16_t cutoff, int8_t nr_taps) {
	float sum = 0;
	float _taps[nr_taps];
	for(int i=0; i<nr_taps; ++i) {
		_taps[i] = sinc((float)(2 * cutoff * (i - ((float)(nr_taps - i) / 2.0))) / (float)SAMPLE_FREQ);
		sum += _taps[i];
	}

	for(int i=0; i<nr_taps; ++i) {
		taps[i] = (_taps[i] / sum) * FILTER_PRECISISON;
	}
}

int16_t get_sample(uint32_t angle) {
	//float part = angle - (int)angle;
	//int idx = angle;

	//float v = ((sf_wave[idx % SF_SIZE] * part) + (sf_wave[(idx +1) % SF_SIZE] * (1 - part))) / 2;

	//return v;
	return sf_wave[angle % SF_SIZE];
}

void apply_filter_envelope(s_note* note) {
	switch(note->filter_envelope_stage) {
		case ATTACK:
			if(wg_settings.filter_cutoff_attack_dir != 0 && note->filter_envelope_phase++ % wg_settings.filter_attack == 0) {
				note->fltr_cutoff += wg_settings.filter_cutoff_attack_dir;
			}

			if(note->fltr_cutoff == wg_settings.filter_cutoff_decay_idx) {
				note->filter_envelope_stage=DECAY;
				note->filter_envelope_phase=0;
			}
			break;
		case DECAY:
			if(wg_settings.filter_cutoff_decay_dir != 0 && note->filter_envelope_phase++ % wg_settings.filter_decay == 0) {
				note->fltr_cutoff += wg_settings.filter_cutoff_decay_dir;
			}

			if(note->fltr_cutoff == wg_settings.filter_cutoff_sustain_idx) {
				note->filter_envelope_stage=SUSTAIN;
				note->filter_envelope_phase=0;
			}
			break;

		case SUSTAIN:
			if(note->envelope_stage == RELEASE) {
				note->filter_envelope_stage = RELEASE;
			}
			break;
		case RELEASE:
			if(wg_settings.filter_cutoff_release_idx != note->fltr_cutoff && wg_settings.filter_cutoff_release_dir != 0 && note->filter_envelope_phase++ % wg_settings.filter_release == 0) {
				note->fltr_cutoff += wg_settings.filter_cutoff_release_dir;
			}

			break;
	}
	if(note->fltr_cutoff<0) {
		note->fltr_cutoff = 0;
	}
}

int32_t apply_envelope(int32_t v, s_note* note, uint8_t* note_off) {
	int32_t ret = v;
	switch(note->envelope_stage) {
		case ATTACK:
			if(note->envelope_phase++ % wg_settings.adsr_attack == 0) {
				note->rel_volume++;
			}

			if(note->rel_volume >= 127) {
				note->envelope_stage++;
				note->envelope_phase=0;
			}
			break;
		case DECAY:
			if(note->envelope_phase++ % wg_settings.adsr_decay == 0) {
				note->rel_volume--;
			}

			if(note->rel_volume <= wg_settings.adsr_sustain) {
				note->envelope_stage++;
				note->envelope_phase=0;
			}
			break;
		case SUSTAIN:
			break;
		case RELEASE:
			if(note->envelope_phase++ % wg_settings.adsr_release == 0) {
				note->rel_volume--;
			}

			if(note->rel_volume <= 0) {
				note->envelope_phase=0;
				*note_off = 1;
			}
			break;
	}

	return v * note->rel_volume;
}

int16_t write_samples(int16_t* buffer, int16_t size) {
	s_note** note_ptr;
	int j;
	int i;
	int num_played_notes;

	int start_note = 0;
	int end_note = MAX_NOTES;

	uint8_t rm_notes[MAX_NOTES];
	uint8_t rm_notes_n = 0;

	for(num_played_notes=0;num_played_notes<MAX_NOTES && PLAYED_NOTES[num_played_notes] != NULL;++num_played_notes);

	for(i=0;i<size / 2;++i) {
			int32_t temp_v = 0;

			//Cool arpeggio mode
			if(wg_settings.arpeggio > 0 && num_played_notes > 1) {
				if(wg_arpeggio_phase++ % (wg_settings.arpeggio*256) == 0){
					PLAYED_NOTES[start_note]->phase = 0;
					start_note = (wg_arpeggio_note++) % num_played_notes;
				} else {
					start_note = (wg_arpeggio_note) % num_played_notes;
				}
				end_note = start_note + 1;
			}

			//Loop through every key pressed
			rm_notes_n = 0;
			for(note_ptr=(PLAYED_NOTES + start_note),j=start_note;j<end_note && *note_ptr != NULL;++j, ++note_ptr){
				uint8_t note_off=0;

				uint32_t angle;
				uint32_t i_osc_angle;

				//Calculate angle, get a sample for that angle and adjust the volume, given the envelope settings.
				if(wg_settings.fmod > 0) {
					//Playing a bit with frequency modulation.
					i_osc_angle = (uint32_t)((*note_ptr)->internal_osc_phase++ * (*note_ptr)->freq * 12 * SF_SIZE / (120 * SAMPLE_FREQ)) % SF_SIZE;
					angle = ((((*note_ptr)->phase++ * (*note_ptr)->freq + (pure_sine[i_osc_angle] * wg_settings.fmod * 128)) * SF_SIZE)) / (SAMPLE_FREQ * 10);
				} else {
					angle = ((*note_ptr)->phase++ * (*note_ptr)->freq * SF_SIZE) / (SAMPLE_FREQ * 16);
				}

				int32_t temp_n = get_sample(angle) * (*note_ptr)->volume;
				temp_n =  apply_envelope(temp_n, *note_ptr, &note_off);

				//IIR lowpass filter (Chebyshev) with an envelope for a cutoff frequency.
				//Nice visualization here: http://jaggedplanet.com/iir/iir-explorer.asp
				//Filter coeficients are precalculated and put into lookup table, so we don't waste preciuous
				//cycles for calling trygonometric functions.
				//Filter though is still quite "heavy" on the MCU and playing 8 simultaneus notes will hang it for sure
				//64 bit ints are certainly part of a problem.
				//TODO: 1. Evaluate if the filter sounds ok, if we downscale coefficients and fit all calculations into 32 bit variables.
				//TODO: 2. Make cutoff frequency follow pressed key.
				if(wg_settings.filter_cutoff_idx > 0) {
					apply_filter_envelope(*note_ptr);

					int64_t out=0;
					int32_t lookup_idx = (*note_ptr)->fltr_cutoff * (3 * 128) + (*note_ptr)->fltr_ripple * 3;
					uint8_t filter_idx = (*note_ptr)->phase % (IIR_FILTER_ORDER + 1);

					(*note_ptr)->iir_x[filter_idx] = temp_n / iir_lookup[lookup_idx+2];

					out += (*note_ptr)->iir_x[(filter_idx) % (IIR_FILTER_ORDER + 1)] * (1 * FILTER_PRECISISON);
					out += (*note_ptr)->iir_x[(filter_idx + 2) % (IIR_FILTER_ORDER + 1)] * (2 * FILTER_PRECISISON);
					out += (*note_ptr)->iir_x[(filter_idx + 1) % (IIR_FILTER_ORDER + 1)] * (1 * FILTER_PRECISISON);

					out -= ((*note_ptr)->iir_y[(filter_idx + 2) % (IIR_FILTER_ORDER + 1)] * iir_lookup[lookup_idx+1]);
					out -= ((*note_ptr)->iir_y[(filter_idx + 1) % (IIR_FILTER_ORDER + 1)] * iir_lookup[lookup_idx]);

					out /= (FILTER_PRECISISON);

					(*note_ptr)->iir_y[(filter_idx) % (IIR_FILTER_ORDER + 1)] = out;

					temp_n = (int32_t)(out);
					temp_n /= (128 * 8);
				} else {
					temp_n /= (128 * 128);
				}
				temp_v += temp_n;


				if(note_off) {
					rm_notes[rm_notes_n++] = (*note_ptr)->note;
				}
			}

			//Remove a note when the RELEASE phase of the envelope ended.
			for(j=0;j<rm_notes_n;++j) {
				remove_note(rm_notes[j]);
				--num_played_notes;
			}

			//Additional effects.
			if(wg_settings.distortion > 0 && ABS(temp_v) > wg_settings.distortion && temp_v != 0) {
				temp_v = wg_settings.distortion * (temp_v/ABS(temp_v));
			}

			if(wg_settings.tremolo > 0) {
				temp_v *= (pure_sine[(int)(wg_tremolo_phase++ * wg_settings.tremolo * 200 / (SAMPLE_FREQ)) % SF_SIZE] / 2.0) + 1.0;
			}

			//IIR lowpass filter (Chebyshev) for the mixed output of all sounds.
			if(wg_settings.final_filter_cutoff_idx > 0) {
				temp_v *= 128;
				int64_t out=0;
				int8_t cutoff_idx = wg_settings.final_filter_cutoff_idx + wg_dynamic_values.final_filter_cutoff;

				if(cutoff_idx < 0) {
					cutoff_idx *=-1;
				}

				int32_t lookup_idx = cutoff_idx * (3 * 128) + wg_settings.final_filter_ripple_idx * 3;
				uint8_t filter_idx = wg_final_filter_phase++ % (IIR_FILTER_ORDER + 1);

				wg_iir_x[filter_idx] = temp_v / iir_lookup[lookup_idx+2];

				out += wg_iir_x[(filter_idx) % (IIR_FILTER_ORDER + 1)] * (1 * FILTER_PRECISISON);
				out += wg_iir_x[(filter_idx + 2) % (IIR_FILTER_ORDER + 1)] * (2 * FILTER_PRECISISON);
				out += wg_iir_x[(filter_idx + 1) % (IIR_FILTER_ORDER + 1)] * (1 * FILTER_PRECISISON);

				out -= (wg_iir_y[(filter_idx + 2) % (IIR_FILTER_ORDER + 1)] * iir_lookup[lookup_idx+1]);
				out -= (wg_iir_y[(filter_idx + 1) % (IIR_FILTER_ORDER + 1)] * iir_lookup[lookup_idx]);

				out /= (FILTER_PRECISISON);

				wg_iir_y[(filter_idx) % (IIR_FILTER_ORDER + 1)] = out;

				temp_v = (int32_t)(out / 16);
			}

			buffer[2*i] = (int16_t)(temp_v * wg_settings.master_volume / 256);
			buffer[2*i+1] = buffer[2*i];

		}

	return i*2;
}
