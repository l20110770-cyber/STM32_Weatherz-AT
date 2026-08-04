#include "stm32f10x.h"                  // Device header

static volatile uint32_t sys_tick = 0;

void TIM2_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);   /* TIM2挂在APB1总线, 必须用APB1PeriphClockCmd使能, 否则时钟永远不会打开 */
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_Prescaler=71;
    TIM_TimeBaseInitStructure.TIM_Period=999;
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
    
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM2,ENABLE);
}

uint32_t GetSysTick(void)
{
    uint32_t tmp;
    __disable_irq();
    tmp=sys_tick;
    __enable_irq();
    return tmp;
}

void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus (TIM2,TIM_IT_Update)==SET)
    {
        sys_tick ++;
        TIM_ClearITPendingBit (TIM2,TIM_IT_Update);
    }
}
