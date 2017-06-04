#include "FPU.h"

extern float cossinBuf[256];
/*
 * fft.c
 *
 *  Created on: 2017Äê2ÔÂ12ÈÕ
 *      Author: tom
 */
void fft256(float magBuf[129],float inBuf[]){
	float outBuf[256];
	RFFT_F32_STRUCT rfft;
	rfft.InBuf = inBuf;
	rfft.OutBuf = outBuf;
	rfft.CosSinBuf = cossinBuf;
	rfft.MagBuf = magBuf;
	rfft.FFTSize = 256;
	rfft.FFTStages = 8;
	RFFT_f32u(&rfft);
	RFFT_f32s_mag(&rfft);
}

