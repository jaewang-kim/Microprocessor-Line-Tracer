#include "TExaS.h"
#include "DC_motor.h"
#include "..//tm4c123gh6pm.h"

unsigned long H, L, Val = 0x00, Wave, turn = 0x00;

void DC_Motor_Init(void){   //PA5 = ������ ����, PA6 = ���� ����
   SYSCTL_RCGC2_R |= 0x00000001; 																														// port A clock Ȱ��ȭ
   H = 60000;
	 L = 20000;
   GPIO_PORTA_AMSEL_R &= ~0x6C;																													  // analog ��Ȱ��ȭ
   GPIO_PORTA_PCTL_R &= ~0x00F00000;																									  // PCTL ��� X
   GPIO_PORTA_DIR_R |= 0x6C;																																	  // output���� ����
   GPIO_PORTA_DR8R_R |= 0x6C;																															  // enable 8 mA drive on PA5, PA6
   GPIO_PORTA_AFSEL_R &= ~0x6C; 																												  	// analog function ��Ȱ��ȭ
   GPIO_PORTA_DEN_R |= 0x6C; 																																	// digital pin���� ����
   GPIO_PORTA_DATA_R &= ~0x6C; 																														// �ʱⰪ = 0
   NVIC_ST_CTRL_R = 0; 																																							// NVIC �ʱ�ȭ
   NVIC_ST_RELOAD_R = H-1;																																				// Reload �ʱ�ȭ
   NVIC_ST_CURRENT_R = 0;																																				// Current �ʱ�ȭ
   NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x00000000;					// priority 0
   NVIC_ST_CTRL_R = 0x00000007;																																// interrupts Ȱ��ȭ
}

//������ ���� PA5(0X20), ���� ���� PA6(0X40)
void SysTick_Handler(void){
	if (turn == 0x00){
		H  = 50000;					//���� ������ ��ũ�� �����Ͽ� H�� ����
		L = 80000 - H;
		GPIO_PORTA_DATA_R |= 0x20;
		if((GPIO_PORTA_DATA_R & 0x40) == 0x40){
		 GPIO_PORTA_DATA_R &= ~0x40;
		 NVIC_ST_RELOAD_R = L-1;
   } else{
			GPIO_PORTA_DATA_R |= 0X40;
			NVIC_ST_RELOAD_R = H-1;
   }
	}
	else if (turn == 0x01){
		H  = 60000;
		L = 80000 - H;
		GPIO_PORTA_DATA_R |= 0x40;
		if((GPIO_PORTA_DATA_R & 0x20) == 0x20){
		 GPIO_PORTA_DATA_R &= ~0x20;
		 NVIC_ST_RELOAD_R = L-1;
   } else{
			GPIO_PORTA_DATA_R |= 0X20;
			NVIC_ST_RELOAD_R = H-1;
   }
	}
	else if (turn == 0x02){
		GPIO_PORTA_DATA_R |= 0x60;
	}
}

void Change_direction(unsigned long data){   //3bit data
	if ((data & 0x07) == 0x04 || (data & 0x07) == 0x06){
		Val = 0;		//���ʰ� ���
	}
	else if ((data & 0x07) == 0x01|| (data & 0x07) == 0x03){
		Val = 1;		//�����ʰ� ��� 
	}
	
	if((data & 0x07) == 0x04 || (data & 0x07) == 0x06 || ((data & 0x07) == 0x02 && Val == 1)){
		turn = 0x00;		//��ȸ��
	}                   
   else if ((data & 0x07) == 0x01|| (data & 0x07) == 0x03 || ((data & 0x07) == 0x02 && Val == 0)){
		turn = 0x01;		//��ȸ��
   }
	 else if(Val == 2){
		 turn = 0x02;		//����
	 }
	 
	  if((data & 0x07) == 0x02){
			Val = 2;
		}
}

void Motor_stop(void){
	GPIO_PORTA_DATA_R = 0x00;
	NVIC_ST_RELOAD_R = 0;
	NVIC_ST_CTRL_R = 0x00;
}

void Motor_start(void){
	NVIC_ST_RELOAD_R = H;
	NVIC_ST_CTRL_R = 0x07;
}