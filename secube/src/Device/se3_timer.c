
#include "se3_timer.h"


TIM_HandleTypeDef timer_Struct;

void count_up_to(int n){
    se3_write_trace(se3_debug_create_string("\n[se3_core_mio] Timer init...\0"), debug_address++);

	TIM_Base_InitTypeDef    TIM_TimeBaseStructure;
	TIM_ClockConfigTypeDef clock_source;

	  __HAL_RCC_TIM2_CLK_ENABLE();

	  clock_source.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  clock_source.ClockPrescaler = TIM_CLOCKPRESCALER_DIV8;
	  clock_source.ClockPolarity = TIM_CLOCKPOLARITY_RISING;

	  HAL_TIM_ConfigClockSource(&timer_Struct, NULL);

	  TIM_TimeBaseStructure.Period = 0x7F;
	  TIM_TimeBaseStructure.Prescaler = 0xFFF;
	  TIM_TimeBaseStructure.ClockDivision = 0x0;
	  TIM_TimeBaseStructure.CounterMode = TIM_COUNTERMODE_UP;

	  timer_Struct.Instance = TIM2;
	  timer_Struct.Init = TIM_TimeBaseStructure;
	  timer_Struct.Channel = HAL_TIM_ACTIVE_CHANNEL_2;
	  timer_Struct.Lock = HAL_UNLOCKED;
	  HAL_TIM_Base_Init(&timer_Struct);

	  //__HAL_TIM_ENABLE_IT(&timer_Struct, TIM_IT_TRIGGER);

	  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);

	  /* Enable interrupt and peripheral */
	  HAL_TIM_Base_Start_IT(&timer_Struct);


	  return;
}
