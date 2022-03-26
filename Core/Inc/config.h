/*
 * config.h
 *
 *  Created on: Oct 7, 2021
 *      Author: liberator
 */

#include "math.h"

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#define PI 3.14159f

#define SAMPLE_FREQ 48000
#define SF_SIZE 2048
#define BUFFER_SIZE 1024

#define MAX_NOTES 20

#define DRAW_BARS 8

#define MIDI_RX_BUFF_SIZE 64

#define MAX_FILTER_LENGTH 127
#define FILTER_PRECISISON (1 << 28)
#define IIR_FILTER_ORDER 2

#endif /* INC_CONFIG_H_ */
