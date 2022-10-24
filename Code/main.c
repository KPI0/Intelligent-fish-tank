#include <REGX52.H>
#include "Delay.h"
#include "LCD1602.h"
#include "Timer0.h"
#include "delaytime10us.h"
#include "MotorRun.h"
#include "OneWire.h"
#include "DS18B20.h"
#include <intrins.h>

sbit TRIG=P1^6;
sbit ECHO=P1^7;

sbit KEY1=P3^1;
sbit KEY2=P3^0;
//sbit RE1=P2^0;
//sbit RE2=P2^1;

sbit LCD_E=P2^7;
sbit LCD_RW=P2^5;
sbit LCD_RS=P2^6;

sbit RE1=P2^0;
sbit RE2=P2^1;




unsigned char code word3[]="0123456789";

unsigned int Set=40;
unsigned int Now;
unsigned int count_T0=0,count_1ms,miao=0;
unsigned int High_Time;
unsigned char flag;
//********函数总集*********
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
void display_string();
void Timer_delay(unsigned int BS);
void jidianqiInit();
float T;

//************************主函数************************
void main()
{

		unsigned char bai,shi,ge;
	  jidianqiInit();
	
	lcdInit();
	display_string();
		
	LCD_Init();
	Timer0Init();
	
	DS18B20_ConvertT();		//上电先转换一次温度，防止第一次读数据错误
	Delay(100);			//等待转换完成
	LCD_Init();
	
	LCD_ShowString(1,1,"Temp:");
	LCD_ShowString(1,15,"C");
	LCD_ShowString(2,1,"High:");
	LCD_ShowString(2,15,"cm");
	//LCD_ShowString(2,1,"High:");
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
//*显示开始的字母*/
void display_string()
{
	unsigned m,n;
//  for(m=0;m<4;m++)
//	{
//    display_LCD(0,m,word1[m]);
//		delayms(1);
//	}	
    
//	for(n=0;n<16;n++)
//	{
//    display_LCD(1,n,word2[n]);
//		delayms(1);
//	}		
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
  lcd_write_com(0x38);//×Ö·ûÎª5*7µãÕó
	lcd_write_com(0x0c); //ÏÔÊ¾¿ª ¹â±ê¹Ø ÉÁË¸¹Ø
	lcd_write_com(0x06);//Ð´ÈëÊý¾Ýºó¹â±êÓÒÒÆÒ»Î» ÏÔÊ¾ÆÁ²»¶¯
	lcd_write_com(0x01);//ÇåÆÁ
	lcd_write_com(0x80);//ÉèÖÃÊý¾ÝÖ¸ÕëÆðµã
}

/**¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ªLCDÐ´ÃüÁî¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª*/
void lcd_write_com(unsigned char com)
{
  LCD_E=0;
	LCD_RS=0;//ÃüÁî
	LCD_RW=0;//Ð´Èë
	P0=com;
	delayms(1);
	LCD_E=1;//Ð´ÈëÊ±Ðò
	delayms(1);
	LCD_E=0;

}

/*¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ªLCDÐ´Êý¾Ý¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª*/
void lcd_write_data(unsigned char dat)
{
  LCD_E=0;
	LCD_RW=0;//Ð´Èë
	LCD_RS=1;//Êý¾Ý
	P0=dat;
	delayms(1);
	LCD_E=1;
	delayms(1);
	LCD_E=0;
}
//*¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ªÒº¾§ÆÁÏÔÊ¾¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª*/
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


/*¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¶¨Ê±Æ÷³õÊ¼»¯¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª*///T1ÑÓÊ±£¬T0ÖÐ¶Ï
void Timer0_Init()
{
	//count_T0=0;
	//miao=0;
  TMOD=0x11;
	TH0=0;
	TL0=0;
	//ET0=1;
	TR0=1;TR1=1;
	//EA=1;
}
void Timer0() interrupt 1
{
	TH0=0;
	TL0=0;

}

//*¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª°´¼üÉ¨Ãè¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª*/
void scan_key()
{

	P1=0xf0;
	if(P1!=0xff)delayms(10);
	//key_up=0;
//	if(P1!=0xff)
//	{
//	  switch(P1&0xff)
//		{
//		    case 0xfe :Set=Set+10;break;//key_up  p1.0
//				case 0xfd :Set=Set-10;break;//key_dn  p1.1
//		}
//		while(P1!=0xff);
//	}
}
//*¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª*/
void delayms(unsigned char t)
{
  unsigned i,j;
	for(i=0;i<t;i++)
	   for(j=0;j<120;j++);
}


//***********继电器初始化******************
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

//***********按键一控制换水***************
/**
  * @brief  按键一按下控制直流电机同时进行加水和抽水
  * @param  无
  * @retval 无
  */

//sbit KEY1=P3^1;
//sbit KEY2=P3^0;
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
void KEY2_Scan()
{
	if(KEY2 == 0)
	{
		Delay10us();
		if(KEY2 == 0)
		{
			Delay10us();
			MotorRun(4389/2,1,100);					//	nangle=4096ÎªÒ»È¦£»	drct=0ÎªÄæÊ±Õë×ª¶¯£¬drct=1ÎªË³Ê±Õë×ª¶¯£»  speed×ªËÙ75`400£»
			MotorRun(4389/2,0,100);					//	nangle=4096ÎªÒ»È¦£»	drct=0ÎªÄæÊ±Õë×ª¶¯£¬drct=1ÎªË³Ê±Õë×ª¶¯£»  speed×ªËÙ75`400£»
		}
	}
}

