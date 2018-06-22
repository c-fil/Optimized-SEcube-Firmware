
#include "se3_timer.h"


TIM_HandleTypeDef timer_Struct;

void count_up_to(int n){
	MYPRINTF("We are in 'count up to' function...\0", BASE_DEBUG_ADDRESS + (debug_count++));

	TIM_Base_InitTypeDef    TIM_TimeBaseStructure;
	TIM_ClockConfigTypeDef clock_source;

	  TIM_TimeBaseStructure.Period = 0x7F;
	  TIM_TimeBaseStructure.Prescaler = 0x0FFF;
	  TIM_TimeBaseStructure.ClockDivision = 0x0;
	  TIM_TimeBaseStructure.CounterMode = TIM_COUNTERMODE_UP;

	  timer_Struct.Instance = TIM2;
	  timer_Struct.Init = TIM_TimeBaseStructure;
	  timer_Struct.Channel = HAL_TIM_ACTIVE_CHANNEL_2;
	  timer_Struct.Lock = HAL_UNLOCKED;
	  HAL_TIM_Base_Init(&timer_Struct);

	  clock_source.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  clock_source.ClockPrescaler = TIM_CLOCKPRESCALER_DIV8;
	  clock_source.ClockPolarity = TIM_CLOCKPOLARITY_RISING;

	  HAL_TIM_ConfigClockSource(&timer_Struct, NULL);

	  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);


	  /* Enable interrupt and peripheral */
	  HAL_TIM_Base_Start_IT(&timer_Struct);
}
