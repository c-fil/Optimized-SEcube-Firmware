#include "se3_cmd1_crypto.h"
#include "stm32f4xx_hal_def.h"

TIM_HandleTypeDef timer_Struct;

void count_up_to(int n){
	TIM_Base_InitTypeDef    TIM_TimeBaseStructure;

	  TIM_TimeBaseStructure.Period = 0xFF;
	  TIM_TimeBaseStructure.Prescaler = 0x0FFF;
	  TIM_TimeBaseStructure.ClockDivision = 0x0;
	  TIM_TimeBaseStructure.CounterMode = TIM_COUNTERMODE_UP;


	  timer_Struct.Instance = TIM2;
	  timer_Struct.Init = TIM_TimeBaseStructure;
	  timer_Struct.Channel = HAL_TIM_ACTIVE_CHANNEL_2;
	  timer_Struct.Lock = HAL_UNLOCKED;

	  HAL_TIM_Base_Init(&timer_Struct);

	  /* Enable interrupt and peripheral */
	  HAL_TIM_Base_Start_IT(&timer_Struct);
}
