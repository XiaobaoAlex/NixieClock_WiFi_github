/**
 * HV57708����
 * оƬ��飺
 *     32MHz, 64ͨ������������벢��оƬ
 *     ���� 4 �� ���е� 16 λ��λ�Ĵ���
 * ���ߣ�    XiaobaoAlex
 * �����£�2021/7/22

 */

#ifndef __HV57708_H
#define __HV57708_H


#define HV57708_MODE_NORMAL 1
#define HV57708_MODE_INVERTION 0

void HV57708_Init(unsigned char HV57708_Mode);
void HV57708_SendData(unsigned long datapart1, unsigned long datapart2);
void HV57708_OutputData(void);
void HV57708_Display(unsigned char *data);
void HV57708_Protection(void);
void HV57708_5201314(void);
void SetDisplay(unsigned char data[]);

#endif