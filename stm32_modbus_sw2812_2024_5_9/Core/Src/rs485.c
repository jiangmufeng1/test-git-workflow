#include "rs485.h"

/* 串口中断接收到数据的长度变量 */
uint8_t USART_RX_CNT = 0;
/* 串口中断接收到数据保存的缓冲区 */
uint8_t USART2_RX_BUF[255];

/* 用于标识串口接收数据包是否完成标志 */
uint8_t From_Flag = 0;
/* 通讯标志 主机发送数据后置1 接收到应答后清零 */
uint8_t RS485Busy = 0;
/* 接收缓存区 */
uint8_t RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
/* 用于保存读命令获得的数据 */
uint16_t ReadDateVal = 0;
uint16_t Battery_voltage = 0;
uint16_t Battery_capacity = 0;
uint16_t CD_current = 0;


/****************************************************************************************************
 * 函数名称： void Send_Data(u8 *buf,u8 len)
 * 入口参数：u8 *buf u8 len
 * 返回  值：无
 * 功能说明：串口发送数据
 * 			 buf:发送区首地址
 *			 len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
 ***************************************************************************************************/
//uint8_t f=0;
void Send_Data(uint8_t *buf,uint8_t len)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);			//设置为发送模式
//	HAL_Delay(1);      //延时可有可无
//	for(t=0;t<len;t++)		//循环发送数据
//	{
//		HAL_UART_Transmit(&huart2,&buf[t],1,30);
//		printf("发送的值 = buf[%d] = %x \r\n",t,buf[t]);
////		HAL_Delay(2);
//	}
	HAL_UART_Transmit(&huart2,buf,len,20);

//	HAL_Delay(3);
	USART_RX_CNT=0;	  
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);				//设置为接收模式	
}

/****************************************************************************************************
 * 函数名称：u8 UartRead(u8 *buf, u8 len) 
 * 入口参数：u8 *buf u8 len
 * 返回  值：u8
 * 功能说明：计算接收的数据长度，并且将数据放到*buf数组中
 ***************************************************************************************************/             
uint8_t UartRead(uint8_t *buf, uint8_t len)  
{
	uint8_t i;
	if(len > USART_RX_CNT)  		//指定读取长度大于实际接收到的数据长度时
	{
		len = USART_RX_CNT; 		//读取长度设置为实际接收到的数据长度
	}
	
	for(i = 0;i < len;i ++)  		//拷贝接收到的数据到接收指针中
	{
		*buf = USART2_RX_BUF[i];  	//将数据复制到buf中
		buf  ++;
	}
	USART_RX_CNT=0;              	//接收计数器清零
	return len;                   	//返回实际读取长度
}
/****************************************************************************************************
 * 函数名称：void UartRxMonitor(u8 ms)
 * 入口参数：u8 ms
 * 返回  值：无
 * 功能说明：在定时器中调用，用于监控数据接收
 ***************************************************************************************************/   
void UartRxMonitor(uint8_t ms) 					
{
	static uint8_t USART_RX_BKP = 0;  			//定义USART2_RC_BKP暂时存储诗句长度与实际长度比较
	static uint8_t idletmr      = 0;        		//定义监控时间
	
	if(USART_RX_CNT > 0)					//接收计数器大于零时，监控总线空闲时间
	{
		if(USART_RX_BKP != USART_RX_CNT) 	//接收计数器改变，即刚接收到数据时，清零空闲计时
		{
			USART_RX_BKP = USART_RX_CNT;  	//赋值操作，将实际长度给USART2_RX_BKP
			idletmr      = 0;               //将监控时间清零
		}
		else                              	//接收计数器未改变，即总线空闲时，累计空闲时间
		{
											//如果在一帧数据完成之前有超过3.5个字节时间的停顿，接收设备将刷新当前的消息并假定下一个字节是一个新的数据帧的开始
			if(idletmr < 3)                 //空闲时间小于3ms时，持续累加
			{
				idletmr += ms;
				if(idletmr >= 3)            //空闲时间达到3ms时，即判定为1帧接收完毕
				{
					From_Flag = 1;			//设置命令到达标志，帧接收完毕标志
				}
			}
		}
	}
	else
	{
		USART_RX_BKP = 0;
	}
}

/****************************************************************************************************
 * 函数名称：void UartRxMonitor(u8 ms)
 * 入口参数：u8 ms
 * 返回  值：无
 * 功能说明：串口驱动函数，检测数据帧的接收，调度功能函数，需在主循环中调用
 *           这里是主机，所以在功能调度函数里只需要得到读命令返回的数据，
 *           得到的数据根据自已的需要使用即可，这里是基于modbus协议，所以
 *           需要对照modbus协议去理解，请查阅资料里的modbus资料    计算不包含针头
 ***************************************************************************************************/   
void UartDriver(void)
{
	unsigned char i;
	
	unsigned int crc;
	unsigned char crch,crcl;
	static unsigned char len;
	static unsigned char buf[255];
	if (From_Flag)            									//帧接收完成标志，即接收到一帧新数据
	{
		From_Flag = 0;           								//帧接收完成标志清零
		len       = UartRead(buf,sizeof(buf));   				//将接收到的命令读到缓冲区中
		crc       = GetCRC16(buf,len-2);       					//计算CRC校验值，除去CRC校验值
		crch=crc  >> 8;    										//crc高位
		crcl=crc  &  0xFF;										//crc低位
		printf("485接收数据\r\n");
		for(i = 0;i < len;i ++)
			printf("485接收的值 = buf[%d] = %x \r\n",i,buf[i]);
//		printf("%x  ",buf[i]);
		printf("\n");
		if((buf[len-2] == crch) && (buf[len-1] == crcl))  		//判断CRC校验是否正确
		{
			if (buf[1] == 0x03)									//0x03 读命令
			{
//					/* 通过上面的验证判断后 在这里可直接获取数据 保存在ReadDateVal中 */
//					ReadDateVal = buf[3] * 0x100 + buf[4];		//modbus数据是16位的
//					printf("%u   ",ReadDateVal);
				//电池电压
				Battery_voltage = buf[35] * 0x100 + buf[36];
				printf("%f\n",Battery_voltage*0.01);
				//充放电流
				CD_current = buf[37] * 0x100 + buf[38];
				printf("CD_current= %f\n",CD_current*0.01);
				//电池容量
				Battery_capacity = buf[39] * 0x100 + buf[40];
				int Battery_capacity_int= 0;
				Battery_capacity_int	= Battery_capacity*10;
				printf("%d \n",Battery_capacity_int);
				if(Battery_capacity_int>15605)
					RGB_GREEN();
					else if(Battery_capacity_int>6242&&Battery_capacity_int<=15605)
						RGB_YELLOW();
					else 
					{
						printf("提醒用户充电");
						RGB_RED();
					}
				
			}
			/* 写命令不需要数据只需要应答即可 */
			
			/* 判断校验码正确后 无论是读还是写 都清零485忙标志，表示收到应答，释放485，可进行其它命令操作 */
			RS485Busy = 0;										
		}
		else	/* 如果校验值错误，执行错误处理函数 这里用串口1打印一条提示信息 也可自定义共它处理函数 */
			printf("接收错误\r\n");
	}
}

/****************************************************************************************************
 * 函数名称：void RS485_RW_Opr(u8 ucAddr,u8 ucCmd,u16 ucReg,u16 uiDate)
 * 入口参数：u8 ucAddr,u8 ucCmd,u16 ucReg,u16 uiDate
 * 			 ucAddr：从机地址
 *			 ucCmd ：功能码 03->读	06->写
 *			 ucReg ：寄存器地址
 *			 uiDate：写操作即是发送的数据 读操作即是读取数据个数
 * 返回  值：无
 * 功能说明：485读写操作函数
 ***************************************************************************************************/   
void RS485_RW_Opr(uint8_t ucAddr,uint8_t ucCmd,uint16_t ucReg,uint16_t uiDate)
{
	unsigned int crc;
	unsigned char crcl;
	unsigned char crch;	
	unsigned char ucBuf[8];
	
	ucBuf[0] = ucAddr;				/* 从机地址 */
	ucBuf[1] = ucCmd;				/* 命令 06 写 03 读 */
	ucBuf[2] = ucReg >> 8;			/* 寄存器地址高位 */
	ucBuf[3] = ucReg;				/* 寄存器地址低位 */
	ucBuf[4] = uiDate >> 8;			/* 数据高8位 */
	ucBuf[5] = uiDate;				/* 数据低8位 */
	crc      = GetCRC16(ucBuf,6);   /* 计算CRC校验值 */
	crch     = crc >> 8;    		/* crc高位 */
	crcl     = crc &  0xFF;			/* crc低位 */
	ucBuf[6] = crch;				/* 校验高8位 */
	ucBuf[7] = crcl;				/* 校验低8位 */
	
	Send_Data(ucBuf,8);				/* 发送数据 */
	RS485Busy = 1;					/* 发送完成后 忙标志置位 */
}




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	uint8_t i;
	if(huart->Instance == huart2.Instance)
	{
			USART2_RX_BUF[USART_RX_CNT++] = recvData;
		while(HAL_UART_Receive_IT(&huart2,&recvData,1)!= HAL_OK);
		
	}
}



