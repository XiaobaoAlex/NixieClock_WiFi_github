#ifndef __esp8266_H
#define __esp8266_H
struct time
{
	unsigned char second[6];
	unsigned char minute[6];
	unsigned char hour[6];
	unsigned char weekday[6];
	unsigned char day[6];
	unsigned char month[6];
	unsigned char year[6];	
};
unsigned char atk_send_cmd(unsigned char *cmd,unsigned char len,unsigned char *result);  
void esp8266_start(void);
void esp8266_get_time(unsigned char * reback);
void extract_time(unsigned char *time_array,struct time *time_msg );
void tran_3231set_array(struct time *time_msg_part, unsigned char *time_array);
#endif