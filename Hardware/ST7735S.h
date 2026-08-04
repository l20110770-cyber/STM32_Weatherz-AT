#ifndef   __ST7735S_H
#define   __ST7735S_H

#include "stm32f10x.h"                  // Device header

#define LCD_WIDTH               128
#define LCD_HEIGHT              128

#define WHITE                   0xFFFF
#define BLACK                   0x0000
#define RED                     0xF800
#define GREEN                   0x07E0
#define BLUE                    0x001F
#define YELLOW					0xFFE0
#define CYAN					0x07FF		//青色

#define ST7735_NOP              0x00
#define ST7735_SWRESET          0x01
#define ST7735_SLPIN            0x10
#define ST7735_SLPOUT           0x11
#define ST7735_PTLON            0x12
#define ST7735_NORON            0x13
#define ST7735_INVOFF           0x20
#define ST7735_INVON            0x21
#define ST7735_DISPOFF          0x28
#define ST7735_DISPON           0x29
#define ST7735_CASET            0x2A    /* 列地址设置 */
#define ST7735_RASET            0x2B    /* 行地址设置 */
#define ST7735_RAMWR            0x2C    /* 显存写入 */
#define ST7735_MADCTL           0x36    /* 显存访问控制 */
#define ST7735_COLMOD           0x3A    /* 像素格式 */
#define ST7735_FRMCTR1          0xB1
#define ST7735_FRMCTR2          0xB2
#define ST7735_FRMCTR3          0xB3
#define ST7735_INVCTR           0xB4
#define ST7735_PWCTR1           0xC0
#define ST7735_PWCTR2           0xC1
#define ST7735_PWCTR3           0xC2
#define ST7735_PWCTR4           0xC3
#define ST7735_PWCTR5           0xC4
#define ST7735_VMCTR1           0xC5
#define ST7735_GMCTRP1          0xE0
#define ST7735_GMCTRN1          0xE1

/* MADCTL参数 屏幕方向 */
#define MADCTL_MY               0x80    /*Y镜像*/
#define MADCTL_MX               0x40    /*X镜像*/
#define MADCTL_MV               0x20    /*XY镜像*/
#define MADCTL_ML               0x10    
#define MADCTL_RGB              0x00    /*RGB顺序*/
#define MADCTL_BGR              0x08    /*BGR顺序*/

void ST7735S_Init(void);
void ST7735S_HardReset(void);
void ST7735S_SetWindow(uint8_t X0,uint8_t Y0,uint8_t X1,uint8_t Y1);
void ST7735S_FillColor(uint16_t Color);
void ST7735S_FillRectangel(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,uint16_t Color);
void ST7735S_DrawPoint(uint8_t X,uint8_t Y,uint16_t Color);

void ST7735S_ShowImage(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,const uint16_t *Image);
void ST7735S_DMA_ShowImage(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,const uint16_t *Image);

void ST7735S_DrawChar(uint8_t X,uint8_t Y,char ch,uint16_t Color,uint16_t BG_Color);
void ST7735S_DrawString(uint8_t X,uint8_t Y,char *Str,uint16_t Color,uint16_t BG_Color);

void ST7735S_DrawChar_16X32(uint8_t X,uint8_t Y,char ch,uint16_t Color,uint16_t BG_Color);
void ST7735S_DrawString_16X32(uint8_t X,uint8_t Y,char *Str,uint16_t Color,uint16_t BG_Color);

void ST7735S_DrawChinese(uint8_t X,uint8_t Y,const uint8_t *Font_Data,uint16_t Color,uint16_t BG_Color);
void ST7735S_DrawString_Mix(uint8_t X,uint8_t Y,char *Str,uint16_t Color,uint16_t BG_Color);

//void ST7735S_WaitTE(void);
void ST7735S_BackLight(uint8_t State);


#endif
