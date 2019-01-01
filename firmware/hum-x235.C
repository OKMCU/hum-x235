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

#define KEY_DEBOUNCE  50
#define COLOR_SPEED   23
#define COLOR_HOLD    1000
//===========================================================
//Variable definition
//===========================================================
uint8_t mode = 0;
uint8_t key_debounce = 0;
uint8_t color_r = 0;
uint8_t color_g = 0;
uint8_t color_b = 0;
uint8_t color_cnt = 0;
uint8_t color_r_tar = 0;
uint8_t color_g_tar = 0;
uint8_t color_b_tar = 0;
uint8_t color_id = 0;
uint16_t color_speed = 0;
uint16_t color_hold = 0;

const uint8_t color_table[7][3] = {
    {255, 0, 0},
    {255, 255, 0},
    {0, 255, 0},
    {0, 255, 255},
    {0, 0, 255},
    {255, 0, 255},
    {255, 255, 255},
};

//===========================================================
//Funtion name：interrupt ISR
//parameters：无
//returned value：无
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
	OSCCON = 0B01110001;   //Bit7    >>>  LFMOD=0 >>> WDT振荡器频率=32KHz
                           //Bit6:4 >>> IRCF[2:0]=111 >>> 内部RC频率=16MHz
                           //Bit0   >>> SCS=1      >>> 系统时钟选择为内部振荡器

    MSCKCON = 0B00010000;  //Bit4 >>> SLVREN=1 >>>低电压复位睡眠期间关闭,唤醒时开启
	INTCON = 0B00000000; //暂禁止所有中断
	CMCON0 = 0B00000111; //关闭比较器，CxIN为数字IO口
	PORTA = 0B11111110;  //RA0控制风扇输出0，其他LED或者KEY输出1
	TRISA = 0B00101110;  //RA0,RA4,RA6,RA7为输出,其他PORTA口为输入
	WPUA = 0B11111111;   //默认启用全部PORTA的内部上拉
	PORTC = 0B00000000;
	TRISC = 0B00000000;  //设置所有PORTC口为输出
	OPTION = 0B00000000; //Bit7,PAPU=0,开启PORTA内部上拉总闸
	                     //Bit6,INTEDG=0,falling edge interrupt on PA2
	                     //Bit5,T0CS=0,Timer 0 clock source is internal instruction cycle clock
	                     //Bit4,T0SE=0,Timer 0 上升沿计数器+1
	                     //Bit3,PSA=0,预分频器给Timer 0使用
	                     //Bit2:0,PS=000,1:2*FOSC/2,FOSC=16MHz,0.25us per tick, 256*0.25us = 64us per timer overflow interrupt
    T0IF = 0;            //清TIMER0中断标志位
	T0IE = 1;            //使能TIMER0中断
    GIE = 1;	         //开启总中断
}
//===========================================================
//Funtion name：main
//parameters：无
//returned value：无
//===========================================================
main()
{
    device_init();

    //mode = 1;
    //color_r_tar = color_table[color_id][0];
    //color_g_tar = color_table[color_id][1]; 
    //color_b_tar = color_table[color_id][2];
    //color_speed = COLOR_SPEED;
    
    while(1)
    {
        //Key scan
        if( IO_KEY )
        {
            key_debounce = 0;
        }
        else
        {
            if( key_debounce < KEY_DEBOUNCE )
            {
                key_debounce++;
                if( key_debounce == KEY_DEBOUNCE )
                {
                    //Key enter
                    mode++;
                    if( mode > 2 )
                        mode = 0;

                    if( mode == 0 )
                    {
                        IO_FAN = 0;
                        color_r = 0;
                        color_g = 0;
                        color_b = 0;
                    }
                    else if( mode == 1 )
                    {
                        IO_FAN = 1;
                        color_r_tar = color_table[color_id][0];
                        color_g_tar = color_table[color_id][1]; 
                        color_b_tar = color_table[color_id][2];
                        color_speed = COLOR_SPEED;
                    }
                    else
                    {
                        color_r = 0;
                        color_g = 0;
                        color_b = 0;
                    }
                }
            }
        }
        
        if( mode == 1 )
        {
            if( color_hold == 0 )
            {
                color_speed++;
                if( color_speed > COLOR_SPEED )
                {
                    color_speed = 0;
                    
                    if     ( color_r < color_r_tar )    {color_r++;}
                    else if( color_r > color_r_tar )    {color_r--;}
                    if     ( color_g < color_g_tar )    {color_g++;}
                    else if( color_g > color_g_tar )    {color_g--;}
                    if     ( color_b < color_b_tar )    {color_b++;}
                    else if( color_b > color_b_tar )    {color_b--;}

                    if( color_r == color_r_tar && color_g == color_g_tar && color_b == color_b_tar )
                    {
                        color_id++;
                        if( color_id > 6 )
                            color_id = 0;
                        color_r_tar = color_table[color_id][0];
                        color_g_tar = color_table[color_id][1]; 
                        color_b_tar = color_table[color_id][2];
                        
                        color_hold = COLOR_HOLD;
                    }
                }
            }
            else
            {
                color_hold--;
            }
        }

        __delay_ms(1);
    }
    
}
//===========================================================
