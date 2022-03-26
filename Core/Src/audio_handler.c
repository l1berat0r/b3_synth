/*
 * audio_handler.c
 *
 *  Created on: 10 paź 2021
 *      Author: liberator
 */

#include "audio_handler.h"


void synth_error_handler(void) {
	BSP_LED_On(LED5);
	//while(1){}
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void) {
	BSP_LED_Off(LED3);
	write_samples(SOUND_BUFFER + (BUFFER_SIZE / 2), BUFFER_SIZE / 2);
	BSP_LED_On(LED3);
}


void BSP_AUDIO_OUT_HalfTransfer_CallBack(void) {
	BSP_LED_Off(LED4);
	write_samples(SOUND_BUFFER, BUFFER_SIZE / 2);
	BSP_LED_On(LED4);

}

void BSP_AUDIO_OUT_Error_CallBack(void) {
	BSP_LED_On(LED5);
	while(1){}
}
