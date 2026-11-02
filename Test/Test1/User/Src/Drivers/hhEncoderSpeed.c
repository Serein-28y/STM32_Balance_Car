#include "tim.h"

void InitAllEncoderAll(){
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
}


int16_t Encoder_Get(uint8_t n){
	int16_t Tmp=0;
	if(n==1){ 
		Tmp=__HAL_TIM_GET_COUNTER(&htim3);
		__HAL_TIM_SET_COUNTER(&htim3,0);
	}else if(n==2){
		Tmp=__HAL_TIM_GET_COUNTER(&htim4);
		__HAL_TIM_SET_COUNTER(&htim4,0);
	}
	return Tmp;
}
