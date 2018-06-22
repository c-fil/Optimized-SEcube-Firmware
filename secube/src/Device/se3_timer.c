
#include "se3_timer.h"


TIM_HandleTypeDef timer_Struct;

void count_up_to(int n){
	TIM_Base_InitTypeDef    TIM_TimeBaseStructure;
	TIM_ClockConfigTypeDef clock_source;

	  TIM_TimeBaseStructure.Period = 0xFF;
	  TIM_TimeBaseStructure.Prescaler = 0x0FFF;
	  TIM_TimeBaseStructure.ClockDivision = 0x0;
	  TIM_TimeBaseStructure.CounterMode = TIM_COUNTERMODE_UP;

	  timer_Struct.Instance = TIM2;
	  timer_Struct.Init = TIM_TimeBaseStructure;
	  timer_Struct.Channel = HAL_TIM_ACTIVE_CHANNEL_2;
	  timer_Struct.Lock = HAL_UNLOCKED;

	  clock_source.ClockSource = 0;
	  clock_source.ClockPrescaler = 0;
	  clock_source.ClockPolarity = 0;

	  HAL_TIM_ConfigClockSource(&timer_Struct, NULL);
	  HAL_TIM_Base_Init(&timer_Struct);

	  /* Enable interrupt and peripheral */
	  HAL_TIM_Base_Start_IT(&timer_Struct);
}
