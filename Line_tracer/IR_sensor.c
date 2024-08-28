#include "IR_sensor.h"
#include "..//tm4c123gh6pm.h"

void IR_sensor_Init(void){ 																																								//PE0, 1, 2 초기화
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;   																															//Port E의 clock 사용
  delay = SYSCTL_RCGC2_R;         																																	//안정화
  GPIO_PORTE_DIR_R &= ~0x07;      																																//PE0, 1, 2 를 analog의 input
  GPIO_PORTE_AFSEL_R |= 0x07;     																																//alternate function 사용
  GPIO_PORTE_DEN_R &= ~0x07;      																																//digital 비활성화
  GPIO_PORTE_AMSEL_R |= 0x07;     																															//analog function 사용
  SYSCTL_RCGC0_R |= 0x00010000;  																																//ADC0 활성화
  delay = SYSCTL_RCGC2_R;         
  SYSCTL_RCGC0_R &= ~0x00000300;																															//125K samples/second
  ADC0_SSPRI_R = 0x0213;          																																			//ADC0의 Sequence priority 설정
  ADC0_ACTSS_R &= ~0x0007;
  ADC0_EMUX_R &= ~0x0FFF;         																																	//sequence들의 software trigger
	
	ADC0_SSMUX0_R = (ADC0_SSMUX0_R&0xFFFFFFF0)|0x03;														//Ain3 (PE0) 설정	left
  ADC0_SSCTL0_R = 0x0004;         																																		//no TS0 D0 END0, yes IE0
	
  ADC0_SSMUX1_R = (ADC0_SSMUX1_R&0xFFFFFFF0)|0x02;														//Ain2 (PE1) 설정 right
  ADC0_SSCTL1_R = 0x0004;         																																		//no TS0 D0 END0, yes IE0

  ADC0_SSMUX2_R = (ADC0_SSMUX2_R&0xFFFFFFF0)|0x01; 														//Ain1 (PE2) 설정	mid
  ADC0_SSCTL2_R = 0x0004;         																																		//no TS0 D0 END0, yes IE0
	
	ADC0_IM_R |= 0x00000007;																																						//ADC0 intterrupt 사용
	ADC0_CTL_R |= 0x07;																																										//
	NVIC_PRI3_R |= (NVIC_PRI3_R&0x0000FFFF)|0x40400000;															//sample sequence 0, 1의 priority 설정
	NVIC_PRI4_R |= (NVIC_PRI4_R&0xFFFFFFFF)|0x00000040;															//sample sequence 2의 priority 설정
	NVIC_EN0_R |= 0x0001C000;																																					//sample sequence 0, 1, 2의 interrupt enable
	
  ADC0_ACTSS_R |= 0x0007;         																																		//enable sample sequencer 0, 1, 2
	ADC0_PSSI_R = 0x0007;																																									//sampling 시작
}