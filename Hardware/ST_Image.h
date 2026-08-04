#ifndef   __ST_IMAGE_H
#define   __ST_IMAGE_H

#include <stdint.h>

typedef struct
{
    const uint16_t Width;
    const uint16_t Height;
    const uint8_t *data;
} ST_Image_t;

extern const ST_Image_t img_Cloudy;
extern const ST_Image_t img_Snow;
extern const ST_Image_t img_Rainy;
extern const ST_Image_t img_Sunny;
extern const ST_Image_t img_Wind;
extern const ST_Image_t img_yin;

#endif
