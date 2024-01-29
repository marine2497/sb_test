#ifndef _DGPS_H_
#define _DGPS_H_

#include "stdlib.h"

extern double gl_dblDiffTime ,gl_dblDiffLat		, 	gl_dblDiffLon ,  gl_dblDiffHorSpd ,gl_dblDiffTrkGnd ;
extern double gl_dblDiffWHeading;
extern double gl_dblDiffWHeadingFilter;
extern double gl_dblDiffPosN	,	gl_dblDiffPosE   , gl_dblDiffHeight		;
extern double gl_dblDiffVeloN	,	gl_dblDiffVeloE  , gl_dblDiffVeloU		;
extern double	gl_dblDiffUTC ,	gl_dblDiffHeading  ,gl_dblDiffRoll  , gl_dblDiffPitch , gl_dblDiffState  ,gl_dblDiffSeeSat , gl_dblDiffDelay ;
extern double gl_dblPosE ,gl_dblPosN ;
extern double gl_dblDiffLatControl		, 	gl_dblDiffLonControl ;//用作控制的经纬度 
extern double gl_dblDiffLatShow		, 	gl_dblDiffLonShow ;//用来显示的经纬度 航迹误差最小的点

void ParseDGPSFrm(unsigned char* pucDataBuf, unsigned int unDataLen);
void ParseDGPSData(unsigned char* pucDataBuf, unsigned  int unDataLen);
void WPS84_XY(void);


#endif

