#include "string.h"
#include "usart.h"
#include "esp8266.h"



		 

   /*************************************************************/
/*                       发送接收AT指令函数                        */
/*************************************************************/
//可以通过strstr函数进行匹配  
unsigned char atk_send_cmd(unsigned char *cmd,unsigned char len,unsigned char *result)    //发送指令，判定结果，写好接收字符长度len
{   
   unsigned char *reback;
 
   while(1) {
            
        // send_string(cmd);
         SCI_receiveN(len,reback);			//此处len为期望接收字符实际长度，不可随意加大，接收字符串函数会一直等待接收直到有数据传来
        if(strstr(reback,result)!=NULL)       //  strstr(str1,str2) 函数用于判断字符串str2是否是str1的子串str1,str2为指针，如果是，则该函数返回 str1字符串从 str2第一次出现的位置开始到 str1结尾的字符串；否则，返回NULL
         break;
   }
   return 0;
}         
 





//ESP8266模块连接路由并通过sntp获取时间信息
void esp8266_start(void)
{
	//设置工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
	atk_send_cmd("AT+CWMODE=1\r\n",3,"OK");
	
	//让Wifi模块重启的命令
	atk_send_cmd("AT+RST\r\n",6,"ready");
	
	delay();        //延时3S等待重启成功
	delay();
	delay();
	delay();
	
	//让模块连接上自己的路由
	while(atk_send_cmd("AT+CWJAP=\"2205\",\"82568316+\"\r\n",30,"WIFI GOT IP"));
	
	//=0：单路连接模式     =1：多路连接模式
	atk_send_cmd("AT+CIPMUX=0\r\n",3,"OK");
	
}
//获取时间信息至结构体
void esp8266_get_time(unsigned char * reback)
{

	atk_send_cmd("AT+CIPSNTPCFG=1,8\r\n",3,"OK");	//设置时区   
  //  send_string("AT+CIPSNTPTIME?\r\n");
    SCI_receiveN(40,reback);	//查询时间
         	
}

void extract_time(unsigned char *time_array,struct time *time_msg )//从wifi模块发过来的+CIPSNTPTIME:Mon Aug 02 11:10:25 2021中提取信息至结构体，时间信息以字符形式存在结构体数组中
{
	 memcpy(time_msg->weekday, time_array+14,3);     //thu,字符表示
     *(time_msg->weekday+3)='\0';				//手动添加结束符
     memcpy(time_msg->month, time_array+18,3);     //month，字符表示
     *(time_msg->month+3)='\0';
     memcpy(time_msg->day, time_array+22,2);     //day
     *(time_msg->day+2)='\0';
     memcpy(time_msg->hour, time_array+25,2);     //hour
     *(time_msg->hour+2)='\0';
     memcpy(time_msg->minute, time_array+28,2);    //minute
     *(time_msg->minute+2)='\0';
     memcpy(time_msg->second, time_array+31,2);     //second
     *(time_msg->second+2)='\0';
     memcpy(time_msg->year, time_array+36,2);     //year，这里只提取型如21的俩位数
     *(time_msg->year+2)='\0';
	
}
void tran_3231set_array(struct time *time_msg_part, unsigned char *time_array)//从结构体中提取时间信息至数组，数组大小为7，每个元素里以数字形式存储秒、分钟、小时、星期、日、月、年
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



