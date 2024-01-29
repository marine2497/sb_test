#include "Global.h" 


#define USART1_RX_LEN	512
#define USART1_TX_LEN	512
#define USART1_FRM_SNDLEN	259
static uint8_t m_pucUSART1_Rx[USART1_RX_LEN] = {0};
static uint8_t m_pucUSART1_Tx[USART1_TX_LEN] = {0};
static uint8_t m_unUSART1_RcvedLen = 0;
static uint8_t m_pucUSART1_RcvedBuf[USART1_RX_LEN] = {0};
static double	m_dblDiffGpsData[30] = {0.00};
static double m_dblDiffTime = 0.0, m_dblDiffLat = 0.0, m_dblDiffLon = 0.0, m_dblDiffHorSpd  = 0.0, m_dblDiffTrkGnd = 0.0;
static double m_dblDiffUTC = 0.0,m_dblDiffHeading = 0.0, m_dblDiffRoll = 0.0,m_dblDiffPitch = 0.0, m_dblDiffState = 0.0, m_dblDiffSeeSat = 0.0, m_dblDiffDelay= 0.0;

//$GPRMC
double gl_dblDiffTime = 0.0,gl_dblDiffLat		= 0.0, 	gl_dblDiffLon = 0.0,  gl_dblDiffHorSpd = 0.0,gl_dblDiffTrkGnd = 0.0;
double gl_dblDiffWHeading =0.0;
double gl_dblDiffWHeadingFilter =0.0;
double gl_dblDiffPosN	= 0.0,	gl_dblDiffPosE   = 0.0, gl_dblDiffHeight		= 0.0;
double gl_dblDiffVeloN	= 0.0,	gl_dblDiffVeloE  = 0.0, gl_dblDiffVeloU		= 0.0;
double gl_dblDiffLatControl		= 0.0, 	gl_dblDiffLonControl = 0.0;//用作控制的经纬度 
double gl_dblDiffLatShow		= 0.0, 	gl_dblDiffLonShow = 0.0;//用来显示的经纬度 航迹误差最小的点
//$PSIC
double	gl_dblDiffUTC = 0.0,	gl_dblDiffHeading = 0.0 ,gl_dblDiffRoll = 0.00 , gl_dblDiffPitch = 0.0 , gl_dblDiffState = 0.0 ,gl_dblDiffSeeSat = 0.0 , gl_dblDiffDelay = 0.0;
//$BSWPS //路径数据
static u16 m_unPointNumber=0;
static double m_dblPointLat=0.0,m_dblPointLon=0.0;
static u32 m_ulPointFlag=0;
//$BSWPT //测线切换
static u16 m_unPointCutInLine=0;
double gl_dblPosE = 0.0,gl_dblPosN = 0.0;
//在主程序中初始化

extern int32_t uart3_transmit(const void * data,int32_t len);
//??????while???
void ParseDGPSFrm(unsigned char* pucDataBuf, unsigned int unDataLen)
{
	static uint16_t unDataNo = 0, unDataPart = 0;
	static uint8_t pucDataFrmBuf[USART1_RX_LEN] = {'\0'};
	
	uint8_t ucRcvingChar;
	bool bRestartRcving = false;
	
	uint16_t i;
	m_unUSART1_RcvedLen=unDataLen;
	for (i = 0; i < m_unUSART1_RcvedLen; i++)
	{
		m_pucUSART1_RcvedBuf[i]=pucDataBuf[i];
		ucRcvingChar = m_pucUSART1_RcvedBuf[i];
		pucDataFrmBuf[unDataNo++] = ucRcvingChar;
		
		switch (unDataPart)
		{              
			case 0:
				if (ucRcvingChar == '$')
					unDataPart = 1;
				else
					bRestartRcving = true;
				break;	
			case 1:
				if (ucRcvingChar == 'P'||ucRcvingChar == 'G'||ucRcvingChar == 'B')
					unDataPart = 2;
				else
					bRestartRcving = true;
				break;					
			case 2:
				if (ucRcvingChar == 0x0a)//??
				{ 
					ParseDGPSData(pucDataFrmBuf, unDataNo);
					bRestartRcving = true;
				}
				break;
				
			default:
				bRestartRcving = true;
				break;
		}
		
		if (bRestartRcving)
		{
			unDataNo = 0;
			unDataPart = 0;
			memset(pucDataFrmBuf, '\0', USART1_RX_LEN);
			bRestartRcving = false;
		}
	}
	
	//?????????
	m_unUSART1_RcvedLen = 0;
}
//在主程序中的while内调用
void ParseDGPSData(unsigned char* pucDataBuf, unsigned int unDataLen)
{
#define FLAGNUM_MAX	30
	
	uint16_t unDataType = 0xffff;
	
	uint16_t i;
	uint16_t unFlagNo = 0;
	uint16_t punFlagPos[FLAGNUM_MAX] = {0};
	u8 XorSum=0;                 
	char SendData[100];
	if ((pucDataBuf[1] == 'G') &&//航向
            (pucDataBuf[2] == 'P'||pucDataBuf[2] == 'N') &&
            (pucDataBuf[3] == 'H') &&
            (pucDataBuf[4] == 'D') &&
	    (pucDataBuf[5] == 'T'))
                unDataType = 0;

	if ((pucDataBuf[1] == 'G') &&//经纬度
	    (pucDataBuf[2] == 'P'||pucDataBuf[2] == 'N') &&
	    (pucDataBuf[3] == 'R') &&
	    (pucDataBuf[4] == 'M') &&
	    (pucDataBuf[5] == 'C'))
		unDataType = 1;
	if ((pucDataBuf[1] == 'B') &&//航迹
	    (pucDataBuf[2] == 'S') &&
	    (pucDataBuf[3] == 'W') &&
	    (pucDataBuf[4] == 'P') &&
	    (pucDataBuf[5] == 'S'))
		unDataType = 2;
	if ((pucDataBuf[1] == 'B') &&//状态切换
	    (pucDataBuf[2] == 'S') &&
	    (pucDataBuf[3] == 'V') &&
	    (pucDataBuf[4] == 'S') &&
	    (pucDataBuf[5] == 'C'))
		unDataType = 3;
	if ((pucDataBuf[1] == 'B') &&//测线切换
	    (pucDataBuf[2] == 'S') &&
	    (pucDataBuf[3] == 'W') &&
	    (pucDataBuf[4] == 'P') &&
	    (pucDataBuf[5] == 'T'))
		unDataType = 4;
	if (unDataType == 0xffff)
		return;
	
	//1	
	unFlagNo = 0;
	for (i = 0; i < unDataLen; i++)
	{
		if ((pucDataBuf[i] == ',') || (pucDataBuf[i] == ' '))
			punFlagPos[unFlagNo++] = i + 1;
		
		if ((pucDataBuf[i] == '\0') || (unFlagNo >= FLAGNUM_MAX))
			break;
	}
	if (
		((unDataType == 0) && (unFlagNo !=  2))||
		((unDataType == 1) && (unFlagNo != 12))||
		((unDataType == 2) && (unFlagNo !=  5))||
	  ((unDataType == 3) && ((unFlagNo != 3)&&(unFlagNo != 2)))||
		((unDataType == 4) && (unFlagNo !=  2))
	)
		return;

	//2
	switch(unDataType)
	{
	/*	case(0)://psic,tra
			m_dblDiffGpsData[23] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[1]], NULL);//UTC
			m_dblDiffGpsData[24]	= (double)strtod((char*)&pucDataBuf[punFlagPos[2]], NULL);//航向
			m_dblDiffGpsData[25] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[3]], NULL);//roll
			m_dblDiffGpsData[26] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[4]], NULL);//pitch
			m_dblDiffGpsData[27] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[5]], NULL);//GNSS解状态
			m_dblDiffGpsData[28] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[6]], NULL);//卫星数
			m_dblDiffGpsData[29] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[7]], NULL);//差分延迟
			m_dblDiffUTC      =  	m_dblDiffGpsData[23];
			m_dblDiffHeading	= 	m_dblDiffGpsData[24];
			m_dblDiffRoll 	  =   m_dblDiffGpsData[25];
			m_dblDiffPitch		=		m_dblDiffGpsData[26];
			m_dblDiffState    =		m_dblDiffGpsData[27];
			m_dblDiffSeeSat   =		m_dblDiffGpsData[28];
			m_dblDiffDelay    =		m_dblDiffGpsData[29];
			gl_dblDiffUTC 		= 	m_dblDiffUTC;
			gl_dblDiffHeading = 	m_dblDiffHeading;
			gl_dblDiffRoll 		= 	m_dblDiffRoll;
			gl_dblDiffPitch 	= 	m_dblDiffPitch;
			gl_dblDiffState		= 	m_dblDiffState;
			gl_dblDiffSeeSat 	= 	m_dblDiffSeeSat;
			gl_dblDiffDelay 	=		m_dblDiffDelay;*/
          		case(0)://GPHDT
			m_dblDiffGpsData[23] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[0]], NULL);//航向
			m_dblDiffHeading      =  	m_dblDiffGpsData[23];
			gl_dblDiffHeading = 	m_dblDiffHeading;
                        //printf("heading: %f \r\n",gl_dblDiffHeading);
                        //printf("Turning_Kp:%f\r\n",gl_dblTurning_Kp);         
			break;
          
			break;
		case(1)://gprmc
			m_dblDiffGpsData[0] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[0]],  NULL);//UTC TIME
			m_dblDiffGpsData[1] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[1]],  NULL);//活跃度
			m_dblDiffGpsData[2] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[2]],  NULL);//纬度
			m_dblDiffGpsData[4] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[4]],  NULL);//经度
			m_dblDiffGpsData[6] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[6]],  NULL);//地面速率
			m_dblDiffGpsData[7] 	= (double)strtod((char*)&pucDataBuf[punFlagPos[7]],  NULL);//航迹角
			
			m_dblDiffTime      = m_dblDiffGpsData[0];
			m_dblDiffLat    	 = ((m_dblDiffGpsData[2] - floor(m_dblDiffGpsData[2] / 100) * 100)) * MIN2DAD + floor(m_dblDiffGpsData[2] / 100);
			m_dblDiffLon   	 	 = ((m_dblDiffGpsData[4] - floor(m_dblDiffGpsData[4] / 100) * 100)) * MIN2DAD + floor(m_dblDiffGpsData[4] / 100);
			//m_dblDiffLat    	 = m_dblDiffGpsData[2]/100;
			//m_dblDiffLon   	 	 = m_dblDiffGpsData[4]/100;
			gl_dblDiffLatShow=m_dblDiffLat;//经纬度送至显示
			gl_dblDiffLonShow=m_dblDiffLon;
			InvCalcGrd(m_dblDiffLat,m_dblDiffLon, &gl_dblDiffLatShow, &gl_dblDiffLonShow, 0.5, gl_dblDiffHeading);
			m_dblDiffHorSpd    = m_dblDiffGpsData[6];
			m_dblDiffTrkGnd		 = m_dblDiffGpsData[7];
		
			gl_dblDiffTime      = m_dblDiffTime;//UTC TIME
			gl_dblDiffLat       = m_dblDiffLat;//纬度
			gl_dblDiffLon       = m_dblDiffLon;//经度
			gl_dblDiffHorSpd    = m_dblDiffHorSpd;//地面速率
			gl_dblDiffTrkGnd    = m_dblDiffTrkGnd;//航迹角	
		
			gl_dblDiffVeloN 		= m_dblDiffHorSpd * cos(m_dblDiffTrkGnd * DEG2RAD);//北向速率
			gl_dblDiffVeloE 		= m_dblDiffHorSpd * sin(m_dblDiffTrkGnd * DEG2RAD);//东向速率
			gl_dblDiffVeloU 		= m_dblDiffHorSpd;//合成速率		
			WPS84_XY();
                        
           	break;
		case(2)://bswps
			//m_unPointNumber=(u16)strtod((char*)&pucDataBuf[punFlagPos[0]],  NULL);//点号码
			m_unPointNumber=(u16)strtol((char*)&pucDataBuf[punFlagPos[0]],NULL,16);//点号码
			m_dblPointLat=(double)strtod((char*)&pucDataBuf[punFlagPos[1]],  NULL);//点纬度
			m_dblPointLon=(double)strtod((char*)&pucDataBuf[punFlagPos[2]],  NULL);//点经度
			//m_ulPointFlag=(u32)strtod((char*)&pucDataBuf[punFlagPos[3]],  NULL);//点标志位
			m_ulPointFlag=(u32)strtol((char*)&pucDataBuf[punFlagPos[3]],NULL,16);
			//(double)strtod((char*)&pucDataBuf[punFlagPos[4]],  NULL);//预留
			//printf("Lat lon %f,%f\r\n",m_dblPointLat,m_dblPointLon);
                        switch(m_ulPointFlag&0x0000ffff)//1-16
			{
				case(0x0000):break;//路径点
				case(0x0001):break;//采样点
				case(0x0002):break;//监测点
				case(0x0003):break;//采样监测点
				case(0x0010):break;//测绘点
				default:break;
			}
			switch(m_ulPointFlag&0x003f0000)//17-22
			{
				case(0x00000000):break;//中间点
				case(0x00010000)://开始点
				for(i=0;i<100;i++) //清空路径点
				{
					gl_dblLonD[i]	= 0.0;
					gl_dblLatD[i]	= 0.0;
					
				}
				gl_unCtrlState=0;
                                ucAutoHov=0;
				break;
				case(0x00020000):gl_unPointAll=m_unPointNumber;break;//结束点
				case(0x00030000):gl_unPointAll=m_unPointNumber;break;//开始结束点
				default:break;
			}
                        gl_dblLatD[0]=m_dblDiffLat;
			gl_dblLonD[0]=m_dblDiffLon;
                        gl_dblLatD[m_unPointNumber]=m_dblPointLat;
			gl_dblLonD[m_unPointNumber]=m_dblPointLon;
                        
			//路径数据确认
	/*		if(m_unPointNumber==1)
			Send_ComData("$CSWPR,1*58\r\n",13);
			if(m_unPointNumber==2)
			Send_ComData("$CSWPR,2*5B\r\n",13);
                        if(m_unPointNumber==3)
			Send_ComData("$CSWPR,3*5A\r\n",13);
                        if(m_unPointNumber==4)
			Send_ComData("$CSWPR,4*5D\r\n",13);
                        if(m_unPointNumber==5)
			Send_ComData("$CSWPR,5*5C\r\n",13);
                        if(m_unPointNumber==6)
			Send_ComData("$CSWPR,6*6F\r\n",13);
                        if(m_unPointNumber==7)
			Send_ComData("$CSWPR,7*5E\r\n",13);
*/
		         if(m_unPointNumber<16)
			{
			sprintf(SendData,"$CSWPR,%1X*%2X\r\n",(int)(m_unPointNumber),XorSum);
			XorSum=CRC8Valid(&SendData[1],7);
			sprintf(SendData,"$CSWPR,%1X*%2X\r\n",(int)m_unPointNumber,XorSum);
                        
                        //printf("%s",SendData); 
                        uart3_transmit(SendData,XorSum);//output_to A5
			//Send_ComData(SendData,13);
			}
			else 
			{
			sprintf(SendData,"$CSWPR,%2X*%2X\r\n",(int)(m_unPointNumber),XorSum);
			XorSum=CRC8Valid(&SendData[1],8);
			sprintf(SendData,"$CSWPR,%2X*%2X\r\n",(int)m_unPointNumber,XorSum);
                        
                        //printf("%s",SendData); 
                        uart3_transmit(SendData,XorSum);//output_to A5
			//Send_ComData(SendData,14);
			}
                        
                

                        
                        
                       /* gl_dblLatD[1]=m_dblPointLat-0.0001;
			gl_dblLonD[1]=m_dblPointLon;
	                gl_dblLatD[2]=m_dblPointLat-0.0001;
			gl_dblLonD[2]=m_dblPointLon+0.0003;
                        gl_dblLatD[3]=m_dblPointLat-0.0004;
			gl_dblLonD[3]=m_dblPointLon+0.0003;                      
                        gl_dblLatD[4]=m_dblPointLat-0.0004;
			gl_dblLonD[4]=m_dblPointLon;
                        
                        gl_dblLatD[5]=m_dblPointLat-0.0001;
			gl_dblLonD[5]=m_dblPointLon;
                        m_unPointNumber=5;*/
			break;
		case(3)://状态切换	//bsvsc	
			switch(pucDataBuf[punFlagPos[0]])
			{
				case('M'):gl_unFunctionCode=0;break;//手动
				case('A'):gl_unFunctionCode=6;break;//自动                     
                                
				default:break;
			}
			if(unFlagNo==2)
			{
                            switch(pucDataBuf[punFlagPos[1]])
				{
                                    case('D'):
                                      gl_unFunctionCode=6;
                                      gl_dblLatD[0]=gl_dblDiffLat;
                                      gl_dblLonD[0]=gl_dblDiffLon;	
                                      gl_unCtrlState=0;break;//运行
                                    case('P'):gl_unFunctionCode=0;break;//暂停
                                    case('C'):gl_unFunctionCode=6;break;//继续
				    case('E'):gl_unFunctionCode=0;break;//终止
				    case('N'):break;//无操作
				    default:break;
				}	
			}
			break;
		case(4)://测线切换?//bswpt
			m_unPointCutInLine=(u16)strtol((char*)&pucDataBuf[punFlagPos[1]],NULL,16);
		  ucAutoHov=m_unPointCutInLine;
			gl_unCtrlState=0;
			break;
		default:break;
	}   
		
}

void WPS84_XY(void)
{
  #define INIT_TIMES	150
	static uint16_t unInitTimes = 0;
	static double dblLat0 = 0, dblLon0 = 0;
	double dblThetaRad, dblDltLat, dblDltLon;
	
	if (unInitTimes < INIT_TIMES)
	{
		dblLat0 = (dblLat0 * unInitTimes + gl_dblDiffLat) / (unInitTimes + 1);
		dblLon0 = (dblLon0 * unInitTimes + gl_dblDiffLon) / (unInitTimes + 1);
		unInitTimes++;
	}

	dblThetaRad = (gl_dblDiffLat + dblLat0) * DEG2RAD * 0.5;
	dblDltLat   = gl_dblDiffLat  - dblLat0;
	dblDltLon   = gl_dblDiffLon  - dblLon0;

	gl_dblPosE = (111412.876 * cos(dblThetaRad) - 93.503 * cos(dblThetaRad * 3.0)) * dblDltLon;
	gl_dblPosN = (111132.952 - 559.849 * cos(dblThetaRad * 2.0)) * dblDltLat;
}
