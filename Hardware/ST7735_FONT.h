#ifndef   __ST7735_FONT_H
#define   __ST7735_FONT_H

#include "stm32f10x.h"                  // Device header

#define FONT_8X16_WIDTH         8
#define FONT_8X16_HEIGHT        16
#define FONT_8X16_CHAR_SIZE     16  /*单个字符占用字节数*/

typedef struct{
    char Index[4];      /* UTF-8编码(最多3字节) + 结束符 */
    uint8_t Msk[32];
}Chinese_Font_Def;

extern const uint8_t F8X16[];
extern const uint8_t F16X32[];
extern const uint16_t CH16X16_Num;
extern const Chinese_Font_Def CH16X16[];

#endif
