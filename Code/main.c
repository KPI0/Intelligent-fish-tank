#include <REGX52.H>
#include "Delay.h"
#include "LCD1602.h"
#include "Timer0.h"
#include "Timer1.h"
#include "delaytime10us.h"
#include "MotorRun.h"
#include "OneWire.h"
#include "DS18B20.h"
#include <intrins.h>
//HC-SR04超声波模块引脚
sbit TRIG=P1^6;
sbit ECHO=P1^7;
//KEY1、KEY2按键控制引脚
sbit KEY1=P3^1;
sbit KEY2=P3^0;
//LCD1602显示屏数据引脚
sbit LCD_E=P2^7;
sbit LCD_RW=P2^5;
sbit LCD_RS=P2^6;
//按键输出控制ULN2003驱动板引脚
sbit RE1=P2^0;
sbit RE2=P2^1;
//显示字符存储
unsigned char code word3[]="0123456789";
unsigned int Now;
unsigned int High_Time;

/********函数总集*********/
void KEY1_Scan();
void KEY2_Scan();
void LCD_WriteCommand(unsigned char Command);
void LCD_WriteData(unsigned char Data);
void LCD_Init();
void LCD_SetCursor(unsigned char Line,unsigned char Column);
void Timer0_Init();
void Timer1_Init();
void delayms(unsigned char t);
void scan_key();
unsigned int WAVE();
void Timer_delay(unsigned int BS);
void jidianqiInit();
float T,H,S; //实时温度T、实时水位高度H、超声波测量高度S
float D=100; //设置鱼缸高度D,单位cm，实时水位高度H=鱼缸高度D-超声波测量高度S

/************************主函数************************/
void main()
{
	jidianqiInit();
	Timer0Init();
	Timer1Init();
	DS18B20_ConvertT();		//上电先转换一次温度，防止第一次读数据错误
	Delay(100);			//等待转换完成
	LCD_Init();
	LCD_ShowString(1,1,"Temp:");
	LCD_ShowString(1,15,"C");
	LCD_ShowString(2,1,"High:");
	LCD_ShowString(2,15,"cm");
	while(1)
	{
		DS18B20_ConvertT();	//转换温度
		T=DS18B20_ReadT();	//读取温度
		if(T<0)				//如果温度小于0
		{
			LCD_ShowChar(1,7,'-');	//显示负号
			T=-T;			//将温度变为正数
		}
		else				//如果温度大于等于0
		{
			LCD_ShowChar(1,7,'+');	//显示正号
		}
		LCD_ShowNum(1,8,T,2);		//显示温度整数部分
		LCD_ShowChar(1,10,'.');		//显示小数点
		LCD_ShowNum(1,11,(unsigned long)(T*10000)%10000,3);//显示温度小数部分
	  KEY1_Scan();
		KEY2_Scan();
		scan_key();
		
    High_Time=WAVE();//超声波
		Now=(int)(High_Time*0.0175);
		S=Now;
		LCD_ShowNum(2,7,D-S,3);	//显示高度整数部分
		LCD_ShowChar(2,10,'.');		//显示小数点
		LCD_ShowNum(2,11,(unsigned long)((D-S)*10000)%10000,3);//显示高度小数部分

	}
}

void Timer_delay(unsigned int BS)//T1延时±0.5ms
{
  unsigned int k;
  for(k=0;k<BS;k++)
	{
	  TH1=(65536-100)/256;
	  TL1=(65536-100)%256;
    while(TF1==0);
		TF1=0;
	}
}

/*超声波计算时间*/
unsigned int WAVE()
{
	  unsigned int result;
	  unsigned char p;
    TRIG=0;
		_nop_();//1微秒
		TRIG=1;
	  for(p=0;p<10;p++);//大于10us
		TRIG=0;
		while(ECHO==0);//等高电平来
    Timer0_Init();//开始计时记高电平时间即超声波发射--返回时间
		while(ECHO)
		{
			 if((TH0>0x8c)|| (TH0==0x8c&&TL0>0xa0))break;
		}
		TR0=0;
		//18us=18000ms=4650H  ;100us--18ms有效   超过36ms无效 36ms=8cA0
		 if((TH0<0x46)|| (TH0==0x46&&TL0<=0x50))
		 {
		   result=(TH0<<8)+TL0;
			 return result;
		 }
		else return 0;
}

/*定时器初始化*///T1延时，T0中断
void Timer0_Init()
{
  TMOD=0x11;
	TH0=0;
	TL0=0;
	TR0=1;
	TR1=1;
}

/*按键扫描*/
void scan_key()
{
	P1=0xf0;
	if(P1!=0xff)delayms(10);
}

void delayms(unsigned char t)
{
  unsigned i,j;
	for(i=0;i<t;i++)
	   for(j=0;j<120;j++);
}

/***********继电器初始化******************/
/**
  * @brief  加水和抽水的初始化
  * @param  无
  * @retval 无
  */

void jidianqiInit ()
{
	RE1=0;
	RE2=0;
}

/***********按键一控制换水***************/
/**
  * @brief  按键一按下控制直流电机同时进行加水和抽水
  * @param  无
  * @retval 无
  */

void KEY1_Scan()
{
	if(KEY1 == 0)
	{
		Delay(200);
		while(KEY1 == 0);
		Delay(200);
		RE1=~RE1;
		RE2=~RE2;	
	}
}

/***********按键二控制喂食***************/
/**
  * @brief  按键二按下控制步进电机先顺时针半圈再逆时针半圈
  * @param  无
  * @retval 无
  */

void KEY2_Scan()
{
	if(KEY2 == 0)
	{
		Delay(200);
		if(KEY2 == 0)
		{
			Delay(200);
			MotorRun(4389/2,1,100);//nangle=4096为一圈； drct=0为逆时针转动，drct=1为顺时针转动； speed转速75`400；
			MotorRun(4389/2,0,100);//nangle=4096为一圈； drct=0为逆时针转动，drct=1为顺时针转动； speed转速75`400；
		}
	}
}