#include "Global.h"

//global
//main
double gl_dblTimerms = 0;
float gl_fRollManuOper 	= 0, 	gl_fPitchManuOper 	= 0,	gl_fYawManuOper =	0, 	gl_fOilManuOper = 0, gl_fLockHManuOper 	= 0, gl_fModeManuOper;
float gl_fMotor1 = 0,	gl_fMotor2 = 0, gl_fMotor3 = 0, gl_fMotor4 = 0, gl_fMotor5 = 0, gl_fMotor6 = 0,gl_fMotor7 = 0,gl_fMotor8 = 0;
float gl_fAutoOilManuOper =0, AutoOilIncrement=0,gl_fAutoOilManuOper1=0;
float gl_fTargetSpeed =0;
float gl_fSpeed=0;
float gl_fSpeedFilter=0;
//global
void DisassembleShort(s16 nData, u8* punDataBuf)
{
	punDataBuf[0] =		   nData & 0x00ff;
	punDataBuf[1] = (nData >> 8) & 0x00ff;
}

float AssmbFloat(unsigned char* puwDataBuf)
{
	UnnData nData;
	unsigned char i;
	
	for (i = 0; i < 4; i++)
		nData.m_pubData[i] = (unsigned char)(puwDataBuf[i] & 0x00ff);
	
	return nData.m_fData;
}

void DisassmbFloat(float fData, unsigned char* pubDataBuf)
{
	UnnData nData;
	unsigned char i;

	nData.m_fData = fData;
	
	for (i = 0; i < 4; i++)
		pubDataBuf[i] = nData.m_pubData[i];
}

float MeanFilter(float* pfData, unsigned int unDataLen)
{
	unsigned int i;
	float fRtnVal = 0;

	if (unDataLen == 0)
		return 0;

	for (i = 0; i < unDataLen; i++)
		fRtnVal += pfData[i];
	fRtnVal /= unDataLen;
	
	return fRtnVal;
}

float DblMeanFilter(float* pfData, unsigned int unDataLen)
{
	unsigned int i;
	float fRtnVal = 0;

	if (unDataLen == 0)
		return 0;

	for (i = 0; i < unDataLen; i++)
		fRtnVal += pfData[i];
	fRtnVal /= unDataLen;
	
	return fRtnVal;
}

unsigned char DblDataValid(double* pdblData, unsigned int unDataLen, double dblDataLimit)
{
	unsigned int i;

	if (unDataLen <= 1)
		return 1;
	
	for (i = 0; i < (unDataLen - 1); i++)
	{
		if (fabs(pdblData[i] - pdblData[i + 1]) > dblDataLimit)
			return 0;
	}

	return 1;
}

bool DataValid(float* pfData, unsigned int unDataLen, float fDataLimit)
{
	unsigned int i;

	if (unDataLen <= 1)
		return true;
	
	for (i = 0; i < (unDataLen - 1); i++)
	{
		if (fabs(pfData[i] - pfData[i + 1]) > fDataLimit)
			return false;
	}

	return true;
}

double Yaw180(double fYaw)
{
	if (fYaw > 180)
		fYaw -= 360;
	else if (fYaw <  -180)
		fYaw += 360;
	if (fYaw > 180)
		fYaw -= 360;
	else if (fYaw <  -180)
		fYaw += 360;
	return fYaw;
}

short AssembleShort(unsigned char* pucDataBuf)
{
	short sRtnData;

	sRtnData   = pucDataBuf[1] & 0x00ff;
	sRtnData <<= 8;
	sRtnData  |= pucDataBuf[0] & 0x00ff;
	
	return sRtnData;
}

unsigned short AssembleUShort(unsigned char* pucDataBuf)
{
	unsigned short usRtnData;

	usRtnData   = pucDataBuf[1] & 0x00ff;
	usRtnData <<= 8;
	usRtnData  |= pucDataBuf[0] & 0x00ff;
	
	return usRtnData;
}
long AssembleLong(unsigned char* pucDataBuf)
{
	long lData;
	
	lData	= pucDataBuf[3] & 0x00ff;
	lData <<= 8;
	lData  |= pucDataBuf[2] & 0x00ff;
	lData <<= 8;
	lData  |= pucDataBuf[1] & 0x00ff;
	lData <<= 8;
	lData  |= pucDataBuf[0] & 0x00ff;
	
	return lData;
}
void DisassembleULong(u32 ulData, u8* punDataBuf)
{
	punDataBuf[0] =		   ulData  & 0x00ff;
	punDataBuf[1] = (ulData >> 8)  & 0x00ff;
	punDataBuf[2] = (ulData >> 16) & 0x00ff;
	punDataBuf[3] = (ulData >> 24) & 0x00ff;
}
float AssembleFlt(unsigned char* pucDataBuf)
{
	union
	{
		unsigned long ulData;
		float fData;
	} cData;

	cData.ulData   = pucDataBuf[3] & 0x00ff;
	cData.ulData <<= 8;
	cData.ulData  |= pucDataBuf[2] & 0x00ff;
	cData.ulData <<= 8;
	cData.ulData  |= pucDataBuf[1] & 0x00ff;
	cData.ulData <<= 8;
	cData.ulData  |= pucDataBuf[0] & 0x00ff;

	return cData.fData;
}

void DisassmbDbl(double dblData, unsigned char* pucDataBuf)
{
	int i=0;
	union
	{
		unsigned char pucDataBuf[8];
		double dblData;
	} cData;

	cData.dblData = dblData;
	
	for (i = 0; i < 8; i++)
		pucDataBuf[i] = cData.pucDataBuf[i];
}
double AssembleDbl(unsigned char* pucDataBuf)
{
	int i=0;
	union
	{
		unsigned char pucDataBuf[8];
		double dblData;
	} cData;	 
	for( i = 0; i < 8; i++)
	{
		cData.pucDataBuf[i] = pucDataBuf[i];
	}
	return cData.dblData;
}
double MeanDataFilter(double* pdblData, unsigned char uwDataLen)
{
	unsigned char i = 0, j = 0;
	double dblData = 0;
	double dblRtnVal = 0;

	if (uwDataLen == 0)
		return 0;
	
	for(i = 0; i < uwDataLen; i++)
	{
		for(j = 0; j < uwDataLen - 1; j++)
		{
			if(pdblData[j] > pdblData[j + 1])
			{
				dblData = pdblData[j];
				pdblData[j] = pdblData[j + 1];
				pdblData[j + 1] = dblData;
			}
		}
	}
	
	for (i = 1; i < uwDataLen - 1; i++)
		dblRtnVal += pdblData[i];
	dblRtnVal /= (uwDataLen - 2);
	
  return dblRtnVal;
}	

double DblDataLimit(double dblData, double dblDataMin, double dblDataMax)
{
	double dblResult = 0;
	dblResult = dblData;
	if(dblData < dblDataMin)
		dblResult = dblDataMin;
	if(dblData > dblDataMax)
		dblResult = dblDataMax;
	return dblResult;
}

float MidFilter(float* pdblData, unsigned int unDataLen)
{
	float pdblSrcData[8];
	unsigned int  i, j;
	float dblRtnVal = 0, dblDataTmp;

	if (unDataLen < 3)
		return pdblData[0];

	for (i = 0; i < unDataLen; i++)
		pdblSrcData[i] = pdblData[i];
	for (i = 0; i < (unDataLen - 1); i++)
	{
		for (j = 0; j < (unDataLen - 1 - i); j++)
		{
			if (pdblSrcData[j] > pdblSrcData[j + 1])
			{
				dblDataTmp = pdblSrcData[j + 1];
				pdblSrcData[j + 1] = pdblSrcData[j];
				pdblSrcData[j] = dblDataTmp;
			}
		}
	}

	j = unDataLen / 2;
	dblRtnVal = DblMeanFilter(&pdblSrcData[j - 1], 3);
	
	return dblRtnVal;
}
double DataIn360(double dblData)
{
	if(dblData < 0)
		dblData = dblData + 360;
	else if(dblData > 360)
		dblData = dblData - 360;
	
	return dblData;
}
u8 CRC8Valid(u8* puchData, int len) 
{      
	u8 crc8 = 0;     
	while (len--)     
	{          
	crc8 = crc8 ^ (*puchData++);     
	}         
	return crc8; 
}

