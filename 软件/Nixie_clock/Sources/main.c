#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "HV57708.h"
#include "PinDef.h"
#include "delay.h"
#include "ds3231.h"
#include "esp8266.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"


unsigned int Flag_Collect;//周期采集时间标志
#define RTC_1_MS        1
#define RTC_10_MS       10
#define RTC_100_MS      100
#define RTC_200_MS      200
#define RTC_1000_SEC    1000//RTC模数寄存器
#define LED	PTED_PTED5
#define LED_D	PTEDD_PTEDD5

struct time timearray;
unsigned char set[7]={0,0,0,0,0,0,0};
unsigned char cmd[4]={"AT\r\n"};//数组长度必须与实际指令字符长度一致
unsigned char close_echo[6]={"ATE0\r\n"};
unsigned char cmd_MODE[13]={"AT+CWMODE=1\r\n"};
unsigned char cmd_WIFI[29]={"AT+CWJAP=\"2205\",\"82568316+\"\r\n"};
unsigned char cmd_WIFI1[33]={"AT+CWJAP=\"CMCC-A35C\",\"6a3aJGbc\"\r\n"};
unsigned char cmd_TimeZone[36]={"AT+CIPSNTPCFG=1,8,\"ntp.aliyun.com\"\r\n"};
unsigned char cmd_Time[17]={"AT+CIPSNTPTIME?\r\n"};
unsigned char reback_Time[60]={'0','0','0','0','0','0','0','0','0','0',\
								'0','0','0','0','0','0','0','0','0','0',\
								'0','0','0','0','0','0','0','0','0','0',\
								'0','0','0','0','0','0','0','0','0','0',\
								'0','0','0','0','0','0','0','0','0','0',\
								'0','0','0','0','0','0','0','0','0','0'};
char reback_WIFI[20]={'0','0','0','0','0','0','0','0','0','0',\
'0','0','0','0','0','0','0','0','0','\0'};
char reback_WIFI1[20]={'0','0','0','0','0','0','0','0','0','0',\
'0','0','0','0','0','0','0','0','0','\0'};
unsigned char reback_WIFI_DISCONNECT[20]={'0','0','0','0','0','0','0','0','0','0',\
'0','0','0','0','0','0','0','0','0','0'};
unsigned char reback_WIFI_GOT_IP[20]={'0','0','0','0','0','0','0','0','0','0',\
'0','0','0','0','0','0','0','0','0','0'};
char result[14]={"CONNECTED"};
unsigned char result_dis[10]={"DISCONNECT"};
int flag1=0;
int flag2=0;
Bool flag=0;
char *ret;
char *ret1;
unsigned char State_Timing=0;
unsigned int Flag_Protect_count=0;//阴极保护显示计数
unsigned char CurrentT_all[7]={0,0,0,0,0,0,0};//用于存储从ds3231获取的完整时间信息
unsigned char CurrentT_time_array[6]={0,0,0,0,0,0};
const unsigned char sntp_time_data[7]={25,10,11,1,2,8,21};//用于存储通过sntp获取的完整的时间包括秒、分钟、时、星期、日、月、年
unsigned char reback[40]={"+CIPSNTPTIME:Mon Aug 02 11:10:25 2021"};



/*************************************************************/
/*                      初始化锁相环                         */
/*************************************************************/

void Init_PLL(void)
{


  /* disable COP */
  SOPT1 = SOPT1_STOPE_MASK;     /* disable COP, enable stop mode 禁用看门狗，使能停止模式*/

  /* Init Clock, Use PLL Engaged External mode, from 4MHz crystal -> 8 MHz bus初始化时钟，使用PEE模式，从4M外部晶振转到8M总线时钟 */  
  MCGC2 = MCGC2_RANGE_MASK|MCGC2_HGO_MASK|MCGC2_EREFS_MASK|MCGC2_ERCLKEN_MASK;
 		
  while (!MCGSC_OSCINIT);

  MCGC1 = MCGC1_CLKS1_MASK|MCGC1_RDIV2_MASK|MCGC1_RDIV1_MASK|MCGC1_RDIV0_MASK;
  while (MCGSC_IREFST);       
  while(MCGSC_CLKST != 0x02);
  
  MCGC1 = MCGC1_RDIV1_MASK|MCGC1_CLKS1_MASK;
  MCGC3 = MCGC3_PLLS_MASK|MCGC3_VDIV2_MASK;
  while(!MCGSC_PLLST);

 

  MCGC1 =MCGC1_RDIV1_MASK ;
  while(MCGSC_CLKST != 0x03);
  while (!MCGSC_LOCK); 

//此代码按dz60数据手册（2008版）示例程序编写
}
/*************************************************************/
/*                       初始化实时中断                      */
/*************************************************************/
 void Rtc_Init(void)
 {
     RTCSC = 0x18;            /*LPO clock,1 kHz低功率振荡器 ，此种模式产生计数器的计数周期为1ms，实时中断使能*/
     RTCMOD = RTC_200_MS;      /*default set to 200ms  */
     
 }

/*************************************************************/
/*                       开启升压电路                      */
/*************************************************************/
void BOOST_ON(void)
{
	
		PTBDD_PTBDD5=1;
		PTBD_PTBD5=0;
}
/*************************************************************/
/*                       关闭升压电路                      */
/*************************************************************/
void BOOST_OFF(void)
{
	
		PTBDD_PTBDD5=1;
		PTBD_PTBD5=1;
}
/*************************************************************/
/*                       氖管开启                      */
/*************************************************************/
void NEON_ON(void)
{
	
 NEON_DOT0_D=1; 
 NEON_DOT1_D=1; 
 NEON_DOT2_D=1;
 NEON_DOT3_D=1;  
 
 NEON_DOT0 = 1;
 NEON_DOT1 = 1; 
 NEON_DOT2 = 1; 
 NEON_DOT3 = 1; 
}
/*************************************************************/
/*                       按键驱动日期显示                      */
/*************************************************************/
void KEY_drive(void)
{
 JUMP1_D=0;	
 if(JUMP1==0)
 {
 	KEY_delay();
 	if(JUMP1==0)
 	{
 	    DS3231_ReadTime(CurrentT_all);
        CurrentT_time_array[0]=CurrentT_all[6]/10;
	    CurrentT_time_array[1]=CurrentT_all[6]%10;
	    CurrentT_time_array[2]=CurrentT_all[5]/10;
	    CurrentT_time_array[3]=CurrentT_all[5]%10;
	    CurrentT_time_array[4]=CurrentT_all[4]/10;
	    CurrentT_time_array[5]=CurrentT_all[4]%10;                     
        
 		HV57708_Display(CurrentT_time_array);
 		Delay();
    	Delay();
    	Delay();
    	Delay();
    	Delay();
    	Delay();
    	Delay();
    	Delay();
    	Delay();
    	Delay();	
 	}
 	
 }
}
/*************************************************************/
/*                       实时中断函数                        */
/*************************************************************/
void interrupt 25 RTI_inter(void)
{   
if(RTCSC_RTIF == 1)
    RTCSC_RTIF = 1;
    Flag_Collect=1;   //大约0.2s采集一次  
}
/*
#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt void RTI_inter(void)
{
  if(RTCSC_RTIF == 1)
    RTCSC_RTIF = 1;
    Flag_Collect=1;   //大约0.2s采集一次
 
}
#pragma CODE_SEG DEFAULT

*/
int is_in( char *s,  char *c)
{
    int i=0,j=0,flag=-1;
    while(i<strlen(s) && j<strlen(c)){
        if(s[i]==c[j]){//如果字符相同则两个字符都增加
            i++;
            j++;
        }else{
            i=i-j+1; //主串字符回到比较最开始比较的后一个字符
            j=0;     //字串字符重新开始
        }
        if(j==strlen(c)){ //如果匹配成功
            flag=1;  //字串出现
            break;
        }
    }
    return flag;
}

Bool isCoincide(char *a, char *p)
{
	char *ptemp = p;
	while (*a != '\0')
	{
		if (*a == *p)
		{
			a++;
			p++;
		}
		else
		{
			a++;
			p = ptemp;
		}
		if (*p == '\0')
		{
			return 1;
		}
	}
	return 0;
}


void main(){                                             //定时采集DS3231时钟芯片时间信息，通过RS232串口打印


  	LED_D=1;
    LED=1;
    Init_PLL();//初始化锁相环，总线时钟为8M
    OETXS_D=1;
    OETXS=1;//使能电平转换芯片，高电平该芯片使能
    GPIO_0D=1;
    GPIO_0=1;
    RESET_ESPD=1;
    RESET_ESP=1;//配置wifi模块引脚
    INIT_IIC();//初始化iic
	DS3231_Initialization();//初始化DS3231
    Rtc_Init();//实时中断初始化
    HV57708_Init(HV57708_MODE_INVERTION);//初始化HV57708,所有管脚无输出，辉光管静默,反相输出
    BOOST_ON();//升压电路开启
    NEON_ON();//氖管点亮
    HV57708_Display(CurrentT_time_array);
    INIT_SCI();
    
    send_string(4,cmd);//发送AT指令，这里建议设置发送字符个数，不然这个字符串发送函数会在发完既定字符之后发一些奇怪的东西。
    delay();
    delay();
  	delay();
    delay();//0.167s，发送指令之后要一段延时等待回文
    
    send_string(6,close_echo);//关闭回显，这个不关闭的话rx会先显示发送的信息再显示回文
    delay();
    delay();
  	delay();
    delay();
 	
    send_string(13,cmd_MODE);//设置模式
    delay();
    delay();
  	delay();
    delay();


    send_string(33,cmd_WIFI1);
    delay();
    delay();
  	delay();
    delay();
    delay();
    delay();
  	delay();
    delay();//如果连接上，立即接收会先收到WIFI DISCONNECT,再收到WIFI CONNECTED,故延时接收，确保接收到WIFI CONNECTED
    //如果没连接上，延时接收会接收到FAI,这样也能进入下面的判断
    SCI_receiveN(16,reback_WIFI);//这里会一直等待接收，这个接收字符长度必须与实际接收字符长度一致

    ret = NULL;
    ret = strstr(reback_WIFI, result);
	if(ret == NULL)
	{
		send_string(29,cmd_WIFI);//4s后应答连上wifi
		Delay();
	    Delay();
	    Delay();
	    Delay();
	    Delay();
	    Delay();
	    Delay();
	    Delay();
	    SCI_receiveN(16,reback_WIFI1);	
	}
    Delay();
    Delay();
    Delay();
    Delay();
    Delay();
  	Delay();
    Delay();
    Delay();
    Delay();
  	Delay();
    Delay();
    Delay();
    Delay();
  	Delay();
    Delay();
    send_string(36,cmd_TimeZone);//设置时区，设置时区不能急，要等wifi连接路由后设置，所以前面有许多延时函数
    delay();
  	delay();
    delay(); 
    send_string(17,cmd_Time);//获取时间
    SCI_receiveN(43,reback_Time);//接收时间，这里会一直等待接收，这个接收字符长度必须与实际接收字符长度一致
    
    extract_time(reback_Time,&timearray);
    tran_3231set_array(&timearray, set);
    
    ret1 = NULL;
    ret1 = strstr(reback_WIFI1, result);
    if((ret!=NULL)|(ret1!=NULL))
    DS3231_SetTime(set);//将刚刚获取的时间信息写入DS3231
    delay();
    delay();
  	delay();
    delay();
     
    DS3231_ReadTime(CurrentT_all);                               //掉电检测操作，电源短路或者供电系统电量彻底消耗掉才有效 每次上电读取一次当前时间信息
    if(CurrentT_all[6] == 0x00){                                 //年份信息为0x00，芯片时间需要初始化
        DS3231_Initialization();                               //初始化时间时钟芯片DS3231
       	send_string(17,cmd_Time);//获取时间
    	SCI_receiveN(43,reback_Time);//接收时间，这里会一直等待接收，这个接收字符长度必须与实际接收字符长度一致
    	extract_time(reback_Time,&timearray);
    	tran_3231set_array(&timearray, set); 
    	if((ret!=NULL)|(ret1!=NULL))
    	DS3231_SetTime(set);//将刚刚获取的时间信息写入DS3231                               
    }


    while(1)
    {
        LED=1;
        delay();
        delay();
        LED=0;
        delay();
        delay();
        Flag_Protect_count++;
        if(Flag_Protect_count==150)
        {
          HV57708_Protection();
        }
        if(Flag_Protect_count==300)
        {
          HV57708_5201314();
          Flag_Protect_count=0;	
        }
    	KEY_drive();
        DS3231_ReadTime(CurrentT_all);
        CurrentT_time_array[0]=CurrentT_all[2]/10;
	    CurrentT_time_array[1]=CurrentT_all[2]%10;
	    CurrentT_time_array[2]=CurrentT_all[1]/10;
	    CurrentT_time_array[3]=CurrentT_all[1]%10;
	    CurrentT_time_array[4]=CurrentT_all[0]/10;
	    CurrentT_time_array[5]=CurrentT_all[0]%10;                     
        
 		HV57708_Display(CurrentT_time_array);                                                            
                       
        delay();
        delay();
        delay();
        delay();

              
        
    }
    
}
 