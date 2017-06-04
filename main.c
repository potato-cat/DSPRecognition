#include"DSP28X_Project.h"
#include"tools.h"

interrupt void mcbspReceive();
void writeControl();

/*����һ��������¼ȡ���������ݣ�����Ƶ��8kHz,ʱ��1s������Ϊ8000*/
#pragma DATA_SECTION(recData,"recData");
float recData[8000];
#pragma DATA_SECTION(validData,"validData");
float validData[20][256];
volatile VOICE voice;
volatile FLAG flag = {
		.SamCom = 0,
		.RecCom = 0,
		.reserve = 0
};
volatile MFCC mfccresult;
#pragma DATA_SECTION(result,"result");
float result[20][12];
float dtw1[20][20];
float dist[20][20];
short k,j;
float distance[9];
extern MFCC mfcc1;
extern MFCC mfcc2;
extern MFCC mfcc3;
extern MFCC mfcc4;
extern MFCC mfcc5;
extern MFCC mfcc6;
extern MFCC mfcc7;
extern MFCC mfcc8;
extern MFCC mfcc9;
char ReceivedChar;
//extern float dtw(MFCC data,MFCC moban,float dtw[20][20],float dist[20][20]);

/*
 * main.c
 * ���������
 * ¼��ʶ��ȫ������
 * */
void main(void) {
	/*��һ����ϵͳ��ʼ��
	 * ��ʼ������pll������ʱ�ӡ����Ź�
	 * ����Ĭ�Ϲرգ��ڴ������в�ʹ�ܿ��Ź�
	 * pll���໷����Ϊ150MHzϵͳʱ��
	 * ����ʱ��ȫ��ʹ�ܣ�
	 * */
	InitSysCtrl();
	/*�ڶ��������ж�
	 * TI�����й��жϲ�����ڵڶ����������ｫ�����ڵڶ���
	 * */
	DINT;
	/*��������������ʹ�õ�����GPIO
	 * ��������ʹ����McBSP��I2C
	 * */
	InitMcbspaGpio();
	InitI2CGpio();
	InitXintf16Gpio();
	InitSciGpio();

	/*���Ĳ�����ʼ��PIE����
	 * ��ֹ����PIE���ж�
	 * �������PIE�жϱ�־λ
	 * ��PieCtrl.c�ж������������
	 * */
	InitPieCtrl();
	/*���岽����ʼ��PIE������
	 * ���жϺ����ĳ�ʼ��ָ��ָ��洢���е�PIE������
	 * ��PieVect.c�ж������������
	 * */
	InitPieVectTable();

	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
//	InitFlash()������ram��ִ��
	InitFlash();

	/*����������ֹ����CPU���ж����ȫ���жϱ�־λ
	 * */
	IER = 0x0000;
	IFR = 0x0000;
	/*���߲���д���жϺ���*/
	EALLOW;
	PieVectTable.MRINTA = &mcbspReceive;
	EDIS;
	/*�ڰ˲�����ʼ��I2C,McBSP*/
	InitI2C();
	InitMcbspa();
	InitXintf();
	scic_fifo_init();
	scic_echoback_init();
	/*�ھŲ���д��I2C������*/
	writeControl();
	/*��ʮ����ʹ���ж�*/
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
	PieCtrlRegs.PIEIER6.bit.INTx5=1;     // Enable PIE Group 6, INT 5
	IER |= M_INT6;                            // Enable CPU INT6

	EINT;   // Enable Global interrupt INTM
	ERTM;	// Enable Global realtime interrupt DBGM

	while(1){
		while(ScicRegs.SCIFFRX.bit.RXFFST !=1) { }
		ReceivedChar = ScicRegs.SCIRXBUF.all;
		PieCtrlRegs.PIEACK.all = 0x0020;
		EINT;
		ERTM;
		while(1){
			if(flag.SamCom){
				DINT;
				nomalization(recData);
				PreEmphasise(recData);
				vad(recData, voice);
				mfccresult.len = (voice.end-voice.start)/128;
				/**��֡**/
				for(k = 0;k < mfccresult.len; k++){
					for(j = 0; j < 256; j++){
						validData[k][j] = recData[voice.start+k*128+j];
					}
					mfcc(result[k], validData[k]);
				}
				for(k = 2; k < mfccresult.len-2; k++){
					for(j = 0; j < 12; j++){
						mfccresult.fullresult[k-2][j] = (-2*result[k-2][j] - result[k-1][j] + result[k+1][j] + 2*result[k+2][j])/3;
					}
					for(j = 12; j < 24; j++){
						mfccresult.fullresult[k-2][j] = result[k-2][j-12];
					}
				}
				mfccresult.len = mfccresult.len-4;
				if(ReceivedChar == 49){
					mfcc1 = mfccresult;
					scic_msg("OK");
				}
				else if(ReceivedChar == 50){
					mfcc2 = mfccresult;
					scic_msg("OK");
				}
				else if(ReceivedChar == 51){
					mfcc3 = mfccresult;
					scic_msg("OK");
				}

				else if(ReceivedChar == 52){
					mfcc4 = mfccresult;
					scic_msg("OK");
				}

				else if(ReceivedChar == 53){
					mfcc5 = mfccresult;
					scic_msg("OK");
				}

				else if(ReceivedChar == 54){
					mfcc6 = mfccresult;
					scic_msg("OK");
				}

				else if(ReceivedChar == 55){
					mfcc7 = mfccresult;
					scic_msg("OK");
				}

				else if(ReceivedChar == 56){
					mfcc8 = mfccresult;
					scic_msg("OK");
				}

				else if(ReceivedChar == 57){
					mfcc9 = mfccresult;
					scic_msg("OK");
				}

				else{
					dtw(mfccresult.fullresult,mfcc1.fullresult,mfccresult.len,mfcc1.len,dtw1,dist);
					distance[0] = dist[mfccresult.len-1][mfcc1.len-1];
					dtw(mfccresult.fullresult,mfcc2.fullresult,mfccresult.len,mfcc2.len,dtw1,dist);
					distance[1] = dist[mfccresult.len-1][mfcc2.len-1];
					dtw(mfccresult.fullresult,mfcc3.fullresult,mfccresult.len,mfcc3.len,dtw1,dist);
					distance[2] = dist[mfccresult.len-1][mfcc3.len-1];
					dtw(mfccresult.fullresult,mfcc4.fullresult,mfccresult.len,mfcc4.len,dtw1,dist);
					distance[3] = dist[mfccresult.len-1][mfcc4.len-1];
					dtw(mfccresult.fullresult,mfcc5.fullresult,mfccresult.len,mfcc5.len,dtw1,dist);
					distance[4] = dist[mfccresult.len-1][mfcc5.len-1];
					dtw(mfccresult.fullresult,mfcc6.fullresult,mfccresult.len,mfcc6.len,dtw1,dist);
					distance[5] = dist[mfccresult.len-1][mfcc6.len-1];
					dtw(mfccresult.fullresult,mfcc7.fullresult,mfccresult.len,mfcc7.len,dtw1,dist);
					distance[6] = dist[mfccresult.len-1][mfcc7.len-1];
					dtw(mfccresult.fullresult,mfcc8.fullresult,mfccresult.len,mfcc8.len,dtw1,dist);
					distance[7] = dist[mfccresult.len-1][mfcc8.len-1];
					dtw(mfccresult.fullresult,mfcc9.fullresult,mfccresult.len,mfcc9.len,dtw1,dist);
					distance[8] = dist[mfccresult.len-1][mfcc9.len-1];
					float min = distance[0];
					char index = 0;
					for(k = 1;k<9;k++){
						if(min > distance[k]){
							min = distance[k];
							index = k;
						}
					}
					if(min<1000)
						scic_xmit(index+49);
					else
						scic_msg("ERROR");
				}
				flag.SamCom = 0;
				flag.RecCom = 1;
				break;
			}
		}
	}
}


void Delay(int time)
{
 int i,j,k=0;
 for(i=0;i<time;i++)
  for(j=0;j<1024;j++)
   k++;
}

void writeControl(){
	AIC23Write(0x00,0x00);
	Delay(100);
	AIC23Write(0x02,0x00);
	Delay(100);
	AIC23Write(0x04,0x7f);
	Delay(100);
	AIC23Write(0x06,0x7f);
	Delay(100);
	AIC23Write(0x08,0x14);
	Delay(100);
	AIC23Write(0x0A,0x00);
	Delay(100);
	AIC23Write(0x0C,0x00);
	Delay(100);
	AIC23Write(0x0E,0x43);
	Delay(100);
	AIC23Write(0x10,0xCD);
	Delay(100);
	AIC23Write(0x12,0x01);
	Delay(100);

}

void scic_fifo_init()
{
    ScicRegs.SCIFFTX.all=0xE040;
    ScicRegs.SCIFFRX.all=0x204f;
    ScicRegs.SCIFFCT.all=0x0;

}

void scic_echoback_init()
{
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function

 	ScicRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
	ScicRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
	ScicRegs.SCICTL2.all =0x0003;
	ScicRegs.SCICTL2.bit.TXINTENA =1;
	ScicRegs.SCICTL2.bit.RXBKINTENA =1;
	#if (CPU_FRQ_150MHZ)
	  ScicRegs.SCIHBAUD    =0x0001;  // 9600 baud @LSPCLK = 37.5MHz.
	  ScicRegs.SCILBAUD    =0x00E7;
	#endif
	#if (CPU_FRQ_100MHZ)
      ScicRegs.SCIHBAUD    =0x0001;  // 9600 baud @LSPCLK = 20MHz.
      ScicRegs.SCILBAUD    =0x0044;
	#endif
	ScicRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
}

void scic_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scic_xmit(msg[i]);
        i++;
    }
}

void scic_xmit(int a)
{
    while (ScicRegs.SCIFFTX.bit.TXFFST != 0) {}
    ScicRegs.SCITXBUF=a;

}

#pragma CODE_SECTION(mcbspReceive, "ramfuncs");
interrupt void mcbspReceive(){
	static int i = 0;
	short temp = McbspaRegs.DRR1.all;
	 recData[i++] = temp;
	if(i>=8000){
		i = 0;
		flag.SamCom = 1;
	}
	else{
		PieCtrlRegs.PIEACK.all = 0x0020;
		PieCtrlRegs.PIEIFR6.bit.INTx5 = 0;
		ERTM;
	}

}
