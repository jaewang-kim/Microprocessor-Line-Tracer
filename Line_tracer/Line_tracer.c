#include "..//tm4c123gh6pm.h"
#include "TExaS2.h"
#include "IR_sensor.h"
#include "DC_motor.h"
#include "sonic.h"

unsigned long pre_data = 0x00, sensor_data = 0x00, left, right, mid, p = 0x00, Threshold = 0x500, th = 0x00, co = 0x00;    // 12-bit 0 to 4095 sample

void ADC0Seq0_Handler(void){
	left = ADC0_SSFIFO0_R&0xFFF;   // 왼쪽 IR센서의 값을 가져온다
  ADC0_ISC_R |= 0x0007;             	// IRS clear
	ADC0_PSSI_R = 0x0007;							//sampling 재시작
}

void ADC0Seq1_Handler(void){
	right = ADC0_SSFIFO1_R&0xFFF;   // 오른쪽 IR센서의 값을 가져온다
}

void ADC0Seq2_Handler(void){
	mid = ADC0_SSFIFO2_R&0xFFF;   //가운데 IR센서의 값을 가져온다
}

void update_sensor(void){			//임계값을 설정하여 해당 값에 따라 값을 갱신
	if (left > Threshold){
			sensor_data |= 0x04;
		}
		else {
			sensor_data &= ~0x04;
		}
		
		if (right > Threshold){
			sensor_data |= 0x01;
		}
		else {
			sensor_data &= ~0x01;
		}
		
		if (mid > Threshold){
			sensor_data |= 0x02;
		}
		else {
			sensor_data &= ~0x02;
		}
}

int main(void){
	IR_sensor_Init();
	DC_Motor_Init();
	sonic_Init();
	
  while(1){
		pre_data = sensor_data;									//변화된 값 저장
		Change_direction(sensor_data);			//변화에 따른 방향  전환
		while(1){																					//센서의 변화 대기
			if (th < 0xEFFF){
				Motor_stop();
				co = 0x01;
			}
			else {
				if (pre_data != sensor_data || co == 0x01){
					if (co == 0x01){
						Motor_start();
						co = 0x00;
					}
					break;
				}
				else
					update_sensor();
			}
		}
  }
}