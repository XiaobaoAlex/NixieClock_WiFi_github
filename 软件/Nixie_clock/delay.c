/**
 * 软件延时程序
 * 最后更新：2021/7/22
 */

#include	"delay.h"


/**
 * void  Delay(void)
 * 简介：简单延时
 * 输入：无
 * 输出：无
 */
void Delay(void) {    

  unsigned int i;
  unsigned int j;
  
  for(i = 0; i < 100; i++)
     for(j = 0; j < 1600; j++);
  
}


void HV57708_delay(void) 
{    

  unsigned int i;
  unsigned int j;
  
  for(i = 0; i < 1; i++)
     for(j = 0; j < 1; j++);
  
}
void KEY_delay(void) 
{
 unsigned int i,j;
 for(j=0;j<7;j++)
 for(i=0;i<50;i++);
}