#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "string.h"
#include "stdio.h"
#include "math.h"

#include "DGPS.h"
//#include "Radio.h"
#include "TargetCtrl.h"
#include <stdint.h>
#include <stdbool.h>


typedef uint32_t   u32;
typedef uint16_t   u16;
typedef uint8_t    u8;

typedef int16_t   s16;


#define LEN_GPSDATA 		64
#define LEN_LATLONCHECK 3
#define LEN_MIDFILTER		3	
#define INITLONLAT_LEN	30

#define LEN_DIFFGPS			18*8
#define LEN_CalibraMotorData    24
#define LEN_OutMotorData        32
#define LEN_DIFFGPSDATA  160
#define MIN2DAD					 0.016666666667

#define RAD2DEG					57.29577951
#define DEG2RAD					0.01745329252	

#define LEN_AHRS				36
#define LEN_GPS					40		
#define LEN_MS5611			12
#define LEN_KS103				8
#define LEN_ADC         16       


#define SUCCESS					1
#define ERROR						0



typedef union UnionData
{
	unsigned char m_pubData[4];
	float m_fData;
} UnnData;

//////////////////////////////////////////////////////////////////////////
typedef struct sKS103Data
{
	float fAtti_KS103;
	float fAttiSpd_KS103;
}StKS103Data;

typedef union uKS103Data
{
	StKS103Data stKS103Data;
	unsigned char pubKS103Data[LEN_KS103];
}UnnKS103Data;

typedef struct sMS5611Data
{
	float fBarAtti;
	float fBarAttiSpd;
	float fBarTempe;
}StMS5611Data;

typedef union uMS5611Data
{
	StMS5611Data stMS5611Data;
	unsigned char pubMS5611Data[LEN_MS5611];
}UnnMS5611Data;


typedef struct sGPSData
{
	float fLon;
	float fLat;
	
	float fPosN;
	float fPosE;
	float fHeight;
	
	float fVeloN;
	float fVeloE;
	float fVeloU;
	
	float fSeeSat;
	float fGpsStatus;
   
		
}StGPSData;

typedef union uGPSData
{
	StGPSData stGpsData;
	unsigned char pubGpsData[LEN_GPS];
}UnnGPSData;

typedef struct sADCData
{
	u16 adc_vaule0;
	u16 adc_vaule1;
	u16 adc_vaule2;
	u16 adc_vaule3;
} StADCData;
typedef union uADCData
{
 StADCData stADCData;
	unsigned char pubADCData[LEN_ADC];
} UnnADCData;
typedef struct sDiffGPSData
{
	double dblDiffTime;
	double dblDiffLat;
	double dblDiffLon;	
	double dblDiffHorSpd;
	double dblDiffTrkGnd;
	
	double dblDiffPosN;
	double dblDiffPosE;
	double dblDiffHeight;	
	double dblDiffVeloN;
	double dblDiffVeloE;
	double dblDiffVeloU;

		
	double dblDiffUTC;     
	double dblDiffHeading;	
	double dblDiffRoll;
	double dblDiffPitch;	  		
	double dblDiffState;
	double dblDiffSeeSat;  
	double dblDiffDelay; 
	
}StDiffGPSData;

typedef union uDiffGPSData
{
	StDiffGPSData stDiffGpsData;
	unsigned char pubDiffGpsData[LEN_DIFFGPS];
}UnnDiffGPSData;
//PWM²¶»ñ
typedef struct sCalibraMotorData
{
	float fPitchManuOper;
	float fRollManuOper;
	float fYawManuOper;
	float fOilManuOper;
	float fModeManuOper;
	float fLockHManuOper;

}StCalibraMotorData;

typedef union uCalibraMotorData
{
	StCalibraMotorData stCalibraMotorData;
	unsigned char punCalibraMotorData[LEN_CalibraMotorData];
}UnnCalibraMotorData;
typedef struct sOutMotorData
{
	float fMotor1;
	float fMotor2;
	float fMotor3;
	float fMotor4;
	float fMotor5;
	float fMotor6;
	float fMotor7;
	float fMotor8;

}StOutMotorData;

typedef union uOutMotorData
{
	StOutMotorData stOutMotorData;
	unsigned char punOutMotorData[LEN_OutMotorData];
}UnnOutMotorData;
//////////////////////////////////////////////////////////////////////////

//global
extern double gl_dblTimerms;
float AssmbFloat(unsigned char * puwDataBuf);
void DisassmbFloat(float fData, unsigned char* pubDataBuf);
float MeanFilter(float* pfData, unsigned int unDataLen);
double dblSmoothFilter(double* pdblData, unsigned char uwLen, double dblDltLimit);
bool DataValid(float* pfData, unsigned int unDataLen, float fDataLimit);
unsigned char DblDataValid(double* pdblData, unsigned int unDataLen, double dblDataLimit);
double Yaw180(double fYaw);
void DisassembleShort(s16 nData, unsigned char* punDataBuf);
void DisassmbDbl(double dblData, unsigned char* pucDataBuf);
double AssembleDbl(unsigned char* pucDataBuf);

float AssembleFlt(unsigned char* pucDataBuf);
short AssembleShort(unsigned char* pucDataBuf);
long AssembleLong(unsigned char* pucDataBuf);
void DisassembleULong(unsigned int ulData, unsigned char* punDataBuf);
unsigned short AssembleUShort(unsigned char* pucDataBuf);

double MeanDataFilter(double* pdblData,unsigned char uwDataLen);
double DblDataLimit(double dblData, double dblDataMin, double dblDataMax);
float MidFilter(float* pdblData, unsigned int unDataLen);
float DblMeanFilter(float* pfData, unsigned int unDataLen);
extern float gl_fRollManuOper 	, 	gl_fPitchManuOper 	,	gl_fYawManuOper , 	gl_fOilManuOper , gl_fLockHManuOper 	, gl_fModeManuOper;
extern float gl_fMotor1 ,	gl_fMotor2 , gl_fMotor3 , gl_fMotor4 , gl_fMotor5 , gl_fMotor6 , gl_fMotor7 , gl_fMotor8 ;
extern double gl_dblYaw0 		, gl_dblDistYawError , gl_dblDist2NextPt, gl_dblDistYawErrorRate,	gl_dblDistYawErrorLast;
extern float gl_fAutoOilManuOper, AutoOilIncrement,gl_fAutoOilManuOper1;
extern float gl_fTargetSpeed ;
extern float gl_fSpeed ;
extern float gl_fSpeedFilter;
//DGPS

extern double gl_dblDiffLat		, 	gl_dblDiffLon , gl_dblDiffTrkGnd , gl_dblDiffHorSpd ;
extern double gl_dblDiffNumSat , gl_dblDiffNavMode ;
extern double	gl_dblDiffUTC ,	gl_dblDiffHeading  ,gl_dblDiffPitch ,gl_dblDiffRoll ,gl_dblDiffSVNumber  ,gl_dblDiffDataAge ,gl_dblDiffRSID ;
u8 CRC8Valid(u8* puchData, int len) ;
/////
double DataIn360(double data);

#endif

