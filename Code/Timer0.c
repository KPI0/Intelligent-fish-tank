#include <REGX52.H>
//#include "MotorRun.h"
/**
  * @brief  定时器0初始化，1毫秒@12.000MHz
  * @param  无
  * @retval 无
  */
	
unsigned char Sec=0,Min=0,Hour=0;
int zhuanFlag=0;
void Timer0Init(void)
{
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
	PT0=0;
}

void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count,HourCount;
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	T0Count++;   //1000次计数
			if(T0Count>=1000)	//定时器分频，1s
		{
			T0Count=0;
			Sec++;			//1秒到，Sec自增
			if(Sec>=60)
			{
				Sec=0;		//60秒到，Sec清0，Min自增
				Min++;
				if(Min>=60)
				{
					Min=0;	//60分钟到，Min清0，Hour自增
					Hour++;
					if(Hour>=24)
					{
						Hour=0;	//24小时到，Hour清0
						HourCount++;
				}
			}
		}
	}
}


