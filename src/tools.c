/*
 * nomalization.c
 *
 *  Created on: 2017年3月30日
 *      Author: Administrator
 */
#include"tools.h"
#include"math.h"

extern float hamming[];
float fft[129];
float melresult[10];
extern float melBank[][];
extern float lifting[];
extern float dctcoef[][];

void nomalization(float rec[]){
	short m;
	float max = 0;
	for( m = 0; m < 8000; m++){
		if(max < fabs(rec[m]))
			max = fabs(rec[m]);
	}
	for(m = 0; m < 8000; m++){
		rec[m] = rec[m]/max;
	}
}

void vad(float rec[],VOICE * voice){
	short i = 0, j = 0;
	float en;
	char x = 0;
	voice->start = 1280;//初始化起始点
	voice->end = 3840;//初始化结束点
	for(i = 0; i < 61; i++){
		en = 0;
		for(j = 0; j < 256; j++){
			en += rec[i*128+j]*rec[i*128+j];
		}
		if(x == 0){
			if(en > 1){//语音段门限
				x = 1;
				voice->start = i*128;
			}
		}else if(x == 1){
			if(en < 1){//静音段门限
				x = 2;
				voice->end = i*128;
			}
		}else{
			x = 0;
			return;
		}

	}
}

void PreEmphasise(float *data)//预加重
{
	short i;
    for(i = 255; i >= 1; i--)
    {
        data[i] = data[i] - 0.9735 * data[i-1];
    }
}

void melFilter(float data[129],float result[10],float melBank[10][129]){
	short m,n;
	for(m = 0;m < 10; m++){
		result[m] = 0;
		for(n = 0; n < 129; n++){
			result[m] += melBank[m][n] * data[n];
		}
		result[m] = log(result[m]);
	}
}

void dctCoeff(float result[12],float dctcoef[12][10], float data[10])//标准DCT变换。
{
	short i,j;
    for(i = 0; i < 12; i++){
    	result[i] = 0;
    	for(j = 0; j < 10; j++){
    		result[i] += dctcoef[i][j]*data[j];
    	}
    }
}

void mfcc(float result[12],float input[256]){
	short i;
	for(i = 0; i < 256; i++){
		input[i] = input[i] * hamming[i];//加窗
	}
	fft256(fft,input);
	for(i = 0;i < 129; i++){
		fft[i] = fft[i]*fft[i];//傅里叶变换
	}
	melFilter(fft, melresult, melBank);//MEL带通滤波器组
	dctCoeff(result,dctcoef,melresult);//离散余弦变换
	for(i = 0; i < 12; i++){
		result[i] = result[i] * lifting[i];
	}

}

void dtw(float data[26][24],float moban[26][24],int datalen,int mobanlen,float dtw[20][20],float dist[20][20]){
//	MFCC data,MFCC moban,float dtw[20][20],float dist[20][20]
	short i,j,k;
	short row,column;
	row = datalen;
	column = mobanlen;
	for(i = 0; i < row; i++){
		for(j = 0; j < column; j++){
			dtw[i][j] = 0;
			for(k = 0; k < 24; k++){
				float temp = data[i][k] - moban[j][k];
				dtw[i][j] += temp*temp;
			}
		}
	}
	dist[0][0] = dtw[0][0];
	for(i = 0; i < row; i++)
		for(j = 0; j < column; j++){
			float d1,d2,d3;
			if(i > 0)
				d1 = dist[i-1][j];
			else
				d1 = 10000;
			if(j > 0)
				d3 = dist[i][j-1];
			else
				d3 = 10000;
			if(i > 0 && j > 0)
				d2 = dist[i-1][j-1];
			else
				d2 = 10000;
			if(d1<d2 && d1<d3)
				dist[i][j] = dtw[i][j] + d1;
			if(d2<d1 && d2<d3)
				dist[i][j] = dtw[i][j] + d2;
			if(d3<d1 && d3<d2)
				dist[i][j] = dtw[i][j] + d3;
		}
}

