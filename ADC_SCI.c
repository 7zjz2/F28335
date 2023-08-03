/*
 * main.c
 *LED长亮程序运行，电位器变化，数码管显示，SCI通讯返回
 *  Created on: 2023年7月27日
 *      Author: HP
 */
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
extern void LED_Init(void);
extern void SCI_Init(unsigned int baud);
extern void SCI_Send_Byte(int a);
extern void SCI_Send_String(char *message);
extern void ADC_Init();
extern unsigned int ADC_Read_Value();
char *message;
char ReceivedValue[10]="zhangjinze";
float ADCValue=0;
int i=0;
void main()
{
	InitSysCtrl();
	LED_Init();
	SCI_Init(4800);
	ADC_Init();
	message="\r\nHello,World!\r\n";
	SCI_Send_String(message);
//	while(1)
//	{
		message="\r\nADC send a word : ";
		SCI_Send_String(message);
//		for(i=0;i<10;i++)
//		{
//			while(SciaRegs.SCIFFRX.bit.RXFFST!=1);
//			ReceivedValue[i]=(char)(SciaRegs.SCIRXBUF.bit.RXDT);
//		}
//		ReceivedValue[9]='\0';
		ADCValue=(float)ADC_Read_Value()*3.0/4095;
		i=ADCValue*10;
		ReceivedValue[0]=i/10+0x30;
		ReceivedValue[1]='.';
		ReceivedValue[2]=i%10+0x30;
		ReceivedValue[3]='V';
		ReceivedValue[4]='\0';

		SCI_Send_String(ReceivedValue);
		message="  DSP received a word : ";
		SCI_Send_String(message);
		SCI_Send_String(ReceivedValue);
		GpioDataRegs.GPCTOGGLE.bit.GPIO64=1;
		DELAY_US(5000);
//	}
}
void LED_Init(void)
{EALLOW;SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK=1;GpioCtrlRegs.GPCMUX1.bit.GPIO64=0;GpioCtrlRegs.GPCPUD.bit.GPIO64=0;GpioCtrlRegs.GPCDIR.bit.GPIO64=1;GpioDataRegs.GPCSET.bit.GPIO64=1;	EDIS;}
void SCI_Init(unsigned int baud)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.SCIAENCLK=1;
	InitSciaGpio();
	Uint16 scihbaud=0;Uint16 scilbaud=0;Uint16 scibaud=0;
	scibaud=37500000/(8*baud)-1;scilbaud=scibaud&0x00ff;scihbaud=scibaud>>8;
	SciaRegs.SCICCR.all=0x0007;
	SciaRegs.SCICTL1.all=0x0003;
	SciaRegs.SCICTL2.all=0x0003;
	SciaRegs.SCIFFTX.all=0xE040;
	SciaRegs.SCIFFRX.all=0x204f;
	SciaRegs.SCIFFCT.all=0x0000;
	SciaRegs.SCICTL1.bit.SWRESET=1;
	SciaRegs.SCIHBAUD=scihbaud;
	SciaRegs.SCILBAUD=scilbaud;
	EDIS;
}
void SCI_Send_Byte(int a)
{	while(SciaRegs.SCIFFTX.bit.TXFFST!=0);
	SciaRegs.SCITXBUF=a;
}
void SCI_Send_String(char *message)
{	int i=0;
	while(message[i]!='\0')
	{SCI_Send_Byte(message[i]);	i++;}
}
void ADC_Init()
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK=1;
	SysCtrlRegs.HISPCP.all=3;
	InitAdc();
	AdcRegs.ADCTRL1.bit.ACQ_PS=0x0f;
	AdcRegs.ADCTRL3.bit.ADCCLKPS=1;
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1;
	AdcRegs.ADCCHSELSEQ1.bit.CONV00=0x0;
	AdcRegs.ADCTRL1.bit.CONT_RUN=1;
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1=0x0;
	AdcRegs.ADCTRL2.all=0x2000;
	EDIS;
}
unsigned int ADC_Read_Value()
{
	while(AdcRegs.ADCST.bit.INT_SEQ1==0);
	AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;
	return AdcRegs.ADCRESULT0>>4;
}
