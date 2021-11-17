//模拟iic
//ds3231驱动
#include "ds3231.h"
 
 
 
void DS3231_Delay(void) {    //DS3231通信速率延时,延时5微秒  12T单片机@12M

  unsigned int i;
  unsigned int j;
  
  for(i = 0; i < 100; i++)
     for(j = 0; j < 800; j++);
  
}
 

/*************************************************************/
/*                        初始化IIC                          */
/*************************************************************/
void INIT_IIC(void) 
{

 SCL_dir = 1;			//方向寄存器设为输出
 SCL_dir = 1;
 SCL = 1;
 SDA = 1;
}

/*************************************************************/
/*                         启动IIC                           */
/*************************************************************/
void IIC_Start(void) 		//在scl为高时，sda产生由1到0的变化，启动iic
{
  SCL_dir = 1;
  SDA_dir = 1;
  SDA = 1;
  somenop();
  SCL = 1;
  somenop();
  somenop();
  SDA = 0;
  somenop();
  somenop();
  SCL = 0;
}

/*************************************************************/
/*                       IIC发送数据                         */
/*************************************************************/
void I2Csend(unsigned char data3)                     
{
 unsigned char i=8;
 SCL_dir = 1;
 SDA_dir = 1;
 while(i) 
 {
      SCL = 0;  
      somenop();
      SDA=(data3&0x80)>>7; 
      data3<<=1; 
      somenop();
      SCL = 1;
      somenop();
      somenop();
      i--;     
 }
 SCL = 0;
 SDA_dir = 0;
}

/*************************************************************/
/*                       IIC接收数据                         */
/*************************************************************/
unsigned char I2Creceive(void) 
{
 unsigned char i=8;
 unsigned char data4=0;

 SCL_dir = 1;
 SDA_dir = 0;
 while(i) 
 {
      SCL = 0;  
      somenop();
      somenop();
      SCL = 1;
      somenop();
      data4<<=1;
      data4|=SDA;
      somenop();
      i--;     
 }
 SCL = 0;
 return(data4);
}

/*************************************************************/
/*                      IIC读取应答位                        */
/*************************************************************/
void I2Creadack(void) 
{

  SCL_dir = 1;
  SDA_dir = 0;
 
  somenop();
  somenop();
  SCL = 1;
  somenop();
  while(1)
  {
  	if(SDA==0)
  	break;
  }
  somenop();
  SCL = 0;
  somenop();
  somenop();

}

/*************************************************************/
/*                      IIC发送应答位                        */
/*************************************************************/
void I2Csendack(void) 
{
  SCL_dir = 1;
  SDA_dir = 1;
  SCL = 0;
  somenop();
  SDA = 0;
  somenop();
  SCL = 1;
  somenop();
  somenop();
  SCL = 0;
  somenop();
  somenop();
}
/*************************************************************/
/*                      IIC发送非应答位                        */
/*************************************************************/
void I2CsendNack(void) 			//非应答信号用于读操作结束时，scl为高时，sda为高
{
  SCL_dir = 1;
  SDA_dir = 1;
  SCL = 0;
  somenop();
  SDA = 1;
  somenop();
  SCL = 1;
  somenop();
  somenop();
  SCL = 0;
  somenop();
  somenop();
}

/*************************************************************/
/*                         停止IIC                           */
/*************************************************************/
void IIC_Stop(void) 			//scl为高时，sda产生由0到1的变化
{
  SCL = 0;
  SDA = 0;
  SCL_dir = 1;
  SDA_dir = 1;
  SCL = 0;
  SDA = 0;
  somenop(); 
  somenop();
  SCL = 1;
  somenop();
  SDA = 1; 
  somenop(); 
  somenop();
  SCL_dir = 0;
  SDA_dir = 0;
}

unsigned char DS3231_ReceiveByte(unsigned char addr) //主设备接收指定寄存器的内容
{
	unsigned char buf=0;
	IIC_Start();
	I2Csend(DS3231WriteAddress);
	I2Creadack();
	I2Csend(addr);
	I2Creadack();
	
	IIC_Start();
	I2Csend(DS3231ReadAddress);
	I2Creadack();
	buf=I2Creceive();
	I2Csendack();
	IIC_Stop();
	
	return buf;
			
	
}
void DS3231_write_reg(unsigned char addr,unsigned char value)
{
	IIC_Start();                                             //IIC通信起始信号
    I2Csend(DS3231WriteAddress);  //写入芯片IIC写地址
    I2Creadack();
    I2Csend(addr);         //选择时寄存器为写入地址
    I2Creadack();
    I2Csend(value);                //写入指令
    I2Creadack();
    IIC_Stop(); 
    DS3231_Delay();
}

 
void DS3231_Initialization(void){            //初始化时钟芯片DS3231,先选择要写入的寄存器，在写入需要设置的指令
    

  //  DS3231_write_reg(DS3231_Hour,0x00);//写入指令，时钟范围为0-23，即24小时制式
    DS3231_write_reg(DS3231_Interrupt,0x04); //中断寄存器初始化,关闭方波信号，关闭闹钟中断
    DS3231_write_reg(DS3231_Status,0x00); //状态寄存器初始化,失效32KHz信号输出，不匹配闹钟
    
}
 
void DS3231_SetTime(unsigned char *Pointer){    //向DS3231写入设置时间信息,依次是秒、分钟、小时、星期、日、月、年
    unsigned char Number = 0x00;
  	unsigned char TransitionData = 0x00;
    IIC_Start();                             //DS3231芯片IIC通信开始信号
    I2Csend(DS3231WriteAddress);       //写入芯片IIC写地址
    I2Creadack();
    I2Csend(DS3231_TimeFirst);         //写入时间寄存器首地址
    I2Creadack();
  for(Number=0; Number<7; Number++)  {
    TransitionData = *Pointer++;
    I2Csend((TransitionData/10)*16+(TransitionData%10));   //向DS3231写入设置时间信息，每次写完地址会递增
    I2Creadack();
  }
  	IIC_Stop();                                   //DS3231芯片IIC通信停止信号
    DS3231_Delay();
}
 
void DS3231_ReadTime(unsigned char *Pointer){       //从DS3231中读出当前时间
    unsigned char Number  = 7;
  	unsigned char Time    = 0x00;
	
	IIC_Start();//DS3231芯片IIC通信开始信号
	I2Csend(DS3231WriteAddress);//写入芯片IIC写地址
	I2Creadack();
	I2Csend(DS3231_TimeFirst);//写入时间寄存器首地址
	I2Creadack();
	
	IIC_Start();                                  //DS3231芯片IIC通信开始信号
    I2Csend(DS3231ReadAddress);             //写入芯片IIC读地址
    I2Creadack();

  	for(Number=0; Number<6; Number++) {
        Time = I2Creceive();			//带有ack读取，每次读地址会递增
        I2Csendack();
    *Pointer++ = (Time/16*10+Time%16);    
  	}
    Time = I2Creceive();
    *Pointer++ = (Time/16*10+Time%16); 
    I2CsendNack();//非应答ack,结束读操作
  	IIC_Stop();                                 //DS3231芯片IIC通信停止信号
}
void DS3231_get_time_display_array(unsigned char *data)
{
    unsigned char *time_msg_time;
    DS3231_ReadTime(time_msg_time);
    *data=(*time_msg_time)/10;
    *(data+1)=(*time_msg_time)%10;
    *(data+2)=(*(time_msg_time+1))/10;
    *(data+3)=(*(time_msg_time+1))%10;
    *(data+4)=(*(time_msg_time+2))/10;
    *(data+5)=(*(time_msg_time+2))%10;
  
}
void DS3231_get_date_display_array(unsigned char *data)
{
    unsigned char * time_msg_date;
    DS3231_ReadTime(time_msg_date);
    *data=(*(time_msg_date+3))/10;
    *(data+1)=(*(time_msg_date+3))%10;
    *(data+2)=(*(time_msg_date+1))/10;
    *(data+3)=(*(time_msg_date+1))%10;
    *(data+4)=(*(time_msg_date+2))/10;
    *(data+5)=(*(time_msg_date+2))%10;
  
}
 
