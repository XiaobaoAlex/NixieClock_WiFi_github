/**
 * HV57708驱动
 * 芯片简介：
 *     32MHz, 64通道推挽输出串入并出芯片
 *     内有 4 个 并行的 16 位移位寄存器
 * 作者：    XiaobaoAlex
 * 最后更新：2021/7/22

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