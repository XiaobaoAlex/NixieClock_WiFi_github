#include "string.h"
#include "usart.h"
#include "esp8266.h"



		 

   /*************************************************************/
/*                       ���ͽ���ATָ���                        */
/*************************************************************/
//����ͨ��strstr��������ƥ��  
unsigned char atk_send_cmd(unsigned char *cmd,unsigned char len,unsigned char *result)    //����ָ��ж������д�ý����ַ�����len
{   
   unsigned char *reback;
 
   while(1) {
            
        // send_string(cmd);
         SCI_receiveN(len,reback);			//�˴�lenΪ���������ַ�ʵ�ʳ��ȣ���������Ӵ󣬽����ַ���������һֱ�ȴ�����ֱ�������ݴ���
        if(strstr(reback,result)!=NULL)       //  strstr(str1,str2) ���������ж��ַ���str2�Ƿ���str1���Ӵ��sstr1,str2Ϊָ�룬����ǣ���ú������� str1�ַ����� str2��һ�γ��ֵ�λ�ÿ�ʼ�� str1��β���ַ��������򣬷���NULL
         break;
   }
   return 0;
}         
 





//ESP8266ģ������·�ɲ�ͨ��sntp��ȡʱ����Ϣ
void esp8266_start(void)
{
	//���ù���ģʽ 1��stationģʽ   2��APģʽ  3������ AP+stationģʽ
	atk_send_cmd("AT+CWMODE=1\r\n",3,"OK");
	
	//��Wifiģ������������
	atk_send_cmd("AT+RST\r\n",6,"ready");
	
	delay();        //��ʱ3S�ȴ������ɹ�
	delay();
	delay();
	delay();
	
	//��ģ���������Լ���·��
	while(atk_send_cmd("AT+CWJAP=\"2205\",\"82568316+\"\r\n",30,"WIFI GOT IP"));
	
	//=0����·����ģʽ     =1����·����ģʽ
	atk_send_cmd("AT+CIPMUX=0\r\n",3,"OK");
	
}
//��ȡʱ����Ϣ���ṹ��
void esp8266_get_time(unsigned char * reback)
{

	atk_send_cmd("AT+CIPSNTPCFG=1,8\r\n",3,"OK");	//����ʱ��   
  //  send_string("AT+CIPSNTPTIME?\r\n");
    SCI_receiveN(40,reback);	//��ѯʱ��
         	
}

void extract_time(unsigned char *time_array,struct time *time_msg )//��wifiģ�鷢������+CIPSNTPTIME:Mon Aug 02 11:10:25 2021����ȡ��Ϣ���ṹ�壬ʱ����Ϣ���ַ���ʽ���ڽṹ��������
{
	 memcpy(time_msg->weekday, time_array+14,3);     //thu,�ַ���ʾ
     *(time_msg->weekday+3)='\0';				//�ֶ���ӽ�����
     memcpy(time_msg->month, time_array+18,3);     //month���ַ���ʾ
     *(time_msg->month+3)='\0';
     memcpy(time_msg->day, time_array+22,2);     //day
     *(time_msg->day+2)='\0';
     memcpy(time_msg->hour, time_array+25,2);     //hour
     *(time_msg->hour+2)='\0';
     memcpy(time_msg->minute, time_array+28,2);    //minute
     *(time_msg->minute+2)='\0';
     memcpy(time_msg->second, time_array+31,2);     //second
     *(time_msg->second+2)='\0';
     memcpy(time_msg->year, time_array+36,2);     //year������ֻ��ȡ����21����λ��
     *(time_msg->year+2)='\0';
	
}
void tran_3231set_array(struct time *time_msg_part, unsigned char *time_array)//�ӽṹ������ȡʱ����Ϣ�����飬�����СΪ7��ÿ��Ԫ������������ʽ�洢�롢���ӡ�Сʱ�����ڡ��ա��¡���
{
	*time_array=(*(time_msg_part->second)-'0')*10+(*(time_msg_part->second+1)-'0');
	*(time_array+1)=(*(time_msg_part->minute)-'0')*10+(*(time_msg_part->minute+1)-'0');
	*(time_array+2)=(*(time_msg_part->hour)-'0')*10+(*(time_msg_part->hour+1)-'0');
	if(strstr(time_msg_part->weekday,"Mon")!=NULL)
	*(time_array+3)=1;
	if(strstr(time_msg_part->weekday,"Tue")!=NULL)
	*(time_array+3)=2;
	if(strstr(time_msg_part->weekday,"Wed")!=NULL)
	*(time_array+3)=3;
	if(strstr(time_msg_part->weekday,"Thu")!=NULL)
	*(time_array+3)=4;
	if(strstr(time_msg_part->weekday,"Fri")!=NULL)
	*(time_array+3)=5;
	if(strstr(time_msg_part->weekday,"Sat")!=NULL)
	*(time_array+3)=6;
	if(strstr(time_msg_part->weekday,"Sun")!=NULL)
	*(time_array+3)=7;
	*(time_array+4)=(*(time_msg_part->day)-'0')*10+(*(time_msg_part->day+1)-'0');
	if(strstr(time_msg_part->month,"Jan")!=NULL)
	*(time_array+5)=1;
	if(strstr(time_msg_part->month,"Feb")!=NULL)
	*(time_array+5)=2;
	if(strstr(time_msg_part->month,"Mar")!=NULL)
	*(time_array+5)=3;
	if(strstr(time_msg_part->month,"Apr")!=NULL)
	*(time_array+5)=4;
	if(strstr(time_msg_part->month,"May")!=NULL)
	*(time_array+5)=5;
	if(strstr(time_msg_part->month,"Jun")!=NULL)
	*(time_array+5)=6;
	if(strstr(time_msg_part->month,"Jul")!=NULL)
	*(time_array+5)=7;
	if(strstr(time_msg_part->month,"Aug")!=NULL)
	*(time_array+5)=8;
	if(strstr(time_msg_part->month,"Sep")!=NULL)
	*(time_array+5)=9;
	if(strstr(time_msg_part->month,"Oct")!=NULL)
	*(time_array+5)=10;
	if(strstr(time_msg_part->month,"Nov")!=NULL)
	*(time_array+5)=11;
	if(strstr(time_msg_part->month,"Dec")!=NULL)
	*(time_array+5)=12;
	*(time_array+6)=(*(time_msg_part->year)-'0')*10+(*(time_msg_part->year+1)-'0');
}



