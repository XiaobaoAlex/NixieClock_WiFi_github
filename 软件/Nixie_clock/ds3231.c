//ģ��iic
//ds3231����
#include "ds3231.h"
 
 
 
void DS3231_Delay(void) {    //DS3231ͨ��������ʱ,��ʱ5΢��  12T��Ƭ��@12M

  unsigned int i;
  unsigned int j;
  
  for(i = 0; i < 100; i++)
     for(j = 0; j < 800; j++);
  
}
 

/*************************************************************/
/*                        ��ʼ��IIC                          */
/*************************************************************/
void INIT_IIC(void) 
{

 SCL_dir = 1;			//����Ĵ�����Ϊ���
 SCL_dir = 1;
 SCL = 1;
 SDA = 1;
}

/*************************************************************/
/*                         ����IIC                           */
/*************************************************************/
void IIC_Start(void) 		//��sclΪ��ʱ��sda������1��0�ı仯������iic
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
/*                       IIC��������                         */
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
/*                       IIC��������                         */
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
/*                      IIC��ȡӦ��λ                        */
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
/*                      IIC����Ӧ��λ                        */
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
/*                      IIC���ͷ�Ӧ��λ                        */
/*************************************************************/
void I2CsendNack(void) 			//��Ӧ���ź����ڶ���������ʱ��sclΪ��ʱ��sdaΪ��
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
/*                         ֹͣIIC                           */
/*************************************************************/
void IIC_Stop(void) 			//sclΪ��ʱ��sda������0��1�ı仯
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

unsigned char DS3231_ReceiveByte(unsigned char addr) //���豸����ָ���Ĵ���������
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
	IIC_Start();                                             //IICͨ����ʼ�ź�
    I2Csend(DS3231WriteAddress);  //д��оƬIICд��ַ
    I2Creadack();
    I2Csend(addr);         //ѡ��ʱ�Ĵ���Ϊд���ַ
    I2Creadack();
    I2Csend(value);                //д��ָ��
    I2Creadack();
    IIC_Stop(); 
    DS3231_Delay();
}

 
void DS3231_Initialization(void){            //��ʼ��ʱ��оƬDS3231,��ѡ��Ҫд��ļĴ�������д����Ҫ���õ�ָ��
    

  //  DS3231_write_reg(DS3231_Hour,0x00);//д��ָ�ʱ�ӷ�ΧΪ0-23����24Сʱ��ʽ
    DS3231_write_reg(DS3231_Interrupt,0x04); //�жϼĴ�����ʼ��,�رշ����źţ��ر������ж�
    DS3231_write_reg(DS3231_Status,0x00); //״̬�Ĵ�����ʼ��,ʧЧ32KHz�ź��������ƥ������
    
}
 
void DS3231_SetTime(unsigned char *Pointer){    //��DS3231д������ʱ����Ϣ,�������롢���ӡ�Сʱ�����ڡ��ա��¡���
    unsigned char Number = 0x00;
  	unsigned char TransitionData = 0x00;
    IIC_Start();                             //DS3231оƬIICͨ�ſ�ʼ�ź�
    I2Csend(DS3231WriteAddress);       //д��оƬIICд��ַ
    I2Creadack();
    I2Csend(DS3231_TimeFirst);         //д��ʱ��Ĵ����׵�ַ
    I2Creadack();
  for(Number=0; Number<7; Number++)  {
    TransitionData = *Pointer++;
    I2Csend((TransitionData/10)*16+(TransitionData%10));   //��DS3231д������ʱ����Ϣ��ÿ��д���ַ�����
    I2Creadack();
  }
  	IIC_Stop();                                   //DS3231оƬIICͨ��ֹͣ�ź�
    DS3231_Delay();
}
 
void DS3231_ReadTime(unsigned char *Pointer){       //��DS3231�ж�����ǰʱ��
    unsigned char Number  = 7;
  	unsigned char Time    = 0x00;
	
	IIC_Start();//DS3231оƬIICͨ�ſ�ʼ�ź�
	I2Csend(DS3231WriteAddress);//д��оƬIICд��ַ
	I2Creadack();
	I2Csend(DS3231_TimeFirst);//д��ʱ��Ĵ����׵�ַ
	I2Creadack();
	
	IIC_Start();                                  //DS3231оƬIICͨ�ſ�ʼ�ź�
    I2Csend(DS3231ReadAddress);             //д��оƬIIC����ַ
    I2Creadack();

  	for(Number=0; Number<6; Number++) {
        Time = I2Creceive();			//����ack��ȡ��ÿ�ζ���ַ�����
        I2Csendack();
    *Pointer++ = (Time/16*10+Time%16);    
  	}
    Time = I2Creceive();
    *Pointer++ = (Time/16*10+Time%16); 
    I2CsendNack();//��Ӧ��ack,����������
  	IIC_Stop();                                 //DS3231оƬIICͨ��ֹͣ�ź�
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
 
