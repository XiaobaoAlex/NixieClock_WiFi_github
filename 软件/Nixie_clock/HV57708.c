/**
 * HV57708驱动
 * 芯片简介：
 *     32MHz, 64通道推挽输出串入并出芯片
 *     内有 4 个 并行的 16 位移位寄存器
 * 作者：    XiaobaoAlex
 * 最后更新：2021/7/22

 *****************************************************************************
 * 使用本程序时，需要在此处或 PinDef.h 中定义 HV57708 的各个引脚，具体包括：
 *     HV57708_CLK - CLK
 *     HV57708_LE  - LE
 *     HV57708_POL - POL
 *     HV57708_DI1 - Din1/Dout4(A)
 *     HV57708_DI2 - Din2/Dout3(A)
 *     HV57708_DI3 - Din3/Dout2(A)
 *     HV67708_DI4 - Din4/Dout1(A)
 *****************************************************************************
 *     HG57708 的 BL 接高电平，DIR 接低电平时逆时针移存，接高电平时顺时针移存
 *		本例中BL接高电平
 */
 
#include "HV57708.h"
#include "PinDef.h"
#include "delay.h"

/**
 * void HV57708_Init(bool HV57708_Mode)
 * 简介：HV57708 初始化
 * 功能：HV57708 初始化，并设置输出模式
 * 输入：
 *       HV57708_Mode:
 *           HV57708_MODE_NORMAL    - 标准输出模式
 *           HV57708_MODE_INVERTION - 反相输出模式
 * 输出：无
 */
void HV57708_Init(unsigned char HV57708_Mode)
{
	
	 HV57708_CLK_D = 1;
   HV57708_LE_D  = 1;
   HV57708_POL_D = 1;
   HV57708_DI1_D = 1;
   HV57708_DI2_D = 1;
   HV57708_DI3_D = 1;
   HV57708_DI4_D = 1;//方向寄存器设置为输出
   	 
	 HV57708_CLK = 0;
   HV57708_LE  = 1;//LE平时为高，注意数据手册中在描述LE时序时，LE上有一横线表示取反
   HV57708_POL = HV57708_Mode;
   HV57708_DI1 = 0;
   HV57708_DI2 = 0;
   HV57708_DI3 = 0;
   HV57708_DI4 = 0;
}

/**
 * void HV57708_SendData(uint32_t datapart1, uint32_t datapart2)
 * 简介：向 HV57708 发送数据
 * 功能：向 HV57708 发送 64 位的数据,从后向前依次按位写入，因为数据手册上的功能框图中，HVOUT1对应SR1的第16位而不是第1位
 * 输入：
 *       datapart1 - 第一部分数据，32位
 *       datapart2 - 第二部分数据，32位
 * 输出：无
 */
void HV57708_SendData(unsigned long datapart1, unsigned long datapart2)
{
	unsigned int i = 0;
	unsigned long tmp = 0;
	tmp = datapart2;
  for(i = 0; i < 8; i++)
	{
	  
	  HV57708_CLK = 0;
	  
	  if (tmp & 0x00000001)
            HV57708_DI4 = 1;
        else
            HV57708_DI4 = 0;
        tmp = tmp >> 1;
        if (tmp & 0x00000001)
            HV57708_DI3 = 1;
        else
           HV57708_DI3 = 0;
        tmp = tmp >> 1;
        if (tmp & 0x00000001)
            HV57708_DI2 = 1;
        else
           HV57708_DI2 = 0;
        tmp = tmp >> 1;
        if (tmp & 0x00000001)
            HV57708_DI1 = 1;
        else
            HV57708_DI1 = 0;
        tmp = tmp >> 1;
			HV57708_delay();//延时函数的时长只要大于62ns即可，不能过长，不然在写寄存器期间HVOUT会输出乱码
		HV57708_CLK = 1;
		HV57708_delay();

	

	}
	tmp = datapart1;
  for(i = 0; i < 8; i++)
	{
	
		HV57708_CLK = 0;
		
 		if (tmp & 0x00000001)
            HV57708_DI4 = 1;
        else
            HV57708_DI4 = 0;
        tmp = tmp >> 1;
        if (tmp & 0x00000001)
            HV57708_DI3 = 1;
        else
           HV57708_DI3 = 0;
        tmp = tmp >> 1;
        if (tmp & 0x00000001)
            HV57708_DI2 = 1;
        else
           HV57708_DI2 = 0;
        tmp = tmp >> 1;
        if (tmp & 0x00000001)
            HV57708_DI1 = 1;
        else
            HV57708_DI1 = 0;
        tmp = tmp >> 1;
        HV57708_delay();
		HV57708_CLK = 1;
		HV57708_delay();


	}
}

/**
 * void HV57708_OutputData(void)
 * 简介：将 HV57708 寄存器中的数据输出至引脚
 * 功能：将 HV57708 寄存器中的数据输出至引脚
 * 输入：无
 * 输出：无
 */
void HV57708_OutputData(void)
{
	

	HV57708_LE  = 0;
	HV57708_delay();
	HV57708_LE  = 1;
	
	HV57708_delay();//lE由高到低时寄存器中的数据输出至引脚
}

/**
  * HV57708_Display(unsigned char *data)
  * 简介： 将存储时间信息的数组data显示在辉光管上
  * 输入： 存储时间信息的数组，数字大小为6.每个元素为0-9之间的数字
  * 输出； 无
*/
void HV57708_Display(unsigned char *data)
{

    unsigned long part2 = 0; 
    unsigned long part1 = 0;
    unsigned long pos[6];
    byte  i;

    for (i = 0; i < 6; i++)
    {
    
        if (*(data+i) == 0)
            pos[i] = 0x004;//如果为0第10位为1，共12位，前10位有效
        else
            pos[i] = 0x800>>(*(data+i) - 1);//数字n对应第n位为1，共12位，前10位有效
    }

 	part1 = (pos[0]<<20) | (pos[1]<<10) | (pos[2]) |( pos[3]>>10);//前4个数移位并按位相加得到前32位数
    part2 = (pos[3]<<22) | (pos[4]<<12) | (pos[5]<<2);//后3个数移位并按位相加得到后32位数

    HV57708_SendData(part1, part2);
  
    HV57708_OutputData();
}

/*
  * void HV57708_Protection(void)
  * 简介： 阴极保护，辉光管循环显示0-9
  * 功能： 辉光管循环显示0-9
  */
void HV57708_Protection(void)
{
	byte i=0;
	byte j=0;
	unsigned char data_scan[6]={0,0,0,0,0,0};
    for (i = 0; i < 10; i++)
    {
        for(j=0;j<6;j++)
        {
        data_scan[j]=i;	
        }
        HV57708_Display(data_scan);
        delay();
  		delay();
        delay();
  		delay();
    }

}
/*
  * void HV57708_Protection(void)
  * 简介： 阴极保护，辉光管循环显示0-9
  * 功能： 辉光管循环显示0-9
  */
void HV57708_5201314(void)
{
	byte j=0;
	unsigned char data_scan[6]={0,0,0,0,0,0};

    for(j=0;j<6;j++)
    {
    data_scan[j]=5;	
    }
    HV57708_Display(data_scan);
    delay();
	delay();
    delay();
	delay();
	delay();
	delay();
    delay();
	delay();
	for(j=0;j<6;j++)
    {
    data_scan[j]=2;	
    }
    HV57708_Display(data_scan);
    delay();
	delay();
    delay();
	delay();
	delay();
	delay();
    delay();
	delay();
	for(j=0;j<6;j++)
    {
    data_scan[j]=0;	
    }
    HV57708_Display(data_scan);
    delay();
	delay();
    delay();
	delay();
	delay();
	delay();
    delay();
	delay();
	for(j=0;j<6;j++)
    {
    data_scan[j]=1;	
    }
    HV57708_Display(data_scan);
    delay();
	delay();
    delay();
	delay();
	delay();
	delay();
    delay();
	delay();
	for(j=0;j<6;j++)
    {
    data_scan[j]=3;	
    }
    HV57708_Display(data_scan);
    delay();
	delay();
    delay();
	delay();
	delay();
	delay();
    delay();
	delay();
	for(j=0;j<6;j++)
    {
    data_scan[j]=1;	
    }
    HV57708_Display(data_scan);
    delay();
	delay();
    delay();
	delay();
	delay();
	delay();
    delay();
	delay();
	for(j=0;j<6;j++)
    {
    data_scan[j]=4;	
    }
    HV57708_Display(data_scan);
    delay();
	delay();
    delay();
	delay();
	delay();
	delay();
    delay();
	delay();
    

}