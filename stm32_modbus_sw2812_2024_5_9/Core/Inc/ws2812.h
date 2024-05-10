#ifndef WS2812_H
#define WS2812_H
 
#include "string.h"
#include "main.h"
#include "spi.h"
 
#define	WS2812_0	0xC0
#define	WS2812_1	0xF0
#define	WS2812_RST	0x00
#define LED_NUMS   	7
#define RGB_BIT   	24
 
typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
}LEDType;
 
 
/**
turn off all leds
*/
void WS2812_TurnOff(void);
void WS2812_Show_Wheel(void);
void WS2812_Update(void);

void RGB_MAGENTA(void);
void RGB_WHITE(void);
void RGB_BLACK(void);
void RGB_PURPLE(void);
void RGB_BLUE(void);
void RGB_CYAN(void);
void RGB_GREEN(void);
void RGB_YELLOW(void);
void RGB_ORANGE(void);
void RGB_RED(void);
#endif

