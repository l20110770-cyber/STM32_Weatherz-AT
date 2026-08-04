#include "BSP_ST7735S.h"

static volatile uint8_t DMA_TxFlag=0;
//static volatile uint8_t TE_Flag=0;

static void SPI_WaitForSend(void)
{
    while(SPI_I2S_GetFlagStatus (LCD_SPI ,SPI_I2S_FLAG_TXE)==RESET);
}

void BSP_LCD_SPI_Init(void)
{
    RCC_APB2PeriphClockCmd (LCD_SPI_GPIO_CLK ,ENABLE );
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO ,ENABLE );
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB ,ENABLE);
    RCC_APB2PeriphClockCmd (LCD_SPI_CLK ,ENABLE );
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1 ,ENABLE );
    
//    GPIO_PinRemapConfig (GPIO_Remap_SWJ_JTAGDisable ,ENABLE );
    
    
    /* SPI引脚配置 SCK/MOSI 复用推挽 */
    GPIO_InitTypeDef GPIO_InitStucture;
    GPIO_InitStucture.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStucture.GPIO_Pin=LCD_SCK_PIN | LCD_MOSI_PIN ;
    GPIO_InitStucture.GPIO_Speed=GPIO_Speed_50MHz ;
    GPIO_Init (LCD_SCK_PORT ,&GPIO_InitStucture);
    
    /* 控制引脚配置 推挽输出 */
    GPIO_InitStucture.GPIO_Pin =LCD_RES_PIN | LCD_CS_PIN | LCD_DC_PIN | LCD_BLK_PIN ;
    GPIO_InitStucture.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStucture.GPIO_Speed=GPIO_Speed_50MHz ;
    GPIO_Init(LCD_RES_PORT ,&GPIO_InitStucture );
    
    /* TE中断引脚配置 下拉输入 */
//    GPIO_InitStucture .GPIO_Pin =LCD_TE_PIN ;
//    GPIO_InitStucture .GPIO_Mode =GPIO_Mode_IPD ;
//    GPIO_Init (LCD_TE_PORT ,&GPIO_InitStucture );
    
    
    /* SPI主模式配置 */
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction=SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
    
    SPI_Init (LCD_SPI ,&SPI_InitStructure);
    SPI_Cmd(LCD_SPI ,ENABLE );
    
    /* DMA TX通道配置 SPI1_TX对应DMA1通道3 */
    DMA_DeInit (DMA1_Channel3);
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t )&LCD_SPI->DR ;
    DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t )0;  //传送时再指定
    DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize=0xFF;
    DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable ;
    DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte ;
    DMA_InitStructure.DMA_MemoryDataSize =DMA_MemoryDataSize_Byte ;
    DMA_InitStructure.DMA_Mode =DMA_Mode_Normal ;
    DMA_InitStructure.DMA_Priority =DMA_Priority_High ;
    DMA_InitStructure.DMA_M2M=DMA_M2M_Disable ;
    DMA_Init (DMA1_Channel3 ,&DMA_InitStructure);
    DMA_Cmd (DMA1_Channel3 ,ENABLE );
    
    DMA_ITConfig (DMA1_Channel3 ,DMA_IT_TC,ENABLE );
    
    /* DMA中断配置 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel =DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    
    /*TE外部中断配置*/
//    GPIO_EXTILineConfig (GPIO_PortSourceGPIOB ,GPIO_PinSource4 );
//    EXTI_InitTypeDef EXTI_InitStructure;
//    EXTI_InitStructure.EXTI_Line=LCD_TE_EXTI_LINE;
//    EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
//    EXTI_InitStructure.EXTI_LineCmd=ENABLE;
//    EXTI_Init(&EXTI_InitStructure);
//    
//    NVIC_InitStructure.NVIC_IRQChannel=LCD_TE_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//    
    LCD_CS_HIGH ();
    LCD_RES_HIGH ();
    LCD_BLK_OFF ();
    
}

void BSP_LCD_SPI_SendByte(uint8_t Byte)
{
    SPI_WaitForSend ();
    SPI_I2S_SendData (LCD_SPI ,Byte);
	while(SPI_I2S_GetFlagStatus (LCD_SPI,SPI_I2S_FLAG_BSY)==SET);
}

void BSP_LCD_SPI_SendBuffer(const uint8_t *Buf,uint32_t Length)
{
    while(Length--)
    {
        SPI_WaitForSend ();
        SPI_I2S_SendData (LCD_SPI ,*Buf++);
    }
	while(SPI_I2S_GetFlagStatus (LCD_SPI ,SPI_I2S_FLAG_BSY )==SET);
}

void BSP_LCD_SPI_DMA_Send(const uint8_t *Buf,uint32_t Length)
{
    DMA_TxFlag =0;
    DMA_Cmd (DMA1_Channel3 ,DISABLE );
    DMA1_Channel3->CMAR = (uint32_t )Buf;
    DMA1_Channel3->CNDTR = Length ;
    SPI_I2S_DMACmd (LCD_SPI ,SPI_I2S_DMAReq_Tx,ENABLE );
    DMA_Cmd (DMA1_Channel3 ,ENABLE );
}    

uint8_t BSP_DMA_GetFlag(void)
{
    return DMA_TxFlag ;
}

void BSP_LCD_DMA_ClearFlag(void)
{
    DMA_TxFlag =0;
}

//uint8_t BSP_LCD_TE_GetFlag(void)
//{
//    return TE_Flag;
//}

//void BSP_LCD_TE_ClearFlag(void)
//{
//    TE_Flag=0;
//}

void DMA1_Channel3_IRQHandler(void)
{
    if(DMA_GetITStatus (DMA1_IT_TC3)==SET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC3);
        SPI_I2S_DMACmd(LCD_SPI,SPI_I2S_DMAReq_Tx,DISABLE);
        LCD_CS_HIGH ();
		DMA_TxFlag =1;
		
    }
}

//void EXTI4_IRQHandler(void)
//{
//    if(EXTI_GetFlagStatus(LCD_TE_EXTI_LINE)==SET)
//    {
//        EXTI_ClearITPendingBit(LCD_TE_EXTI_LINE);
//        TE_Flag=1;
//    }
//}













