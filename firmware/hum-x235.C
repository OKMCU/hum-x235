//Project: hum-x235.prj
// Device: MS81Fxx02
// Memory: Flash 2KX14b, EEPROM 256X8b, SRAM 128X8b
// Author: 
//Company: 
//Version:
//   Date: 
//===========================================================
//===========================================================
#include	"SYSCFG.h"
#include 	"MS81Fxx02.h"
#include    "stdint.h"

#define IO_LIGHT_R    RA7
#define IO_LIGHT_B    RA6
#define IO_LIGHT_G    RA4
#define IO_FAN        RA0
#define IO_KEY        RA2
//===========================================================
//Variable definition
//===========================================================
uint8_t color_r = 0;
uint8_t color_g = 0;
uint8_t color_b = 10;
uint8_t color_cnt = 0;
//===========================================================
//Funtion name��interrupt ISR
//parameters����
//returned value����
//===========================================================
void interrupt ISR(void)
{
    //if( T0IE && T0IF )
	{
		T0IF = 0;
        color_cnt++;
        IO_LIGHT_R = color_cnt < color_r ? 0 : 1;
        IO_LIGHT_G = color_cnt < color_g ? 0 : 1;
        IO_LIGHT_B = color_cnt < color_b ? 0 : 1;
	}
}

void device_init(void)
{
	OSCCON = 0B01110001;   //Bit7    >>>  LFMOD=0 >>> WDT����Ƶ��=32KHz
                           //Bit6:4 >>> IRCF[2:0]=111 >>> �ڲ�RCƵ��=16MHz
                           //Bit0   >>> SCS=1      >>> ϵͳʱ��ѡ��Ϊ�ڲ�����

    MSCKCON = 0B00010000;  //Bit4 >>> SLVREN=1 >>>�͵�ѹ��λ˯���ڼ�ر�,����ʱ����
	INTCON = 0B00000000; //�ݽ�ֹ�����ж�
	CMCON0 = 0B00000111; //�رձȽ�����CxINΪ����IO��
	PORTA = 0B11111110;  //RA0���Ʒ������0������LED����KEY���1
	TRISA = 0B00101110;  //RA0,RA4,RA6,RA7Ϊ���,����PORTA��Ϊ����
	WPUA = 0B11111111;   //Ĭ������ȫ��PORTA���ڲ�����
	PORTC = 0B00000000;
	TRISC = 0B00000000;  //��������PORTC��Ϊ���
	OPTION = 0B00000000; //Bit7,PAPU=0,����PORTA�ڲ�������բ
	                     //Bit6,INTEDG=0,falling edge interrupt on PA2
	                     //Bit5,T0CS=0,Timer 0 clock source is internal instruction cycle clock
	                     //Bit4,T0SE=0,Timer 0 �����ؼ�����+1
	                     //Bit3,PSA=0,Ԥ��Ƶ����Timer 0ʹ��
	                     //Bit2:0,PS=000,1:2*FOSC/2,FOSC=16MHz,0.25us per tick, 256*0.25us = 64us per timer overflow interrupt
    T0IF = 0;            //��TIMER0�жϱ�־λ
	T0IE = 1;            //ʹ��TIMER0�ж�
    GIE = 1;	         //�������ж�
}
//===========================================================
//Funtion name��main
//parameters����
//returned value����
//===========================================================
main()
{
    device_init();
    while(1);
    
}
//===========================================================