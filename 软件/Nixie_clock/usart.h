#ifndef __usart_H
#define __usart_H
void INIT_SCI(void); 
void delay(void); 
void SCI_send(unsigned char data1); 
void send_string(unsigned char count,unsigned char *putchar); 
unsigned char SCI_receive(void); 
void SCI_receiveN(unsigned char n,unsigned char *ch);
#endif