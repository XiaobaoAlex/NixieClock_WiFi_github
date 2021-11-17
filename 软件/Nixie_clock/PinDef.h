/**
 * ���Ŷ���
 * ���ߣ�    XiaobaoAlex
 * �����£�2021-7-22
 */
#ifndef __PINDEF_H
#define __PINDEF_H

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

/**
 * HV57708 ���Ŷ���
 */
#define HV57708_CLK PTAD_PTAD0 
#define HV57708_LE  PTAD_PTAD2
#define HV57708_POL PTAD_PTAD1
#define HV57708_DI1 PTAD_PTAD6
#define HV57708_DI2 PTAD_PTAD5
#define HV57708_DI3 PTAD_PTAD4
#define HV57708_DI4 PTAD_PTAD3

#define HV57708_CLK_D PTADD_PTADD0 
#define HV57708_LE_D  PTADD_PTADD2
#define HV57708_POL_D PTADD_PTADD1
#define HV57708_DI1_D PTADD_PTADD6
#define HV57708_DI2_D PTADD_PTADD5
#define HV57708_DI3_D PTADD_PTADD4
#define HV57708_DI4_D PTADD_PTADD3


/**
 * �ʵ����Ŷ���
 */
#define NEON_DOT0_D PTBDD_PTBDD1
#define NEON_DOT1_D PTBDD_PTBDD2
#define NEON_DOT2_D PTBDD_PTBDD3
#define NEON_DOT3_D PTBDD_PTBDD4 
 
#define NEON_DOT0 PTBD_PTBD1
#define NEON_DOT1 PTBD_PTBD2
#define NEON_DOT2 PTBD_PTBD3
#define NEON_DOT3 PTBD_PTBD4


/**
 * ESP8266���Ŷ���
 */
#define GPIO_0 PTAD_PTAD7
#define GPIO_0D PTADD_PTADD7
#define RESET_ESP PTBD_PTBD0
#define RESET_ESPD PTBDD_PTBDD0
/**
 * ���뿪�����Ŷ���
 */
#define JUMP1 PTDD_PTDD0
#define JUMP1_D PTDDD_PTDDD0


/**
 * ��ƽת��оƬ���Ŷ���
 */
#define OETXS	PTDD_PTDD1//��ƽת��оƬʹ������
#define OETXS_D	PTDDD_PTDDD1

/**
 * �̵���BOOST_SWITCH���Ŷ���
 */
#define BOOST_SWITCH	PTBD_PTBD5//��ƽת��оƬʹ������
#define BOOST_SWITCH_D	PTBDD_PTBDD5
#endif