#include "ST7735S.h"
#include "BSP_ST7735S.h"
#include "Delay.h"
#include "ST7735_FONT.h"


static void ST7735S_WriteCmd(uint8_t Cmd)
{
    LCD_CS_LOW ();
    LCD_DC_LOW ();
    BSP_LCD_SPI_SendByte (Cmd);
    LCD_CS_HIGH ();
}

static void ST7735S_WriteData(uint8_t Data)
{
    LCD_CS_LOW ();
    LCD_DC_HIGH ();
    BSP_LCD_SPI_SendByte (Data);
    LCD_CS_HIGH ();
}

static void ST7735S_WriteArr(const uint8_t *Arr,uint32_t Length)
{
    LCD_CS_LOW ();
    LCD_DC_HIGH ();
    BSP_LCD_SPI_SendBuffer (Arr,Length);
    LCD_CS_HIGH ();
}

/* 写16位数据  用于RGB565颜色 */
static void ST7735S_WriteData16(uint16_t Data)
{
    LCD_CS_LOW ();
    LCD_DC_HIGH ();
    BSP_LCD_SPI_SendByte ((uint8_t )(Data>>8));
    BSP_LCD_SPI_SendByte ((uint8_t )(Data&0xFF));
    LCD_CS_HIGH ();
}

void ST7735S_HardReset(void)
{
    LCD_RES_HIGH ();
    Delay_ms(10);
    LCD_RES_LOW ();
    Delay_us(20);
    LCD_RES_HIGH ();
    Delay_ms(120);
}

void ST7735S_Init(void)
{
    ST7735S_HardReset ();
    
    ST7735S_WriteCmd (ST7735_SWRESET);
    Delay_ms(120);
    
    ST7735S_WriteCmd (ST7735_SLPOUT);
    Delay_ms (120);
    
    /* 帧率配置  正常/空闲/部分模式 */
    ST7735S_WriteCmd (ST7735_FRMCTR1);
    ST7735S_WriteData (0x05);
    ST7735S_WriteData (0x3A);
    ST7735S_WriteData (0x3A);
                 
    ST7735S_WriteCmd (ST7735_FRMCTR2);
    ST7735S_WriteData (0x05);
    ST7735S_WriteData (0x3A);
    ST7735S_WriteData (0x3A);
                 
    ST7735S_WriteCmd (ST7735_FRMCTR3);
    ST7735S_WriteData (0x05);
    ST7735S_WriteData (0x3A);
    ST7735S_WriteData (0x3A);
    ST7735S_WriteData (0x05);
    ST7735S_WriteData (0x3A);
    ST7735S_WriteData (0x3A);
    
    /* 反转控制 */
    ST7735S_WriteCmd (ST7735_INVCTR);
    ST7735S_WriteData (0x07);
     
     /* 电源配置 */
    ST7735S_WriteCmd (ST7735_PWCTR1);
    ST7735S_WriteData (0xA2);
    ST7735S_WriteData (0x02);
    ST7735S_WriteData (0x84);
    
    ST7735S_WriteCmd (ST7735_PWCTR2);
    ST7735S_WriteData (0xC0);
    
    ST7735S_WriteCmd (ST7735_PWCTR3);
    ST7735S_WriteData (0x0A);
    ST7735S_WriteData (0x00);
    
    ST7735S_WriteCmd (ST7735_PWCTR4);
    ST7735S_WriteData (0x8A);
    ST7735S_WriteData (0x2A);
    
    ST7735S_WriteCmd (ST7735_PWCTR5 );
    ST7735S_WriteData (0x8A);
    ST7735S_WriteData (0xEE);
    
    /*VOM配置*/
    
    ST7735S_WriteCmd (ST7735_VMCTR1);
    ST7735S_WriteData (0x0E);
    
    /*显存访问控制MADCTL 竖屏 RGB顺序*/
    ST7735S_WriteCmd (ST7735_MADCTL);
    ST7735S_WriteData (MADCTL_MX |MADCTL_MY |MADCTL_BGR );
    
    /*像素格式 RGB565=16位*/
    ST7735S_WriteCmd (ST7735_COLMOD);
    ST7735S_WriteData (0x05);   //05h=16位/pixel
    
    /*伽马校准*/
    ST7735S_WriteCmd (ST7735_GMCTRP1);
    uint8_t Gamma_P[]={0x0F,0x1A,0x0F,0x18,0x2F,0x28,0x20,0x22,
                       0x1F,0x1B,0x23,0x27,0x00,0x04,0x05,0x0D};
    ST7735S_WriteArr (Gamma_P ,sizeof (Gamma_P ));
                       
    ST7735S_WriteCmd (ST7735_GMCTRN1);
    uint8_t Gamma_N[]={0x0F,0x1B,0x0F,0x17,0x33,0x2C,0x29,0x2E,
                       0x30,0x30,0x39,0x3F,0x00,0x05,0x03,0x0D};
    ST7735S_WriteArr (Gamma_N ,sizeof (Gamma_N ));
                       
    /*关闭显示反转*/
    ST7735S_WriteCmd (ST7735_INVOFF );
    
    /*正常显示模式*/
    ST7735S_WriteCmd (ST7735_NORON );
    Delay_ms (10);
    
    /*开启显示*/
    ST7735S_WriteCmd (ST7735_DISPON);
    Delay_ms (100);
    
    /*默认清屏*/
    ST7735S_FillColor (BLACK );
    
    /*开启背光*/
    ST7735S_BackLight (1);
  
    
}

void ST7735S_SetWindow(uint8_t X0,uint8_t Y0,uint8_t X1,uint8_t Y1)
{
    uint8_t X_OffSet = 2;
    uint8_t Y_OffSet = 3;
    
    /*列地址*/
    ST7735S_WriteCmd (ST7735_CASET);
    ST7735S_WriteData (0x00);
    ST7735S_WriteData (X0+X_OffSet);
    ST7735S_WriteData (0x00);
    ST7735S_WriteData (X1+X_OffSet);
    
    /*行地址*/
    ST7735S_WriteCmd (ST7735_RASET);
    ST7735S_WriteData (0x00);
    ST7735S_WriteData (Y0+Y_OffSet);
    ST7735S_WriteData (0x00);
    ST7735S_WriteData (Y1+Y_OffSet);
    
    /*准备写显存*/
    ST7735S_WriteCmd (ST7735_RAMWR);
}

/*全屏填色*/
void ST7735S_FillColor(uint16_t Color)
{
    uint32_t i;
    ST7735S_SetWindow (0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
    
    LCD_CS_LOW ();
    LCD_DC_HIGH ();
    for(i=0;i<(uint32_t)LCD_WIDTH * LCD_HEIGHT;i++)
    {
        BSP_LCD_SPI_SendByte ((uint8_t)(Color >>8));
        BSP_LCD_SPI_SendByte ((uint8_t)(Color & 0xFF)); 
    }
    LCD_CS_HIGH ();
}

void ST7735S_FillRectangel(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,uint16_t Color)
{
    uint32_t i,Total;
    if(X+Width > LCD_WIDTH || Y+Height > LCD_HEIGHT ) return;
    
    Total = (uint32_t)Width * Height;
    ST7735S_SetWindow (X,Y,X+Width-1 ,Y+Height-1);
    
    LCD_CS_LOW ();
    LCD_DC_HIGH ();
    for(i=0;i<Total;i++)
    {
        BSP_LCD_SPI_SendByte ((uint8_t )(Color >> 8));
        BSP_LCD_SPI_SendByte ((uint8_t )(Color & 0xFF));
    }
    LCD_CS_HIGH ();
}

void ST7735S_DrawPoint(uint8_t X,uint8_t Y,uint16_t Color)
{
    if(X>=LCD_WIDTH || Y>=LCD_HEIGHT) return ;
    ST7735S_SetWindow (X,Y,X,Y);
    ST7735S_WriteData16 (Color);
}

void ST7735S_ShowImage(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,const uint16_t *Image)
{
    if(X+Width > LCD_WIDTH || Y+Height > LCD_HEIGHT )return ;
    ST7735S_SetWindow (X,Y,X+Width-1,Y+Height-1);
    ST7735S_WriteArr((const uint8_t *)Image ,(uint32_t)Width *Height *2);
}

void ST7735S_DMA_ShowImage(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,const uint16_t *Image)
{
    if(X+Width > LCD_WIDTH || Y+Height > LCD_HEIGHT ) return ;
    ST7735S_SetWindow (X,Y,X+Width-1,Y+Height-1);
    
    LCD_CS_LOW ();
    LCD_DC_HIGH ();
    BSP_LCD_SPI_DMA_Send ((const uint8_t *)Image,(uint32_t )Width * Height *2);
    
}

/*绘制单个8X16字符 (优化: 先拼帧缓冲, 单次SetWindow+批量写入, 避免每像素都重设窗口) */
void ST7735S_DrawChar(uint8_t X,uint8_t Y,char ch,uint16_t Color,uint16_t BG_Color)
{
    uint8_t i,j;
    uint8_t Font_Byte;
    uint16_t pX;
    uint16_t idx,color;
    static uint8_t buf[8*16*2];   /* 8x16像素 * 2字节(RGB565) = 256字节 */
    
    if(ch < ' ' || ch > '~') return ;
    if(X+8 > LCD_WIDTH || Y+16 > LCD_HEIGHT ) return;
    
    pX=(ch-' ')*16;
    
    for(i=0;i<8;i++)          /* i: 列(X方向) */
    {
        Font_Byte =F8X16 [pX+i*2];
        for(j=0;j<8;j++)      /* j: 行 0~7 */
        {
            color = (Font_Byte & (0x01<<j)) ? Color : BG_Color;
            idx = (uint16_t)(j*8+i)*2;
            buf[idx]   = (uint8_t)(color>>8);
            buf[idx+1] = (uint8_t)(color&0xFF);
        }
        
        Font_Byte =F8X16 [pX+i*2+1];
        for(j=0;j<8;j++)      /* j: 行 8~15 */
        {
            color = (Font_Byte & (0x01<<j)) ? Color : BG_Color;
            idx = (uint16_t)((j+8)*8+i)*2;
            buf[idx]   = (uint8_t)(color>>8);
            buf[idx+1] = (uint8_t)(color&0xFF);
        }
    }
    
    ST7735S_SetWindow(X,Y,X+7,Y+15);
    ST7735S_WriteArr(buf,sizeof(buf));
}

void ST7735S_DrawString(uint8_t X,uint8_t Y,char *Str,uint16_t Color,uint16_t BG_Color)
{
    while(*Str != '\0')
    {
        ST7735S_DrawChar (X,Y,*Str,Color,BG_Color);
        X+=8;
        if(X+8>LCD_WIDTH)
        {
            X=0;
            Y+=16;
        }
        Str++;
    }
}

/*绘制单个16X32字符 (优化: 单次SetWindow+批量写入) */
void ST7735S_DrawChar_16X32(uint8_t X,uint8_t Y,char ch,uint16_t Color,uint16_t BG_Color)
{
    uint8_t i,j,k,row;
    uint8_t Font_Byte;
    uint16_t pX;
    uint8_t index;
    uint16_t idx,color;
    static uint8_t buf[16*32*2];  /* 16x32像素 * 2字节 = 1024字节 */
    
    if(ch < ' ') return ;
    if(ch >= '0' && ch <= '9') index = ch - '0';
    else if(ch == ':') index = 10;
    else if(ch == 'C') index = 11;
    else if(ch == ' ') index = 12;
    else return;
    if(X+16 > LCD_WIDTH || Y+32 > LCD_HEIGHT ) return;
    
    pX=index*64;
    
    for(i=0;i<16;i++)         /* i: 列(X方向) 0~15 */
    {
        for(k=0;k<4;k++)      /* k: 每4字节覆盖32行, 每字节8行 */
        {
            Font_Byte = F16X32 [pX + i * 4 + k];
            
            for(j=0;j<8;j++)
            {
                row = (uint8_t)(k*8+j);
                color = (Font_Byte & (0x01<<j)) ? Color : BG_Color;
                idx = (uint16_t)(row*16+i)*2;
                buf[idx]   = (uint8_t)(color>>8);
                buf[idx+1] = (uint8_t)(color&0xFF);
            }
        }
    }
    
    ST7735S_SetWindow(X,Y,X+15,Y+31);
    ST7735S_WriteArr(buf,sizeof(buf));
}

void ST7735S_DrawString_16X32(uint8_t X,uint8_t Y,char *Str,uint16_t Color,uint16_t BG_Color)
{
    while(*Str != '\0')
    {
        ST7735S_DrawChar_16X32 (X,Y,*Str,Color,BG_Color);
        X+=16;
        if(X+16>LCD_WIDTH)
        {
            X=0;
            Y+=32;
        }
        Str++;
    }
}


void ST7735S_DrawChinese(uint8_t X,uint8_t Y,const uint8_t *Font_Data,uint16_t Color,uint16_t BG_Color)
{
    uint8_t i,j;
    uint8_t Font_Byte;
    uint16_t idx,color;
    static uint8_t buf[16*16*2];  /* 16x16像素 * 2字节 = 512字节 */
    
    if(X+16>LCD_WIDTH || Y+16>LCD_HEIGHT ) return;
    
    for(i=0;i<16;i++)
    {
        Font_Byte = Font_Data [i*2];
        for(j=0;j<8;j++)
        {
            color = (Font_Byte & (0x01<<j)) ? Color : BG_Color;
            idx = (uint16_t)(j*16+i)*2;
            buf[idx]   = (uint8_t)(color>>8);
            buf[idx+1] = (uint8_t)(color&0xFF);
        }
        
        Font_Byte = Font_Data [i*2+1];
        for(j=0;j<8;j++)
        {
            color = (Font_Byte & (0x01<<j)) ? Color : BG_Color;
            idx = (uint16_t)((j+8)*16+i)*2;
            buf[idx]   = (uint8_t)(color>>8);
            buf[idx+1] = (uint8_t)(color&0xFF);
        }
    }
    
    ST7735S_SetWindow(X,Y,X+15,Y+15);
    ST7735S_WriteArr(buf,sizeof(buf));
}

void ST7735S_DrawString_Mix(uint8_t X,uint8_t Y,char *Str,uint16_t Color,uint16_t BG_Color)
{
    uint16_t i;
    uint8_t utf8_bytes;     /* UTF-8字符的字节数 */

    while(*Str != '\0')
    {
        if((uint8_t)(*Str) <= 127)
        {
            /* ASCII单字节字符 */
            ST7735S_DrawChar(X, Y, *Str, Color, BG_Color);
            X += 8;
            Str += 1;
        }
        else
        {
            /* 根据UTF-8首字节判断字符长度 */
            if(((uint8_t)(*Str) & 0xE0) == 0xC0)           /* 110xxxxx: 2字节 */
                utf8_bytes = 2;
            else if(((uint8_t)(*Str) & 0xF0) == 0xE0)      /* 1110xxxx: 3字节(CJK) */
                utf8_bytes = 3;
            else if(((uint8_t)(*Str) & 0xF8) == 0xF0)      /* 11110xxx: 4字节 */
                utf8_bytes = 4;
            else
                utf8_bytes = 1;                             /* 非法, 跳过 */

            /* 在字库中匹配UTF-8编码 */
            for(i = 0; i < CH16X16_Num; i++)
            {
                if(CH16X16[i].Index[0] == Str[0] &&
                   CH16X16[i].Index[1] == Str[1] &&
                   (utf8_bytes < 3 || CH16X16[i].Index[2] == Str[2]))
                {
                    ST7735S_DrawChinese(X, Y, CH16X16[i].Msk, Color, BG_Color);
                    break;
                }
            }
            X += 16;
            Str += utf8_bytes;      /* UTF-8字符占用的实际字节数 */
        }

        if(X >= LCD_WIDTH)
        {
            X = 0;
            Y += 16;
        }
    }
}


/*等待TE垂直同步 防止撕裂*/
//void ST7735S_WaitTE(void)
//{
//    while(BSP_LCD_TE_GetFlag ()==0);
//    BSP_LCD_TE_ClearFlag ();
//}

void ST7735S_BackLight(uint8_t State)
{
    if(State) LCD_BLK_ON ();
    else      LCD_BLK_OFF ();
}
