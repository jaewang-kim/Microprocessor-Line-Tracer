#include "..//tm4c123gh6pm.h"
#include "TExaS2.h"
#include "IR_sensor.h"
#include "DC_motor.h"
#include "sonic.h"

unsigned long pre_data = 0x00, sensor_data = 0x00, left, right, mid, p = 0x00, Threshold = 0x500, th = 0x00, co = 0x00;    // 12-bit 0 to 4095 sample

void ADC0Seq0_Handler(void){
	left = ADC0_SSFIFO0_R&0xFFF;   // ���� IR������ ���� �����´�
  ADC0_ISC_R |= 0x0007;             	// IRS clear
	ADC0_PSSI_R = 0x0007;							//sampling �����
}

void ADC0Seq1_Handler(void){
	right = ADC0_SSFIFO1_R&0xFFF;   // ������ IR������ ���� �����´�
}

void ADC0Seq2_Handler(void){
	mid = ADC0_SSFIFO2_R&0xFFF;   //��� IR������ ���� �����´�
}

void update_sensor(void){			//�Ӱ谪�� �����Ͽ� �ش� ���� ���� ���� ����
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
		pre_data = sensor_data;									//��ȭ�� �� ����
		Change_direction(sensor_data);			//��ȭ�� ���� ����  ��ȯ
		while(1){																					//������ ��ȭ ���
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