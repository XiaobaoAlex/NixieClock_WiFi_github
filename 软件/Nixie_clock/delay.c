/**
 * �����ʱ����
 * �����£�2021/7/22
 */

#include	"delay.h"


/**
 * void  Delay(void)
 * ��飺����ʱ
 * ���룺��
 * �������
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