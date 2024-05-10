#include "rs485.h"

/* �����жϽ��յ����ݵĳ��ȱ��� */
uint8_t USART_RX_CNT = 0;
/* �����жϽ��յ����ݱ���Ļ����� */
uint8_t USART2_RX_BUF[255];

/* ���ڱ�ʶ���ڽ������ݰ��Ƿ���ɱ�־ */
uint8_t From_Flag = 0;
/* ͨѶ��־ �����������ݺ���1 ���յ�Ӧ������� */
uint8_t RS485Busy = 0;
/* ���ջ����� */
uint8_t RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
/* ���ڱ���������õ����� */
uint16_t ReadDateVal = 0;
uint16_t Battery_voltage = 0;
uint16_t Battery_capacity = 0;
uint16_t CD_current = 0;


/****************************************************************************************************
 * �������ƣ� void Send_Data(u8 *buf,u8 len)
 * ��ڲ�����u8 *buf u8 len
 * ����  ֵ����
 * ����˵�������ڷ�������
 * 			 buf:�������׵�ַ
 *			 len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
 ***************************************************************************************************/
//uint8_t f=0;
void Send_Data(uint8_t *buf,uint8_t len)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);			//����Ϊ����ģʽ
//	HAL_Delay(1);      //��ʱ���п���
//	for(t=0;t<len;t++)		//ѭ����������
//	{
//		HAL_UART_Transmit(&huart2,&buf[t],1,30);
//		printf("���͵�ֵ = buf[%d] = %x \r\n",t,buf[t]);
////		HAL_Delay(2);
//	}
	HAL_UART_Transmit(&huart2,buf,len,20);

//	HAL_Delay(3);
	USART_RX_CNT=0;	  
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);				//����Ϊ����ģʽ	
}

/****************************************************************************************************
 * �������ƣ�u8 UartRead(u8 *buf, u8 len) 
 * ��ڲ�����u8 *buf u8 len
 * ����  ֵ��u8
 * ����˵����������յ����ݳ��ȣ����ҽ����ݷŵ�*buf������
 ***************************************************************************************************/             
uint8_t UartRead(uint8_t *buf, uint8_t len)  
{
	uint8_t i;
	if(len > USART_RX_CNT)  		//ָ����ȡ���ȴ���ʵ�ʽ��յ������ݳ���ʱ
	{
		len = USART_RX_CNT; 		//��ȡ��������Ϊʵ�ʽ��յ������ݳ���
	}
	
	for(i = 0;i < len;i ++)  		//�������յ������ݵ�����ָ����
	{
		*buf = USART2_RX_BUF[i];  	//�����ݸ��Ƶ�buf��
		buf  ++;
	}
	USART_RX_CNT=0;              	//���ռ���������
	return len;                   	//����ʵ�ʶ�ȡ����
}
/****************************************************************************************************
 * �������ƣ�void UartRxMonitor(u8 ms)
 * ��ڲ�����u8 ms
 * ����  ֵ����
 * ����˵�����ڶ�ʱ���е��ã����ڼ�����ݽ���
 ***************************************************************************************************/   
void UartRxMonitor(uint8_t ms) 					
{
	static uint8_t USART_RX_BKP = 0;  			//����USART2_RC_BKP��ʱ�洢ʫ�䳤����ʵ�ʳ��ȱȽ�
	static uint8_t idletmr      = 0;        		//������ʱ��
	
	if(USART_RX_CNT > 0)					//���ռ�����������ʱ��������߿���ʱ��
	{
		if(USART_RX_BKP != USART_RX_CNT) 	//���ռ������ı䣬���ս��յ�����ʱ��������м�ʱ
		{
			USART_RX_BKP = USART_RX_CNT;  	//��ֵ��������ʵ�ʳ��ȸ�USART2_RX_BKP
			idletmr      = 0;               //�����ʱ������
		}
		else                              	//���ռ�����δ�ı䣬�����߿���ʱ���ۼƿ���ʱ��
		{
											//�����һ֡�������֮ǰ�г���3.5���ֽ�ʱ���ͣ�٣������豸��ˢ�µ�ǰ����Ϣ���ٶ���һ���ֽ���һ���µ�����֡�Ŀ�ʼ
			if(idletmr < 3)                 //����ʱ��С��3msʱ�������ۼ�
			{
				idletmr += ms;
				if(idletmr >= 3)            //����ʱ��ﵽ3msʱ�����ж�Ϊ1֡�������
				{
					From_Flag = 1;			//����������־��֡������ϱ�־
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
 * �������ƣ�void UartRxMonitor(u8 ms)
 * ��ڲ�����u8 ms
 * ����  ֵ����
 * ����˵�������������������������֡�Ľ��գ����ȹ��ܺ�����������ѭ���е���
 *           �����������������ڹ��ܵ��Ⱥ�����ֻ��Ҫ�õ�������ص����ݣ�
 *           �õ������ݸ������ѵ���Ҫʹ�ü��ɣ������ǻ���modbusЭ�飬����
 *           ��Ҫ����modbusЭ��ȥ��⣬������������modbus����    ���㲻������ͷ
 ***************************************************************************************************/   
void UartDriver(void)
{
	unsigned char i;
	
	unsigned int crc;
	unsigned char crch,crcl;
	static unsigned char len;
	static unsigned char buf[255];
	if (From_Flag)            									//֡������ɱ�־�������յ�һ֡������
	{
		From_Flag = 0;           								//֡������ɱ�־����
		len       = UartRead(buf,sizeof(buf));   				//�����յ������������������
		crc       = GetCRC16(buf,len-2);       					//����CRCУ��ֵ����ȥCRCУ��ֵ
		crch=crc  >> 8;    										//crc��λ
		crcl=crc  &  0xFF;										//crc��λ
		printf("485��������\r\n");
		for(i = 0;i < len;i ++)
			printf("485���յ�ֵ = buf[%d] = %x \r\n",i,buf[i]);
//		printf("%x  ",buf[i]);
		printf("\n");
		if((buf[len-2] == crch) && (buf[len-1] == crcl))  		//�ж�CRCУ���Ƿ���ȷ
		{
			if (buf[1] == 0x03)									//0x03 ������
			{
//					/* ͨ���������֤�жϺ� �������ֱ�ӻ�ȡ���� ������ReadDateVal�� */
//					ReadDateVal = buf[3] * 0x100 + buf[4];		//modbus������16λ��
//					printf("%u   ",ReadDateVal);
				//��ص�ѹ
				Battery_voltage = buf[35] * 0x100 + buf[36];
				printf("%f\n",Battery_voltage*0.01);
				//��ŵ���
				CD_current = buf[37] * 0x100 + buf[38];
				printf("CD_current= %f\n",CD_current*0.01);
				//�������
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
						printf("�����û����");
						RGB_RED();
					}
				
			}
			/* д�����Ҫ����ֻ��ҪӦ�𼴿� */
			
			/* �ж�У������ȷ�� �����Ƕ�����д ������485æ��־����ʾ�յ�Ӧ���ͷ�485���ɽ�������������� */
			RS485Busy = 0;										
		}
		else	/* ���У��ֵ����ִ�д������� �����ô���1��ӡһ����ʾ��Ϣ Ҳ���Զ��干�������� */
			printf("���մ���\r\n");
	}
}

/****************************************************************************************************
 * �������ƣ�void RS485_RW_Opr(u8 ucAddr,u8 ucCmd,u16 ucReg,u16 uiDate)
 * ��ڲ�����u8 ucAddr,u8 ucCmd,u16 ucReg,u16 uiDate
 * 			 ucAddr���ӻ���ַ
 *			 ucCmd �������� 03->��	06->д
 *			 ucReg ���Ĵ�����ַ
 *			 uiDate��д�������Ƿ��͵����� ���������Ƕ�ȡ���ݸ���
 * ����  ֵ����
 * ����˵����485��д��������
 ***************************************************************************************************/   
void RS485_RW_Opr(uint8_t ucAddr,uint8_t ucCmd,uint16_t ucReg,uint16_t uiDate)
{
	unsigned int crc;
	unsigned char crcl;
	unsigned char crch;	
	unsigned char ucBuf[8];
	
	ucBuf[0] = ucAddr;				/* �ӻ���ַ */
	ucBuf[1] = ucCmd;				/* ���� 06 д 03 �� */
	ucBuf[2] = ucReg >> 8;			/* �Ĵ�����ַ��λ */
	ucBuf[3] = ucReg;				/* �Ĵ�����ַ��λ */
	ucBuf[4] = uiDate >> 8;			/* ���ݸ�8λ */
	ucBuf[5] = uiDate;				/* ���ݵ�8λ */
	crc      = GetCRC16(ucBuf,6);   /* ����CRCУ��ֵ */
	crch     = crc >> 8;    		/* crc��λ */
	crcl     = crc &  0xFF;			/* crc��λ */
	ucBuf[6] = crch;				/* У���8λ */
	ucBuf[7] = crcl;				/* У���8λ */
	
	Send_Data(ucBuf,8);				/* �������� */
	RS485Busy = 1;					/* ������ɺ� æ��־��λ */
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



