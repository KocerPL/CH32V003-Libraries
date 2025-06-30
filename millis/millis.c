#include "ch32v003_millis.h"
uint64_t curMillis =0;
void Millis_Init(int16_t periodCorrection)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_TimeBaseInitStructure.TIM_Period = (SystemCoreClock/1000)-1 +periodCorrection;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel= TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2,ENABLE);


}
uint64_t millis()
{
    return curMillis;
}
uint64_t seconds()
{
    return curMillis/1000;
}
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM2_IRQHandler(void)
{
      if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
      {
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
        curMillis++;
        
      }
}