#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "usart.h"


/*************************************************************/
/*                        初始化SCI                          */
/*************************************************************/
void INIT_SCI(void) 
{
  SCI1C1  = 0x00;//设置SCI0为正常模式，八位数据位，无奇偶校验
  SCI1C2 =0x2c; //接收器使能，发射器接收器使能
  SCI1BDH=0x00;//8MHz,115200bps,SCI0BDL=0x04
  SCI1BDL=0x34;//SCI波特率=BUSCLK/(16xBR),按照此公式计算出来的波特率为9615，与要求的9600有出入，但是不影响传输
              //esp8266使用AT指令时的波特率为115200

}

/*************************************************************/
/*                        延时函数                           */
/*************************************************************/
void delay(void) 
{
 unsigned int i,j;
 for(j=0;j<100;j++)
 for(i=0;i<160;i++);
}


  
/*************************************************************/
/*                       串口发送函数                        */
/*************************************************************/
void SCI_send(unsigned char data1) 
{
  while(!SCI1S1_TDRE);       //等待发送数据寄存器（缓冲器）为空
  SCI1D = data1;
}

/*************************************************************/
/*                  串口发送字符串函数                       */
/*************************************************************/
void send_string(unsigned char count,unsigned char *putchar) 
{
  while(count--)       //判断字符串是否发送完毕
  {
   SCI_send(*putchar++);  
  }

 

}

 /*************************************************************/
/*                       串口接收函数                        */
/*************************************************************/
unsigned char SCI_receive(void) 
{
  unsigned char READ_SCI1S1;
  while(!SCI1S1_RDRF)         //等待接收数据寄存器(缓冲器）为空
  READ_SCI1S1=SCI1S1;
  return(SCI1D);
}
//接收N个字节数据
//当检测到回车符号说明接受完毕
void SCI_receiveN(unsigned char n,unsigned char *ch)
 {

   byte m=0;
   for(m=0;m<n;m++) 
   {
      *(ch+m)=SCI_receive();

      
   }
 }
    
   