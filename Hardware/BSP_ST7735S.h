#ifndef   __BSP_ST7735S_H
#define   __BSP_ST7735S_H

#include "stm32f10x.h"                  // Device header

/* 引脚定义 */

#define LCD_SPI                     SPI1
#define LCD_SPI_CLK                 RCC_APB2Periph_SPI1
#define LCD_SPI_GPIO_CLK            RCC_APB2Periph_GPIOA

/* SPI引脚 */

#define LCD_SCK_PIN                 GPIO_Pin_5
#define LCD_SCK_PORT                GPIOA
#define LCD_MOSI_PIN                GPIO_Pin_7
#define LCD_MOSI_PORT               GPIOA

/* 控制引脚 */

#define LCD_RES_PIN                 GPIO_Pin_5
#define LCD_RES_PORT                GPIOB
#define LCD_DC_PIN                  GPIO_Pin_6
#define LCD_DC_PORT                 GPIOB
#define LCD_CS_PIN                  GPIO_Pin_7
#define LCD_CS_PORT                 GPIOB
#define LCD_BLK_PIN                 GPIO_Pin_8
#define LCD_BLK_PORT                GPIOB

/* TE中断引脚 */

//#define LCD_TE_PIN                  GPIO_Pin_4
//#define LCD_TE_PORT                 GPIOB
//#define LCD_TE_EXTI_LINE            EXTI_Line4
//#define LCD_TE_IRQn                 EXTI4_IRQn

/* 引脚操作 */

#define LCD_RES_LOW()               GPIO_ResetBits(LCD_RES_PORT,LCD_RES_PIN)
#define LCD_RES_HIGH()              GPIO_SetBits(LCD_RES_PORT,LCD_RES_PIN)
#define LCD_DC_LOW()                GPIO_ResetBits(LCD_DC_PORT,LCD_DC_PIN)
#define LCD_DC_HIGH()               GPIO_SetBits(LCD_DC_PORT,LCD_DC_PIN)
#define LCD_CS_LOW()                GPIO_ResetBits(LCD_CS_PORT,LCD_CS_PIN)
#define LCD_CS_HIGH()               GPIO_SetBits(LCD_CS_PORT,LCD_CS_PIN)
#define LCD_BLK_OFF()               GPIO_ResetBits(LCD_BLK_PORT,LCD_BLK_PIN)
#define LCD_BLK_ON()                GPIO_SetBits(LCD_BLK_PORT,LCD_BLK_PIN)

/* 外部调用 */

void BSP_LCD_SPI_Init(void);
void BSP_LCD_SPI_SendByte(uint8_t Byte);
void BSP_LCD_SPI_SendBuffer(const uint8_t *Buf,uint32_t Length);
void BSP_LCD_SPI_DMA_Send(const uint8_t *Buf,uint32_t Length);
uint8_t BSP_DMA_GetFlag(void);
void BSP_LCD_DMA_ClearFlag(void);
//uint8_t BSP_LCD_TE_GetFlag(void);
//void BSP_LCD_TE_ClearFlag(void);

#endif
