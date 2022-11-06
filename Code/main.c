#include <REGX52.H>
#include "Delay.h"
#include "LCD1602.h"
#include "Timer0.h"
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

unsigned char code word3[]="0123456789";
unsigned int Now;
unsigned int High_Time;

/********函数总集*********/
void KEY1_Scan();
void KEY2_Scan();
void lcd_write_com(unsigned char com);
void lcd_write_data(unsigned char dat);
void lcdInit();
void display_LCD(unsigned char hang,unsigned char lie,unsigned dat);
void Timer0_Init();
void delayms(unsigned char t);
void scan_key();
unsigned int WAVE();
void Timer_delay(unsigned int BS);
void jidianqiInit();
float T;

/************************主函数************************/
void main()
{
	unsigned char bai,shi,ge;
	jidianqiInit();
	lcdInit();		
	Timer0Init();
	
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
	
		bai=Now/100%10;
		shi=Now/10%10;
		ge=Now%10;
		
		if(bai!=0)
		{
	
		 delayms(1);
		}
		display_LCD(1,7,word3[shi]);
		delayms(1);
		display_LCD(1,8,word3[ge]);
		delayms(1);
		
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

/*LCD初始化*/
void lcdInit()
{
  lcd_write_com(0x38);//字符为5*7点阵
	lcd_write_com(0x0c);//显示开、光标关、闪烁关
	lcd_write_com(0x06);//写入数据后光标右移一位，显示屏不动
	lcd_write_com(0x01);//清屏
	lcd_write_com(0x80);//设置数据指针起点
}

/*LCD写命令*/
void lcd_write_com(unsigned char com)
{
  LCD_E=0;
	LCD_RS=0;//命令
	LCD_RW=0;//写入
	P0=com;
	delayms(1);
	LCD_E=1;//写入时序
	delayms(1);
	LCD_E=0;

}

/*LCD写数据*/
void lcd_write_data(unsigned char dat)
{
  LCD_E=0;
	LCD_RW=0;//写入
	LCD_RS=1;//数据
	P0=dat;
	delayms(1);
	LCD_E=1;
	delayms(1);
	LCD_E=0;
}
/*LCD显示*/
void display_LCD(unsigned char hang,unsigned char lie,unsigned value) 
{
  if(hang==0)
	{
	  lcd_write_com(0x80+lie);
	}
	if(hang==1)
	{
	  lcd_write_com(0xc0+lie);
	}
	lcd_write_data(value);
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

