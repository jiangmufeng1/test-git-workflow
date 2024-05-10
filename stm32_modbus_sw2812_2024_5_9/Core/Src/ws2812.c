 #include "ws2812.h"
 
uint8_t RGB_BIT_Buffer[RGB_BIT];
uint8_t buffer[RGB_BIT*LED_NUMS];
LEDType LED[LED_NUMS];
 
 
void WS2812_Update(void)
{
	HAL_SPI_Transmit_DMA(&hspi2,buffer,RGB_BIT*LED_NUMS);
}
 
/**
create 24 byte sent by SPI using RGB values.
*/
static void WS2812_CreatData(uint8_t R,uint8_t G,uint8_t B)
{
    uint8_t temp[RGB_BIT] = {0};
		uint8_t i;
    for (i=0;i<8;i++){
        temp[7-i] =  (G & 0x01) ? WS2812_1 : WS2812_0; 
        G = G >> 1;
    }
    for (i=0;i<8;i++){
        temp[15-i] =  (R & 0x01) ? WS2812_1 : WS2812_0; 
        R = R >> 1;
    }
    for (i=0;i<8;i++){
        temp[23-i] =  (B & 0x01) ? WS2812_1 : WS2812_0; 
        B = B >> 1;
    }
    memcpy(RGB_BIT_Buffer, temp, RGB_BIT);
}
 
/**
cook the whole buffer made by many(16 pieces) RGB_BIT_Buffers.
*/
static void WS2812_MakeBuffer()
{
	uint16_t i;
	for(i=0;i < LED_NUMS;i++)
	{
		WS2812_CreatData(LED[i].R,LED[i].G,LED[i].B);
		memcpy(buffer + i * RGB_BIT, RGB_BIT_Buffer, RGB_BIT);
	}
}
 
 
 
void WS2812_TurnOff(void)
{
	uint16_t i;
	for(i=0;i<LED_NUMS*24;i++)
	{
		buffer[i]=WS2812_0;
	}
}
 
 
/**
an led of position will bright with color value
pos is [0 , max-1]
*/
static void WS2812_Color_Pos(uint32_t color,uint16_t Pos)
{
	uint8_t R,G,B;
//	uint16_t i;
	
	R=(color >> 16 ) & 0x00FF;
	G=(color >> 8  ) & 0x0000FF;
	B=(color 	   ) & 0x0000FF;
	
	WS2812_CreatData(R,G,B);
	if(Pos < LED_NUMS && Pos >=0)
	{
		memcpy(buffer + RGB_BIT * Pos, RGB_BIT_Buffer,RGB_BIT);
	}
	else
	{
		WS2812_TurnOff();
	}
}
 
//红色
void RGB_RED(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0xFF0000,(i)%7);
	WS2812_Color_Pos(0xFF0000,(i+1)%7);
	WS2812_Color_Pos(0xFF0000,(i+2)%7);
	WS2812_Color_Pos(0xFF0000,(i+3)%7);
	WS2812_Color_Pos(0xFF0000,(i+4)%7);
	WS2812_Color_Pos(0xFF0000,(i+5)%7);
	WS2812_Color_Pos(0xFF0000,(i+6)%7);
	WS2812_Update();
}


//橙色
void RGB_ORANGE(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0X00FF00,(i)%7);
	WS2812_Color_Pos(0X00FF00,(i+1)%7);
	WS2812_Color_Pos(0X00FF00,(i+2)%7);
	WS2812_Color_Pos(0X00FF00,(i+3)%7);
	WS2812_Color_Pos(0X00FF00,(i+4)%7);
	WS2812_Color_Pos(0X00FF00,(i+5)%7);
	WS2812_Color_Pos(0X00FF00,(i+6)%7);
	WS2812_Update();
}


//黄色
void RGB_YELLOW(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0XFFFF00,(i)%7);
	WS2812_Color_Pos(0XFFFF00,(i+1)%7);
	WS2812_Color_Pos(0XFFFF00,(i+2)%7);
	WS2812_Color_Pos(0XFFFF00,(i+3)%7);
	WS2812_Color_Pos(0XFFFF00,(i+4)%7);
	WS2812_Color_Pos(0XFFFF00,(i+5)%7);
	WS2812_Color_Pos(0XFFFF00,(i+6)%7);
	WS2812_Update();
}

//绿色
void RGB_GREEN(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0X00FF00,(i)%7);
	WS2812_Color_Pos(0X00FF00,(i+1)%7);
	WS2812_Color_Pos(0X00FF00,(i+2)%7);
	WS2812_Color_Pos(0X00FF00,(i+3)%7);
	WS2812_Color_Pos(0X00FF00,(i+4)%7);
	WS2812_Color_Pos(0X00FF00,(i+5)%7);
	WS2812_Color_Pos(0X00FF00,(i+6)%7);
	WS2812_Update();
}

//青色
void RGB_CYAN(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0X00FF7F,(i)%7);
	WS2812_Color_Pos(0X00FF7F,(i+1)%7);
	WS2812_Color_Pos(0X00FF7F,(i+2)%7);
	WS2812_Color_Pos(0X00FF7F,(i+3)%7);
	WS2812_Color_Pos(0X00FF7F,(i+4)%7);
	WS2812_Color_Pos(0X00FF7F,(i+5)%7);
	WS2812_Color_Pos(0X00FF7F,(i+6)%7);
	WS2812_Update();
}

//蓝色
void RGB_BLUE(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0X0000FF,(i)%7);
	WS2812_Color_Pos(0X0000FF,(i+1)%7);
	WS2812_Color_Pos(0X0000FF,(i+2)%7);
	WS2812_Color_Pos(0X0000FF,(i+3)%7);
	WS2812_Color_Pos(0X0000FF,(i+4)%7);
	WS2812_Color_Pos(0X0000FF,(i+5)%7);
	WS2812_Color_Pos(0X0000FF,(i+6)%7);
	WS2812_Update();
}

//紫色
void RGB_PURPLE(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0X7F00FF,(i)%7);
	WS2812_Color_Pos(0X7F00FF,(i+1)%7);
	WS2812_Color_Pos(0X7F00FF,(i+2)%7);
	WS2812_Color_Pos(0X7F00FF,(i+3)%7);
	WS2812_Color_Pos(0X7F00FF,(i+4)%7);
	WS2812_Color_Pos(0X7F00FF,(i+5)%7);
	WS2812_Color_Pos(0X7F00FF,(i+6)%7);
	WS2812_Update();
}

//黑色
void RGB_BLACK(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0X000000,(i)%7);
	WS2812_Color_Pos(0X000000,(i+1)%7);
	WS2812_Color_Pos(0X000000,(i+2)%7);
	WS2812_Color_Pos(0X000000,(i+3)%7);
	WS2812_Color_Pos(0X000000,(i+4)%7);
	WS2812_Color_Pos(0X000000,(i+5)%7);
	WS2812_Color_Pos(0X000000,(i+6)%7);
	WS2812_Update();
}

//白色
void RGB_WHITE(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0XFFFFFF,(i)%7);
	WS2812_Color_Pos(0XFFFFFF,(i+1)%7);
	WS2812_Color_Pos(0XFFFFFF,(i+2)%7);
	WS2812_Color_Pos(0XFFFFFF,(i+3)%7);
	WS2812_Color_Pos(0XFFFFFF,(i+4)%7);
	WS2812_Color_Pos(0XFFFFFF,(i+5)%7);
	WS2812_Color_Pos(0XFFFFFF,(i+6)%7);
	WS2812_Update();
}

//洋红色
void RGB_MAGENTA(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0XFF00DC,(i)%7);
	WS2812_Color_Pos(0XFF00DC,(i+1)%7);
	WS2812_Color_Pos(0XFF00DC,(i+2)%7);
	WS2812_Color_Pos(0XFF00DC,(i+3)%7);
	WS2812_Color_Pos(0XFF00DC,(i+4)%7);
	WS2812_Color_Pos(0XFF00DC,(i+5)%7);
	WS2812_Color_Pos(0XFF00DC,(i+6)%7);
	WS2812_Update();
}


void WS2812_Show_Wheel(void)
{
	static uint16_t i=0;
	i++;
	WS2812_Color_Pos(0xFF0000,(i)%7);
	WS2812_Color_Pos(0X00FF00,(i+1)%7);
	WS2812_Color_Pos(0X7FD800,(i+2)%7);
	WS2812_Color_Pos(0X00FF00,(i+3)%7);
	WS2812_Color_Pos(0X00FF7F,(i+4)%7);
	WS2812_Color_Pos(0X0000FF,(i+5)%7);
	WS2812_Color_Pos(0X7F00FF,(i+6)%7);
//	WS2812_Color_Pos(0X007FFF,(i+7)%7);
//	WS2812_Color_Pos(0X0000FF,(i+8)%7);
//	WS2812_Color_Pos(0X7F00FF,(i+9)%7);
//	WS2812_Color_Pos(0XFF00FF,(i+10)%7);
//	WS2812_Color_Pos(0XFF007F,(i+11)%7);
//	WS2812_Color_Pos(0XFF0000,(i+12)%7);
//	WS2812_Color_Pos(0XFF7F00,(i+13)%7);
//	WS2812_Color_Pos(0XFFFF00,(i+14)%7);
//	WS2812_Color_Pos(0X7FFF00,(i+15)%7);
	WS2812_Update();
}
 

