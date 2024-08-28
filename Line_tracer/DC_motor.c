#include "TExaS.h"
#include "DC_motor.h"
#include "..//tm4c123gh6pm.h"

unsigned long H, L, Val = 0x00, Wave, turn = 0x00;

void DC_Motor_Init(void){   //PA5 = 오른쪽 바퀴, PA6 = 왼쪽 바퀴
   SYSCTL_RCGC2_R |= 0x00000001; 																														// port A clock 활성화
   H = 60000;
	 L = 20000;
   GPIO_PORTA_AMSEL_R &= ~0x6C;																													  // analog 비활성화
   GPIO_PORTA_PCTL_R &= ~0x00F00000;																									  // PCTL 사용 X
   GPIO_PORTA_DIR_R |= 0x6C;																																	  // output으로 설정
   GPIO_PORTA_DR8R_R |= 0x6C;																															  // enable 8 mA drive on PA5, PA6
   GPIO_PORTA_AFSEL_R &= ~0x6C; 																												  	// analog function 비활성화
   GPIO_PORTA_DEN_R |= 0x6C; 																																	// digital pin으로 설정
   GPIO_PORTA_DATA_R &= ~0x6C; 																														// 초기값 = 0
   NVIC_ST_CTRL_R = 0; 																																							// NVIC 초기화
   NVIC_ST_RELOAD_R = H-1;																																				// Reload 초기화
   NVIC_ST_CURRENT_R = 0;																																				// Current 초기화
   NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x00000000;					// priority 0
   NVIC_ST_CTRL_R = 0x00000007;																																// interrupts 활성화
}

//오른쪽 바퀴 PA5(0X20), 왼쪽 바퀴 PA6(0X40)
void SysTick_Handler(void){
	if (turn == 0x00){
		H  = 50000;					//좌측 바퀴의 토크가 부족하여 H를 조정
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
		Val = 0;		//왼쪽과 가운데
	}
	else if ((data & 0x07) == 0x01|| (data & 0x07) == 0x03){
		Val = 1;		//오른쪽과 가운데 
	}
	
	if((data & 0x07) == 0x04 || (data & 0x07) == 0x06 || ((data & 0x07) == 0x02 && Val == 1)){
		turn = 0x00;		//좌회전
	}                   
   else if ((data & 0x07) == 0x01|| (data & 0x07) == 0x03 || ((data & 0x07) == 0x02 && Val == 0)){
		turn = 0x01;		//우회전
   }
	 else if(Val == 2){
		 turn = 0x02;		//직진
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