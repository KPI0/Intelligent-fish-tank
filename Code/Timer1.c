#include <REGX52.H>
#include "MotorRun.h"
sbit RE1=P2^0;
sbit RE2=P2^1;
/**
  * @brief  定时器0初始化，1毫秒@12.000MHz
  * @param  无
  * @retval 无
  */

extern unsigned char Sec,Min,Hour,Day;//,SecStart
extern int zhuanFlag;
int day=0;
void Timer1Init(void)
{
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x10;		//设置定时器模式
	TL1 = 0x66;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF0标志
	TR1 = 1;		//定时器0开始计时
	ET1=1;
	EA=1;
	PT1=0;
}

void Timer1_Routine() interrupt 3
{
	static unsigned int T1Count;
	TL1 = 0x66;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	T1Count++;   //1000次计数
			
			if(T1Count>=1000)	//定时器分频，1s
		{
			T1Count=0;
			Sec++;			//1秒到，Sec自增
				if(Sec==10)
				{
				  RE1=~RE1;
				  RE2=~RE2;
				}
				else if(Sec==15)
				{
					Sec=0;
					RE1=~RE1;
				  RE2=~RE2;
					MotorRun(4389/2,1,100);//nangle=4096为一圈； drct=0为逆时针转动，drct=1为顺时针转动； speed转速75`400；
					MotorRun(4389/2,0,100);//nangle=4096为一圈； drct=0为逆时针转动，drct=1为顺时针转动； speed转速75`400；
				}
//				{
//					Sec=0;
//					Min++;
//					if(Min==60)
//					{
//						Min=0;
//						Hour++;
//						if(Hour==12)
//						{
//							MotorRun(4389/2,1,100);//nangle=4096为一圈； drct=0为逆时针转动，drct=1为顺时针转动； speed转速75`400；
//							MotorRun(4389/2,0,100);//nangle=4096为一圈； drct=0为逆时针转动，drct=1为顺时针转动； speed转速75`400；
//						}
//						else if(Hour==24)
//						{
//							Hour=0;
//							Day++;
//							if(Day==3)
//							{
//								Day=0;
//								RE1=~RE1;
//								RE2=~RE2;
//							}
//						}
//					}
//				}

		}
}
