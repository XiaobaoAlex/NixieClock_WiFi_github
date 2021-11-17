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


unsigned int Flag_Collect;//���ڲɼ�ʱ���־
#define RTC_1_MS        1
#define RTC_10_MS       10
#define RTC_100_MS      100
#define RTC_200_MS      200
#define RTC_1000_SEC    1000//RTCģ���Ĵ���
#define LED	PTED_PTED5
#define LED_D	PTEDD_PTEDD5

struct time timearray;
unsigned char set[7]={0,0,0,0,0,0,0};
unsigned char cmd[4]={"AT\r\n"};//���鳤�ȱ�����ʵ��ָ���ַ�����һ��
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
unsigned int Flag_Protect_count=0;//����������ʾ����
unsigned char CurrentT_all[7]={0,0,0,0,0,0,0};//���ڴ洢��ds3231��ȡ������ʱ����Ϣ
unsigned char CurrentT_time_array[6]={0,0,0,0,0,0};
const unsigned char sntp_time_data[7]={25,10,11,1,2,8,21};//���ڴ洢ͨ��sntp��ȡ��������ʱ������롢���ӡ�ʱ�����ڡ��ա��¡���
unsigned char reback[40]={"+CIPSNTPTIME:Mon Aug 02 11:10:25 2021"};



/*************************************************************/
/*                      ��ʼ�����໷                         */
/*************************************************************/

void Init_PLL(void)
{


  /* disable COP */
  SOPT1 = SOPT1_STOPE_MASK;     /* disable COP, enable stop mode ���ÿ��Ź���ʹ��ֹͣģʽ*/

  /* Init Clock, Use PLL Engaged External mode, from 4MHz crystal -> 8 MHz bus��ʼ��ʱ�ӣ�ʹ��PEEģʽ����4M�ⲿ����ת��8M����ʱ�� */  
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

//�˴��밴dz60�����ֲᣨ2008�棩ʾ�������д
}
/*************************************************************/
/*                       ��ʼ��ʵʱ�ж�                      */
/*************************************************************/
 void Rtc_Init(void)
 {
     RTCSC = 0x18;            /*LPO clock,1 kHz�͹������� ������ģʽ�����������ļ�������Ϊ1ms��ʵʱ�ж�ʹ��*/
     RTCMOD = RTC_200_MS;      /*default set to 200ms  */
     
 }

/*************************************************************/
/*                       ������ѹ��·                      */
/*************************************************************/
void BOOST_ON(void)
{
	
		PTBDD_PTBDD5=1;
		PTBD_PTBD5=0;
}
/*************************************************************/
/*                       �ر���ѹ��·                      */
/*************************************************************/
void BOOST_OFF(void)
{
	
		PTBDD_PTBDD5=1;
		PTBD_PTBD5=1;
}
/*************************************************************/
/*                       �ʹܿ���                      */
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
/*                       ��������������ʾ                      */
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
/*                       ʵʱ�жϺ���                        */
/*************************************************************/
void interrupt 25 RTI_inter(void)
{   
if(RTCSC_RTIF == 1)
    RTCSC_RTIF = 1;
    Flag_Collect=1;   //��Լ0.2s�ɼ�һ��  
}
/*
#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt void RTI_inter(void)
{
  if(RTCSC_RTIF == 1)
    RTCSC_RTIF = 1;
    Flag_Collect=1;   //��Լ0.2s�ɼ�һ��
 
}
#pragma CODE_SEG DEFAULT

*/
int is_in( char *s,  char *c)
{
    int i=0,j=0,flag=-1;
    while(i<strlen(s) && j<strlen(c)){
        if(s[i]==c[j]){//����ַ���ͬ�������ַ�������
            i++;
            j++;
        }else{
            i=i-j+1; //�����ַ��ص��Ƚ��ʼ�Ƚϵĺ�һ���ַ�
            j=0;     //�ִ��ַ����¿�ʼ
        }
        if(j==strlen(c)){ //���ƥ��ɹ�
            flag=1;  //�ִ�����
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


void main(){                                             //��ʱ�ɼ�DS3231ʱ��оƬʱ����Ϣ��ͨ��RS232���ڴ�ӡ


  	LED_D=1;
    LED=1;
    Init_PLL();//��ʼ�����໷������ʱ��Ϊ8M
    OETXS_D=1;
    OETXS=1;//ʹ�ܵ�ƽת��оƬ���ߵ�ƽ��оƬʹ��
    GPIO_0D=1;
    GPIO_0=1;
    RESET_ESPD=1;
    RESET_ESP=1;//����wifiģ������
    INIT_IIC();//��ʼ��iic
	DS3231_Initialization();//��ʼ��DS3231
    Rtc_Init();//ʵʱ�жϳ�ʼ��
    HV57708_Init(HV57708_MODE_INVERTION);//��ʼ��HV57708,���йܽ���������Թ�ܾ�Ĭ,�������
    BOOST_ON();//��ѹ��·����
    NEON_ON();//�ʹܵ���
    HV57708_Display(CurrentT_time_array);
    INIT_SCI();
    
    send_string(4,cmd);//����ATָ����ｨ�����÷����ַ���������Ȼ����ַ������ͺ������ڷ���ȶ��ַ�֮��һЩ��ֵĶ�����
    delay();
    delay();
  	delay();
    delay();//0.167s������ָ��֮��Ҫһ����ʱ�ȴ�����
    
    send_string(6,close_echo);//�رջ��ԣ�������رյĻ�rx������ʾ���͵���Ϣ����ʾ����
    delay();
    delay();
  	delay();
    delay();
 	
    send_string(13,cmd_MODE);//����ģʽ
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
    delay();//��������ϣ��������ջ����յ�WIFI DISCONNECT,���յ�WIFI CONNECTED,����ʱ���գ�ȷ�����յ�WIFI CONNECTED
    //���û�����ϣ���ʱ���ջ���յ�FAI,����Ҳ�ܽ���������ж�
    SCI_receiveN(16,reback_WIFI);//�����һֱ�ȴ����գ���������ַ����ȱ�����ʵ�ʽ����ַ�����һ��

    ret = NULL;
    ret = strstr(reback_WIFI, result);
	if(ret == NULL)
	{
		send_string(29,cmd_WIFI);//4s��Ӧ������wifi
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
    send_string(36,cmd_TimeZone);//����ʱ��������ʱ�����ܼ���Ҫ��wifi����·�ɺ����ã�����ǰ���������ʱ����
    delay();
  	delay();
    delay(); 
    send_string(17,cmd_Time);//��ȡʱ��
    SCI_receiveN(43,reback_Time);//����ʱ�䣬�����һֱ�ȴ����գ���������ַ����ȱ�����ʵ�ʽ����ַ�����һ��
    
    extract_time(reback_Time,&timearray);
    tran_3231set_array(&timearray, set);
    
    ret1 = NULL;
    ret1 = strstr(reback_WIFI1, result);
    if((ret!=NULL)|(ret1!=NULL))
    DS3231_SetTime(set);//���ոջ�ȡ��ʱ����Ϣд��DS3231
    delay();
    delay();
  	delay();
    delay();
     
    DS3231_ReadTime(CurrentT_all);                               //�������������Դ��·���߹���ϵͳ�����������ĵ�����Ч ÿ���ϵ��ȡһ�ε�ǰʱ����Ϣ
    if(CurrentT_all[6] == 0x00){                                 //�����ϢΪ0x00��оƬʱ����Ҫ��ʼ��
        DS3231_Initialization();                               //��ʼ��ʱ��ʱ��оƬDS3231
       	send_string(17,cmd_Time);//��ȡʱ��
    	SCI_receiveN(43,reback_Time);//����ʱ�䣬�����һֱ�ȴ����գ���������ַ����ȱ�����ʵ�ʽ����ַ�����һ��
    	extract_time(reback_Time,&timearray);
    	tran_3231set_array(&timearray, set); 
    	if((ret!=NULL)|(ret1!=NULL))
    	DS3231_SetTime(set);//���ոջ�ȡ��ʱ����Ϣд��DS3231                               
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
 