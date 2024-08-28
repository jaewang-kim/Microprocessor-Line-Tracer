#include "IR_sensor.h"
#include "..//tm4c123gh6pm.h"

void IR_sensor_Init(void){ 																																								//PE0, 1, 2 �ʱ�ȭ
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;   																															//Port E�� clock ���
  delay = SYSCTL_RCGC2_R;         																																	//����ȭ
  GPIO_PORTE_DIR_R &= ~0x07;      																																//PE0, 1, 2 �� analog�� input
  GPIO_PORTE_AFSEL_R |= 0x07;     																																//alternate function ���
  GPIO_PORTE_DEN_R &= ~0x07;      																																//digital ��Ȱ��ȭ
  GPIO_PORTE_AMSEL_R |= 0x07;     																															//analog function ���
  SYSCTL_RCGC0_R |= 0x00010000;  																																//ADC0 Ȱ��ȭ
  delay = SYSCTL_RCGC2_R;         
  SYSCTL_RCGC0_R &= ~0x00000300;																															//125K samples/second
  ADC0_SSPRI_R = 0x0213;          																																			//ADC0�� Sequence priority ����
  ADC0_ACTSS_R &= ~0x0007;
  ADC0_EMUX_R &= ~0x0FFF;         																																	//sequence���� software trigger
	
	ADC0_SSMUX0_R = (ADC0_SSMUX0_R&0xFFFFFFF0)|0x03;														//Ain3 (PE0) ����	left
  ADC0_SSCTL0_R = 0x0004;         																																		//no TS0 D0 END0, yes IE0
	
  ADC0_SSMUX1_R = (ADC0_SSMUX1_R&0xFFFFFFF0)|0x02;														//Ain2 (PE1) ���� right
  ADC0_SSCTL1_R = 0x0004;         																																		//no TS0 D0 END0, yes IE0

  ADC0_SSMUX2_R = (ADC0_SSMUX2_R&0xFFFFFFF0)|0x01; 														//Ain1 (PE2) ����	mid
  ADC0_SSCTL2_R = 0x0004;         																																		//no TS0 D0 END0, yes IE0
	
	ADC0_IM_R |= 0x00000007;																																						//ADC0 intterrupt ���
	ADC0_CTL_R |= 0x07;																																										//
	NVIC_PRI3_R |= (NVIC_PRI3_R&0x0000FFFF)|0x40400000;															//sample sequence 0, 1�� priority ����
	NVIC_PRI4_R |= (NVIC_PRI4_R&0xFFFFFFFF)|0x00000040;															//sample sequence 2�� priority ����
	NVIC_EN0_R |= 0x0001C000;																																					//sample sequence 0, 1, 2�� interrupt enable
	
  ADC0_ACTSS_R |= 0x0007;         																																		//enable sample sequencer 0, 1, 2
	ADC0_PSSI_R = 0x0007;																																									//sampling ����
}