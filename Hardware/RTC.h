#ifndef   __RTC_H
#define   __RTC_H

#include <stdbool.h>
#include <stdint.h>

extern uint16_t RTC_Time[];

void RTC_Init(void);
void RTC_SetTime(void);
void RTC_GetTime(void);
void RTC_Set_timestamp(uint32_t timestamp);
void RTC_Get_timestamp(uint32_t *timestamp);

#endif

