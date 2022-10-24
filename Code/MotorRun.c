#include <REGX52.H>
#include "delaytime10us.h"

#define uint unsigned int

sbit MA=P1^0;
sbit MB=P1^1;
sbit MC=P1^2;
sbit MD=P1^3;


/*
angle：角度，范围[0,360]
drct：旋转方向，0：顺时针，1：逆时针
speed：转速，范围[1,100]
*/
void MotorRun(unsigned int nangle, drct, speed)					//nangle=4096为一圈						 speed转速75`400
{
	uint step;
	step=0;
	if(drct==0)
	{
		while(nangle--)
		{
			switch(step)//8拍方式驱动
			{
				case 0:MA=1;MB=0;MC=0;MD=0;break;
				case 1:MA=1;MB=1;MC=0;MD=0;break;
				case 2:MA=0;MB=1;MC=0;MD=0;break;
				case 3:MA=0;MB=1;MC=1;MD=0;break;
				case 4:MA=0;MB=0;MC=1;MD=0;break;
				case 5:MA=0;MB=0;MC=1;MD=1;break;
				case 6:MA=0;MB=0;MC=0;MD=1;break;
				case 7:MA=1;MB=0;MC=0;MD=1;break;
			}
			if(step==7)step=0;
			else step++;
			delay(speed);
		}
	}
	else
	{
		while(nangle--)
		{
			switch(step)
			{
				case 0:MA=1;MB=0;MC=0;MD=1;break;
				case 1:MA=0;MB=0;MC=0;MD=1;break;
				case 2:MA=0;MB=0;MC=1;MD=1;break;
				case 3:MA=0;MB=0;MC=1;MD=0;break;
				case 4:MA=0;MB=1;MC=1;MD=0;break;
				case 5:MA=0;MB=1;MC=0;MD=0;break;
				case 6:MA=1;MB=1;MC=0;MD=0;break;
				case 7:MA=1;MB=0;MC=0;MD=0;break;
			}
			if(step==7)step=0;
			else step++;
			delay(speed);
		}
	}
	MA=0;MB=0;MC=0;MD=0;
}
