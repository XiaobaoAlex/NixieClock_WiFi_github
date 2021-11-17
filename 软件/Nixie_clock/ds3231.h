#ifndef __ds3231_H
#define __ds3231_H

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#define  DS3231WriteAddress 0xd0  
#define  DS3231ReadAddress  0xd1
#define  DS3231_Second      0x00  
#define  DS3231_TimeFirst   0x00 
#define  DS3231_Minute      0x01 
#define  DS3231_Hour        0x02 
#define  DS3231_Week        0x03 
#define  DS3231_Day         0x04 
#define  DS3231_Month       0x05 
#define  DS3231_Year        0x06 
#define  DS3231_Interrupt   0x0e 
#define  DS3231_Status      0x0f
#define SCL PTFD_PTFD2
#define SDA PTFD_PTFD3
#define SCL_dir PTFDD_PTFDD2
#define SDA_dir PTFDD_PTFDD3
#define somenop(); asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");\
                   asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");\
                   asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");\
                   asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

void INIT_IIC(void);
void DS3231_Delay(void); 
void IIC_Start(void);
void I2Csend(unsigned char data3);
unsigned char I2Creceive(void);
void I2Creadack(void);
void I2Csendack(void);
void I2CsendNack(void); 
void IIC_Stop(void);
void DS3231_write_reg(unsigned char addr,unsigned char value);
unsigned char DS3231_ReceiveByte(unsigned char addr);
void DS3231_Initialization(void);
void DS3231_SetTime(unsigned char *Pointer);
void DS3231_ReadTime(unsigned char *Pointer);
void DS3231_get_time_display_array(unsigned char *data);
void DS3231_get_date_display_array(unsigned char *data);
#endif