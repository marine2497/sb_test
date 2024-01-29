#include "Global.h"                                               


/*�켣������ �Ƕ� ���� */
double dblX0, dblY0, dblX1, dblY1, dblX, dblY, dblYaw0,dblYaw0_Last;	//��������XY��
double dlbR,dlbRAngle;//����Բ�뾶����λ��
double dblPointAngle;//��ǰ�㷽λ��
double dblX00,dblY00;//Բ������
double dblLatStart = 0, dblLonStart = 0, dblLatDest = 0, dblLonDest = 0; //��ʼ�����㡢��ֹ������
double dblLatStart_next = 0, dblLonStart_next = 0, dblLatDest_next = 0, dblLonDest_next = 0; //��ʼ�����㡢��ֹ������
double gl_dblDistYawError = 0.0, gl_dblDist2NextPt = 0.0 , gl_dblStart2NextPt=0.0;//ƫ���� ���о� �г�
unsigned char ucAutoHov =0.0,ucAutoHov_Last =0.0; //��ǰ������ �ϴκ�����
int ncount=0;//����

//ָ��
double gl_dblYaw0 		= 0,gl_dblYawCutLine=0.0;//��ǰָ��  Բ������
double gl_dblLonD[100] , gl_dblLatD[100] ;//Ŀ�꾭γ��
double gl_dblCutLineAngle=0,gl_dblCutLineAngleControl=0.0;//���� 


/*���Բ���*/
//double gl_dblLockYaw_Kp = 0.03, gl_dblLockYaw_Ki = 0.0, gl_dblLockYaw_Kd =  0.06;//ֱ�߶�PID 
double gl_dblLockYaw_Kp = 0.03, gl_dblLockYaw_Ki = 0.0, gl_dblLockYaw_Kd =  0.06;//ֱ�߶�PID 
double gl_dblTurning_Kp = 0.03, gl_dblTurning_Ki = 0.0, gl_dblTurning_Kd =  0.03; //ת���PID
//double gl_dblYawDis_Kp = 10, gl_dblYawDis_Ki = 0, gl_dblYawDis_Kd = 19;//ƫ��������PID
double gl_dblYawDis_Kp = 18, gl_dblYawDis_Ki = 0, gl_dblYawDis_Kd = 19;//ƫ��������PID
double gl_dblSpeed_Kp = 0.6, gl_dblSpeed_Ki = 0.00, gl_dblSpeed_Kd = 10;//�ٶȻ�PID
double gl_dblRatio=0;//0.5;//��ٶ����Ʊ�K
//double gl_dblSetSpeed=1.5;//����ֱ�߶��ٶ�
double gl_dblSetSpeed=1.5;//����ֱ�߶��ٶ�

/*Ԥ�����Բ���*/
double gl_dblLockYaw = 0.0,gl_dblLockYaw_a = 0.0,gl_dblLockYaw_T = 0.0;// ֱ�߶θ������� ��ǰ����ϵ��
double gl_dblSpeedDownLine = 2;//������
//double gl_dblTurnRadius = 5;//ֱ��ת��뾶
double gl_dblTurnRadius = 5;//ֱ��ת��뾶
double gl_dblSetTurnTime = 1 ;//����ת��ʱ��
double gl_dblPos_Kp = 0, gl_dblPos_Ki = 0, gl_dblPos_Kd = 0;//λ�û�PID
double gl_dblDiffMotorRatio = 1;//���ǰ�����������
double dt=0.0;//ת��ʱ��
/*��־λ*/
u8  gl_unAutoFlag     = 0;
u8  gl_unPointAll     = 0;
u8  gl_unFunctionCode = 0;	
u8	gl_unFlyMode_SET  = 0;		
u8	gl_unCTRLPARA_SET = 0;		
u8  gl_unTurningFlag   = 0;//�Ƿ�ת��
u8  gl_unSpeedDownFlag   = 0;//�Ƿ񵽴���ٴ�
u8  gl_unTargetReachFlag   = 0;//�Ƿ񵽴�Ŀ�ĵ�
u8  gl_unCtrlState     = 0; //����״̬
u8  gl_unLockState=0;//Ĭ�Ͻ���
//ARHS 
float gl_fPitch 		= 0, gl_fRoll 		= 0, gl_fYaw 		= 0, gl_fYaw_1  = 0,gl_fYawLast = 0;
float gl_fPitchRate 	= 0, gl_fRollRate 	= 0, gl_fYawRate	= 0;
float gl_fMagYaw		= 0;
float gl_fAccZ_Earth_Filter = 0.0;
float fAccX_Earth		= 0, fAccY_Earth	= 0, fAccZ_kjEarth	= 0;
//GPS 
double gl_dblLon0		= 0, gl_dblLat0		= 0;   //��ʼ��γ��
//motor
double gl_LeftMotorControl = 0.0,gl_RightMotorControl = 0.0;
double gl_MFCLeftMotorControl = 0.0,gl_MFCRightMotorControl = 0.0;

MyPIDControl myPIDHeadingControl;
MyPIDControl myPIDTrackControl;
MyPIDControl myPIDSpeedControl;
MyPIDControl myPIDPosControl;
//MyPIDControl myPIDHeadingControlwYaw;
//ƫ������о���㺯��
void CalcDistYawError2NextWP(double dblX, double dblY, double dblX0, double dblY0, double dblX1, double dblY1, double* pdblDistYawError, double* pdblDist2NextWP)
{	
	double x = dblX - dblX0;
	double y = dblY - dblY0;
	double xe = dblX1 - dblX0;
	double ye = dblY1 - dblY0;
	double dist0 = sqrt(xe * xe + ye * ye);

	if (dist0 > 0.1)
	{
		*pdblDistYawError = (xe * y - ye * x) / dist0;
		*pdblDist2NextWP = dist0 - (ye * y + xe * x) / dist0;
	}
	else
	{
		*pdblDistYawError = 0;
		*pdblDist2NextWP = 0;
	}
}
void CalcDistYawError2OWP(double dblX, double dblY, double dblX0, double dblY0, double dblX1, double dblY1, double* pdblDistYawError)
{	
	double x = dblX - dblX0;
	double y = dblY - dblY0;
	double xe = dblX1 - dblX0;
	double ye = dblY1 - dblY0;
	double dist0 = sqrt(xe * xe + ye * ye);

	if (dist0 > 0.1)
	{
		*pdblDistYawError = (xe * y - ye * x) / dist0;
	//	*pdblDist2NextWP = dist0 - (ye * y + xe * x) / dist0;
	}
	else
	{
		*pdblDistYawError = 0;
	//	*pdblDist2NextWP = 0;
	}
}
//����ת������
//Lat0  Lon0��ʼ��
//Lat   Lon �յ�
void CalcGrd(double dblLat0, double dblLon0, double dblLat, double dblLon, double *pdblPosX, double *pdblPosY)
{
	double dblThetaRad=0.0, dblDltLat=0.0, dblDltLon=0.0;

	dblThetaRad = (dblLat + dblLat0) * DEG2RAD * 0.5;//γ�Ȼ���
	dblDltLat   = (dblLat  - dblLat0);
	dblDltLon   = (dblLon  - dblLon0);

  *pdblPosX = (111132.952 - 559.849 * cos(2.0 * dblThetaRad)) * dblDltLon ;
  *pdblPosY = (111412.876 * cos(dblThetaRad) - 93.503 * cos(3.0 * dblThetaRad)) * dblDltLat;
/**pdblPosX =(6383487.606 * cos(dblThetaRad) - 5357.31 * cos(3.0 * dblThetaRad)) * dblDltLon;
	*pdblPosY =(6367449.134 - 32077.0 * cos(2.0 * dblThetaRad)) * dblDltLat;*/
}
void InvCalcGrd(double dblLat0, double dblLon0, double *dblLat, double *dblLon, double r, double heading)
{                         
	double dblThetaRad=0.0, dblDltLat=0.0, dblDltLon=0.0;
	dblThetaRad = dblLat0 * DEG2RAD ;
  dblDltLon = (r*sin(heading* dblThetaRad))/(111132.952 - 559.849 * cos(2.0 * dblThetaRad))  ;
  dblDltLat = (r*cos(heading* dblThetaRad))/(111412.876 * cos(dblThetaRad) - 93.503 * cos(3.0 * dblThetaRad)) ;
	*dblLat=dblDltLat+dblLat0;
	*dblLon=dblDltLon+dblLon0;
}
void CalTrackHeading()//����Ԥ������
{
	//ȷ����ǰλ��
	dblX0 = 0;
	dblY0 = 0;
	gl_dblLat0=gl_dblDiffLat;
	gl_dblLon0=gl_dblDiffLon;	
	//ȷ�������յ�
	dblLatStart = gl_dblLatD[ucAutoHov+0];//γ�����y��
	dblLonStart = gl_dblLonD[ucAutoHov+0];//������� X��
	dblLatDest  = gl_dblLatD[ucAutoHov+1];//γ���յ�y��
	dblLonDest  = gl_dblLonD[ucAutoHov+1];//�����յ� X��
	dblYaw0_Last=dblYaw0;                 
	//����Ŀ��켣�ĺ���
	CalcGrd(dblLatStart, dblLonStart, dblLatDest, dblLonDest, &dblX1, &dblY1);//���������յ��X��Y
	dblYaw0 = atan2(dblX1, dblY1) * RAD2DEG ; //Ԥ����������
	if(ucAutoHov==0)//����ʱû���ϴι켣��λ��
		dblYaw0_Last=gl_fYaw;
	
	CalcDistYawError2NextWP(0, 0, 0, 0, dblX1, dblY1, &gl_dblDistYawError, &gl_dblStart2NextPt);//������㵽�յ�ľ���

	if(dblYaw0>360)
		dblYaw0=dblYaw0-360;
	if(dblYaw0<0)
		dblYaw0=dblYaw0+360;

	gl_dblYaw0=dblYaw0;
        //printf("calheading %f %f %f\r\n",dblYaw0,dblYaw0_Last);
        //printf("self %f %f %f %f %f\r\n",dblYaw0,gl_dblDistYawError,gl_dblStart2NextPt,gl_LeftMotorControl,gl_RightMotorControl);
}
void TrackParamSet()
{
    myPIDHeadingControl.Kp=gl_dblLockYaw_Kp;
    myPIDHeadingControl.Ki=gl_dblLockYaw_Ki;
    myPIDHeadingControl.Kd=gl_dblLockYaw_Kd;		
}
void TurningParamSet()
{
		myPIDHeadingControl.Kp=gl_dblTurning_Kp; //ת��PID    
		myPIDHeadingControl.Kd=gl_dblTurning_Kd;
		myPIDHeadingControl.Ki=gl_dblTurning_Ki;	
//		gl_fAutoOilManuOper=0;
//		gl_fTargetSpeed=0;
	      gl_unTurningFlag=1;
}
bool TurningOver()
{

	if(gl_unTurningFlag==1)
	{
            if((fabs(Yaw180(dblYaw0-gl_fYaw))<5))//���С��5��
		{
			ncount++;
		}
		else
			ncount=0;
		if(ncount>=1)//ת�����
		{
			gl_unTurningFlag=0;

			ncount=0;
			return 1;

		}
	}
	return 0;

} 
void CtrlOutput(void)
{
	  gl_LeftMotorControl=gl_fAutoOilManuOper-gl_dblRatio*fabs(myPIDHeadingControl.Out)+myPIDHeadingControl.Out;
	  gl_RightMotorControl=gl_fAutoOilManuOper-gl_dblRatio*fabs(myPIDHeadingControl.Out)-myPIDHeadingControl.Out;
}
void ManuDoubleMotorCtrlMode(void)//�ֶ�����ģʽ0(ֱ�����)
{
	gl_LeftMotorControl=gl_fPitchManuOper;
	gl_RightMotorControl=gl_fOilManuOper;
}
void ManuHeadingOilCtrlMode(void)//�ֶ�����ģʽ1(����+����,�������Ϊ����ת�����)
{
	gl_LeftMotorControl=gl_fOilManuOper+gl_fRollManuOper;
	gl_RightMotorControl=gl_fOilManuOper-gl_fRollManuOper;
}
void ManuHeadingCtrlMode(void)//�ֶ�����ģʽ(�����ֶ����ƣ�ң�ظ�������򣬼��뺽����ƣ��������Ϊ����)
{
	gl_dblLockYaw=XyToHeading(gl_fRollManuOper,gl_fPitchManuOper);
	if(gl_dblLockYaw==-1)
	{
		gl_LeftMotorControl=0;
		gl_RightMotorControl=0;
	}
	else
	{
		YawPID_Control(gl_dblLockYaw,gl_fYaw);
		gl_LeftMotorControl=gl_fOilManuOper+(myPIDHeadingControl.Out>0?myPIDHeadingControl.Out:gl_dblDiffMotorRatio*myPIDHeadingControl.Out);
		gl_RightMotorControl=gl_fOilManuOper+((-myPIDHeadingControl.Out)>0?(-myPIDHeadingControl.Out):(gl_dblDiffMotorRatio*-myPIDHeadingControl.Out));
	}
}
void GrdStationHeadingCtrlMode(void)//�������ģʽ
{
	YawPID_Control(gl_dblLockYaw,gl_fYaw);
//	myPIDHeadingControl.Out=PhaseLeadCtl_fCalc(myPIDHeadingControl.Out);//��ǰ����
	myPIDHeadingControl.Out=DblDataLimit(myPIDHeadingControl.Out,-2,2);
	gl_LeftMotorControl=0.8+(myPIDHeadingControl.Out>0?myPIDHeadingControl.Out:(gl_dblDiffMotorRatio*myPIDHeadingControl.Out));
	gl_RightMotorControl=0.8+((-myPIDHeadingControl.Out)>0?(-myPIDHeadingControl.Out):(gl_dblDiffMotorRatio*-myPIDHeadingControl.Out));
}
void RouteSetting(void)//װ������
{
	ucAutoHov = 0;
		
	dblLatStart = gl_dblLatD[0];//γ�����y��
	dblLonStart = gl_dblLonD[0];//�������X��
	dblLatDest  = gl_dblLatD[1];//γ���յ�y��
	dblLonDest  = gl_dblLonD[1];//�����յ�X��
}
void TrackCtrlTest(void)//�Զ����߿���ģʽ1������ģʽ������Զ�����ģʽ2�����ٶȱջ���
{
    
    //  gl_fTargetSpeed = Get_TargetSpeed(0,gl_dblDist2NextPt,);
    AutoOilIncrement=Speed_Control(gl_fTargetSpeed ,gl_fSpeed);
    gl_fAutoOilManuOper = Oil_Control(0,gl_dblDist2NextPt);//���ſ���							
    CtrlOutput();//�������		
}
void TrackCtrlMode(void)//�Զ����߿���ģʽ2
{
	
	if(gl_unPointAll==0)//δװ������
		return;	
        
        switch(gl_unCtrlState)
	{
		case(0):CalTrackHeading();//����Ԥ������	
		  if(ucAutoHov==ucAutoHov_Last||ucAutoHov==0)
			gl_unCtrlState=2;
		  else
			{
			TurningParamSet();//�趨ת�����
			gl_unCtrlState=1;
			gl_unTurningFlag=1;
			dt=0;
			}
		  ucAutoHov_Last=ucAutoHov;
		  break;
		case(1)://ת���
	          //InvCalcGrd(gl_dblDiffLat, gl_dblDiffLon, &gl_dblDiffLatControl, &gl_dblDiffLonControl,0.1, gl_dblDiffHeading); //��λ��ǰ��
		 // CalcGrd(dblLatStart, dblLonStart, gl_dblDiffLatControl, gl_dblDiffLonControl, &dblX, &dblY);//�Գ�ʼ����Ϊԭ����㴬����ǰXY����

		  CalcGrd(dblLatStart, dblLonStart, gl_dblDiffLat, gl_dblDiffLon, &dblX, &dblY);//�Գ�ʼ����Ϊԭ����㴬����ǰXY����
		  CalcDistYawError2NextWP(dblX, dblY, 0, 0, dblX1, dblY1, &gl_dblDistYawError, &gl_dblDist2NextPt);//����ƫ�о�����о�
		  dlbR=fabs(gl_dblTurnRadius/sin((0.5*Yaw180(dblYaw0-dblYaw0_Last)*DEG2RAD)));//��������Բ�뾶
		  //if(fabs(Yaw180(dblYaw0-dblYaw0_Last))>=90)//ת����90����
		  // {
                     //  if(Yaw180(dblYaw0-dblYaw0_Last)>0)//˳ʱ��ת��
                     //  {						
                     //      dlbRAngle=DataIn360(dblYaw0+0.5*fabs(Yaw180(dblYaw0-dblYaw0_Last)));//��������ԲԲ�ķ�λ��
                     //  }
                     //  else//��ʱ��ת��
                     //  {			
                    //       dlbRAngle=DataIn360(dblYaw0-0.5*fabs(Yaw180(dblYaw0-dblYaw0_Last)));//��������ԲԲ�ķ�λ��
                   //    }                       
                  // }
                 // else //תС����90����
                 // {
                      
                      if(Yaw180(dblYaw0-dblYaw0_Last)>0)//˳ʱ��ת��
                      {
                          dlbRAngle=DataIn360(dblYaw0+0.5*(180-fabs(Yaw180(dblYaw0-dblYaw0_Last))));//��������ԲԲ�ķ�λ��								
                      }
                      else //��ʱ��ת��
                          dlbRAngle=DataIn360(dblYaw0-0.5*(180-fabs(Yaw180(dblYaw0-dblYaw0_Last))));//��������ԲԲ�ķ�λ��		
                  //}
                  dblX00=dlbR*sin(dlbRAngle*DEG2RAD);//����Բ������
                  dblY00=dlbR*cos(dlbRAngle*DEG2RAD);		
                  
                  //printf("turnHeading %f %f %f\r\n",dblYaw0,dblYaw0_Last,dlbRAngle);
                  
                  
                  if(fabs(dblX-dblX00)<0.1)//���㵱ǰ����Բ�ĵķ�λ��
                  {
                      if(dblY-dblY00>0)
                          dblPointAngle=0;
                      else
                          dblPointAngle=180;			
                  }
                  else
                  {
                      if(dblX-dblX00>0&&dblY-dblY00>0)
                          dblPointAngle=90-fabs(atan((dblY-dblY00)/(dblX-dblX00))*RAD2DEG);
                      else if(dblX-dblX00>0&&dblY-dblY00<0)
                          dblPointAngle=90+fabs(atan((dblY-dblY00)/(dblX-dblX00))*RAD2DEG);	
                      else if(dblX-dblX00<0&&dblY-dblY00<0) 
                          dblPointAngle=270-fabs(atan((dblY-dblY00)/(dblX-dblX00))*RAD2DEG);
                      else
                          dblPointAngle=270+fabs(atan((dblY-dblY00)/(dblX-dblX00))*RAD2DEG);
                  }
                  
                  if(Yaw180(dblYaw0-dblYaw0_Last)<0)//��ʱ��
                      gl_dblYawCutLine=DataIn360(dblPointAngle-90);//�������� 
                  else
                      gl_dblYawCutLine=DataIn360(dblPointAngle+90);					
                   
                  
                  
                  if(Yaw180(dblYaw0-dblYaw0_Last)<0)//��ʱ��
                      gl_dblDistYawError=fabs(gl_dblTurnRadius/tan((0.5*Yaw180(dblYaw0-dblYaw0_Last)*DEG2RAD)))-sqrt((dblX-dblX00)*(dblX-dblX00)+(dblY-dblY00)*(dblY-dblY00));//E=L-R	
                  else 
                      gl_dblDistYawError=-fabs(gl_dblTurnRadius/tan((0.5*Yaw180(dblYaw0-dblYaw0_Last)*DEG2RAD)))+sqrt((dblX-dblX00)*(dblX-dblX00)+(dblY-dblY00)*(dblY-dblY00));//E=L-R	
                  
                  gl_dblDistYawError=KalmanFilter_PosY(gl_dblDistYawError,0.9,20);	
                  
                  
                  if(fabs(gl_fYaw-dblYaw0)<45)
                  {
                     if(gl_dblDist2NextPt < gl_dblTurnRadius) //δ����Ŀ��� ��������
                     {                       
						 ucAutoHov ++;
						 if(ucAutoHov>=gl_unPointAll)
						 {
						   //ucAutoHov=1;           //����������ʼ,��                         
						   ucAutoHov=0;         						
						   gl_unPointAll=0;
						   gl_LeftMotorControl=0;
						   gl_RightMotorControl=0;
						 }
						 gl_unCtrlState=0;
						 break;
                     }
                    
                         if(Yaw180(dblYaw0-dblYaw0_Last)>=0&&Yaw180(gl_fYaw-dblYaw0)>0)//˳ʱ��ת�� -5~5
                          {
							   TrackParamSet();
							   gl_unCtrlState=2;//ת�����                                                                        
							   break;
                          }
                          if(Yaw180(dblYaw0-dblYaw0_Last)<0&&Yaw180(gl_fYaw-dblYaw0)<0)//��ʱ��ʱ��ת�� 5~-5
                          {
							   TrackParamSet();
							   gl_unCtrlState=2;//ת�����                                     
							   break;
                          }
                  }
			  gl_dblYaw0=DataIn360(DataIn360(dblYaw0_Last+Yaw180(dblYaw0-dblYaw0_Last)*dt/gl_dblSetTurnTime)+TrackPID_Control(0,gl_dblDistYawError));//�����ô��о��������Ŀ�꺽��	
			  dt+=0.2;
			  if(dt>gl_dblSetTurnTime)dt=gl_dblSetTurnTime;	
					   
			  gl_dblYaw0=gl_dblYawCutLine;				
			  gl_dblYaw0=DataIn360(gl_dblYawCutLine+TrackPID_Control(0,gl_dblDistYawError));//�����ô��о��������Ŀ�꺽��
			  YawPID_Control(gl_dblYaw0,gl_fYaw);//������� ���룺Ŀ�꺽�򣬵�ǰ���� ��������ٵ�������� myPIDHeadingControl.Out	
			  if(fabs(Yaw180(dblYaw0-dblYaw0_Last))<90)//С��100�ȵ���
			  myPIDHeadingControl.Out=myPIDHeadingControl.Out+Yaw180(dblYaw0-dblYaw0_Last)/180*0.8;
			  else//����100�ȵ���
			  myPIDHeadingControl.Out=myPIDHeadingControl.Out+Yaw180(dblYaw0-dblYaw0_Last)/180*1;
                  //gl_fTargetSpeed =gl_dblSetSpeed/2;//Get_TargetSpeed(0,fabsgl_dblDist2NextPt,gl_dblStart2NextPt);//PosPID_Control(0,gl_dblDist2NextPt);
			  gl_fTargetSpeed =gl_dblSetSpeed/2;//Get_TargetSpeed(0,fabsgl_dblDist2NextPt,gl_dblStart2NextPt);//PosPID_Control(0,gl_dblDist2NextPt);
			  gl_fAutoOilManuOper=gl_fTargetSpeed/2+SpeedPID_Control(gl_fTargetSpeed ,gl_fSpeed);	
			  
			  //printf("turnHeading %f %f %f %f %f \r\n",dblYaw0,dblYaw0_Last,dlbRAngle,gl_fAutoOilManuOper,myPIDHeadingControl.Out);
			  
			  CtrlOutput();//�������
                  
                  /*if(fabs(gl_fYaw-dblYaw0)<45)
                  {
                 
                    
                         if(Yaw180(dblYaw0-dblYaw0_Last)>=0&&Yaw180(gl_fYaw-dblYaw0)>0)//˳ʱ��ת�� -5~5
                          {
                                   TrackParamSet();
                                   if(gl_dblDist2NextPt > gl_dblTurnRadius) //δ����Ŀ��� ��������
                                   {
                                     gl_unCtrlState=2;//ת�����                                     
                                   }
                                   else//�л�����
                                   {
                                     ucAutoHov ++;
                                     if(ucAutoHov>=gl_unPointAll)
                                     {
                                       //ucAutoHov=1;           //����������ʼ,��                         
                                       ucAutoHov=0;         						
                                       gl_unPointAll=0;
                                       gl_LeftMotorControl=0;
                                       gl_RightMotorControl=0;
                                     }
                                     gl_unCtrlState=0;                                     
                                   }
                                   break;
                          }
                          if(Yaw180(dblYaw0-dblYaw0_Last)<0&&Yaw180(gl_fYaw-dblYaw0)<0)//��ʱ��ʱ��ת�� 5~-5
                          {
                                   TrackParamSet();
                                   if(gl_dblDist2NextPt > gl_dblTurnRadius) //δ����Ŀ��� ��������
                                   {
                                     gl_unCtrlState=2;//ת�����                                     
                                   }
                                   else//�л�����
                                   {
                                     ucAutoHov ++;
                                     if(ucAutoHov>=gl_unPointAll)
                                     {
                                       //ucAutoHov=1;           //����������ʼ,��                         
                                       ucAutoHov=0;         						
                                       gl_unPointAll=0;
                                       gl_LeftMotorControl=0;
                                       gl_RightMotorControl=0;
                                     }
                                     gl_unCtrlState=0;                                     
                                   }
                                   break;
                          }            
                    
                  }
			
                  gl_dblYaw0=DataIn360(DataIn360(dblYaw0_Last+Yaw180(dblYaw0-dblYaw0_Last)*dt/gl_dblSetTurnTime)+TrackPID_Control(0,gl_dblDistYawError));//�����ô��о��������Ŀ�꺽��	
                  dt+=0.2;
                  if(dt>gl_dblSetTurnTime)dt=gl_dblSetTurnTime;	
                           
                  gl_dblYaw0=gl_dblYawCutLine;				
                  gl_dblYaw0=DataIn360(gl_dblYawCutLine+TrackPID_Control(0,gl_dblDistYawError));//�����ô��о��������Ŀ�꺽��
                  YawPID_Control(gl_dblYaw0,gl_fYaw);//������� ���룺Ŀ�꺽�򣬵�ǰ���� ��������ٵ�������� myPIDHeadingControl.Out	
                  if(fabs(Yaw180(dblYaw0-dblYaw0_Last))<90)//С��100�ȵ���
		  myPIDHeadingControl.Out=myPIDHeadingControl.Out+Yaw180(dblYaw0-dblYaw0_Last)/180*0.8;
		  else//����100�ȵ���
		  myPIDHeadingControl.Out=myPIDHeadingControl.Out+Yaw180(dblYaw0-dblYaw0_Last)/180*1;
                  //gl_fTargetSpeed =gl_dblSetSpeed/2;//Get_TargetSpeed(0,fabsgl_dblDist2NextPt,gl_dblStart2NextPt);//PosPID_Control(0,gl_dblDist2NextPt);
                  gl_fTargetSpeed =gl_dblSetSpeed/2;//Get_TargetSpeed(0,fabsgl_dblDist2NextPt,gl_dblStart2NextPt);//PosPID_Control(0,gl_dblDist2NextPt);
                  gl_fAutoOilManuOper=gl_fTargetSpeed/2+SpeedPID_Control(gl_fTargetSpeed ,gl_fSpeed);					
                  CtrlOutput();//�������
                */
                  
                  break;
               case(2):
                 //InvCalcGrd(gl_dblDiffLat, gl_dblDiffLon, &gl_dblDiffLatControl, &gl_dblDiffLonControl,0.5, gl_dblDiffHeading); //��λ��ǰ��
                 //CalcGrd(dblLatStart, dblLonStart, gl_dblDiffLatControl, gl_dblDiffLonControl, &dblX, &dblY);//�Գ�ʼ����Ϊԭ����㴬����ǰXY����
                 
                 CalcGrd(dblLatStart, dblLonStart, gl_dblDiffLat, gl_dblDiffLon, &dblX, &dblY);//�Գ�ʼ����Ϊԭ����㴬����ǰXY����
                 CalcDistYawError2NextWP(dblX, dblY, 0, 0, dblX1, dblY1, &gl_dblDistYawError, &gl_dblDist2NextPt);//����ƫ�о�����о�
                 gl_dblDistYawError=KalmanFilter_PosY(gl_dblDistYawError,0.9,20);
                 if(gl_dblDist2NextPt > gl_dblTurnRadius) //δ����Ŀ��� ��������
                 {
                     gl_dblYaw0=DataIn360(dblYaw0+TrackPID_Control(0,gl_dblDistYawError));//�����ô��о��������Ŀ�꺽��                     
                     YawPID_Control(gl_dblYaw0,gl_fYaw);//������� ���룺Ŀ�꺽�򣬵�ǰ���� ��������ٵ�������� myPIDHeadingControl.Out													
                     gl_fTargetSpeed = Get_TargetSpeed(0,fabs(gl_dblDist2NextPt),fabs(gl_dblStart2NextPt));//PosPID_Control(0.2,gl_dblDist2NextPt);
                     gl_fAutoOilManuOper = gl_fTargetSpeed/2+SpeedPID_Control(gl_fTargetSpeed ,gl_fSpeed);
                     //gl_fAutoOilManuOper = Oil_Control(0,gl_dblDist2NextPt);//���ſ���						
                     
                  //printf("line %f %f %f %f %f \r\n",dblYaw0,dblYaw0_Last,dlbRAngle,gl_fAutoOilManuOper,myPIDHeadingControl.Out);
                     CtrlOutput();//�������							
                     
                 }
                 else//�л�����
                 {
                     ucAutoHov ++;
                     if(ucAutoHov>=gl_unPointAll)
                     {
                        //ucAutoHov=1;           //����������ʼ,��
                         
                        ucAutoHov=0;         						
						gl_unPointAll=0;
						gl_LeftMotorControl=0;
						gl_RightMotorControl=0;
                     }
                     gl_unCtrlState=0;
                 }
                 break;
          default:break;
          
        }
}
void MFCDoubleMotorCtrlMode(void)//�ֶ�����ģʽ0(ֱ�����)
{
	gl_LeftMotorControl=gl_MFCLeftMotorControl;
	gl_RightMotorControl=gl_MFCRightMotorControl;
}
void AutoCtrl(void)	
{
	
	switch (gl_unFunctionCode)
	{
		case 0://�ֶ�˫�������
		{ 
			if(PWM_OUT==1)
			{
				ManuDoubleMotorCtrlMode();
				//gl_LeftMotorControl=1;//���ģʽ
				//gl_RightMotorControl=1;
			}
		}
		break;
		
		case 1://�ֶ�����
		{
			ManuDoubleMotorCtrlMode();
		}
		break;
		
		case 2://ң�����������ģʽ
		{
			ManuHeadingCtrlMode();
		}	
		break;						
				
		case 3://��λ���������ģʽ
		{		
			GrdStationHeadingCtrlMode();
		}
		break;
		
		case 4 ://װ������
		{
			 RouteSetting();
		}
		break;
		
		case 5 ://�ߺ�����ʽ2(�����ٶȱջ�����)
		{			
			TrackCtrlTest();
		}
			break;
		
		case 6://�ߺ���
		{
			TrackCtrlMode();
		}
		break;
		case 7://��λ��ֱ�ӿ���
		{
			MFCDoubleMotorCtrlMode();
		}
		break;
		default:
		break;
	}
}

double Oil_Control(double target_pt,double now_ptf)
{
	double oil;
	if((now_ptf-target_pt)>3*gl_dblSpeedDownLine) //��Ŀ���������6��
		oil=0.8;
	else if((now_ptf-target_pt)>1*gl_dblSpeedDownLine)//��Ŀ���������2��С��6��
		oil = 0.6;
	else if((now_ptf-target_pt)<=1*gl_dblSpeedDownLine) //��Ŀ������С��2��
		oil=0;
	return oil;
}

void TargetShipCtrl(void)    //2017.2.24��
{	
	//IN:gl_fPitchManuOper,gl_fOilManuOper,gl_fYawManuOper,gl_fModeManuOper,gl_fRollManuOper
	//OUT: gl_fMotor1,gl_fMotor2,gl_fMotor3,gl_fMotor4��gl_fMotor5��gl_fMotor6��gl_fMotor7��gl_fMotor8

	  AutoCtrl();
          
	  gl_LeftMotorControl=DblDataLimit(gl_LeftMotorControl,-1,1);
	  gl_RightMotorControl=DblDataLimit(gl_RightMotorControl,-1,1);
	  
	  
	 // if(gl_unFunctionCode!=0)
	Motor_Control(gl_LeftMotorControl,gl_RightMotorControl);
}

void Motor_Control(double left,double right)
{
	gl_fMotor1=DblDataLimit(-right*500+1500,1000,2000);
	gl_fMotor2=DblDataLimit(-left*500+1500,1000,2000);
}
double XyToHeading(double x,double y)
{
	double z=0;
	double pi=3.1415926;
	if(x*x+y*y<0.5*0.5)
		return -1;
	

	
	if(x>0&&y>0)
		z=90-atan(y/x)*180/pi;
	else if(x>0&&y<0)
		z=90+atan((y)/(-x))*180/pi;
	else if(x<0&&y<0)
		z=270-atan(y/x)*180/pi;
	else if(x<0&&y>0)
		z=270+atan(-y/x)*180/pi;
	
	if(z>360)
		z=360;
	else if(z<0)
		z=0;

	return z;
}
/*
					                  1  aTs+1
��ǰ�������ݺ���Ϊ					_  _____
					                  1  Ts+ 1
��ɢ���ʽ��y=��(x-x_1)T+x*t/a+y_1*T)/(T+t)  //tΪ��������

*/
float PhaseLeadCtl_fCalc(float fCtlIn)              
{
	static float fCtlOut = 0, fCtlOut_1 = 0, fCtlIn_1 = 0, a = 0.0, T = 0.0, t = 0.2;
	a=gl_dblLockYaw_a;
	T=gl_dblLockYaw_T;
	/*
	fCtlOut = ((fCtlIn - gl_pcCtlInfo[ubCtlNo].m_fMemParam[0]) * gl_pcCtlInfo[ubCtlNo].m_fCtlParam[0] + fCtlIn * CTRL_TIME / gl_pcCtlInfo[ubCtlNo].m_fCtlParam[1] + gl_pcCtlInfo[ubCtlNo].m_fMemParam[1] * gl_pcCtlInfo[ubCtlNo].m_fCtlParam[0]) / (gl_pcCtlInfo[ubCtlNo].m_fCtlParam[0] + CTRL_TIME);
	gl_pcCtlInfo[ubCtlNo].m_fMemParam[0] = fCtlIn;
	gl_pcCtlInfo[ubCtlNo].m_fMemParam[1] = fCtlOut;*/
	fCtlOut = ((fCtlIn - fCtlIn_1) * a* T + fCtlIn * t + fCtlOut_1 * a ) / (T + t);
	fCtlIn_1 = fCtlIn;
	fCtlOut_1 = fCtlOut;
	return fCtlOut;
}
double YawPID_Control(double target_heading,double heading)
{
	myPIDHeadingControl.M_1=myPIDHeadingControl.M;
	myPIDHeadingControl.M=heading;
	myPIDHeadingControl.dE_2=myPIDHeadingControl.dE_1;
	myPIDHeadingControl.dE_1=myPIDHeadingControl.dE;
	myPIDHeadingControl.dE=Yaw180(target_heading-heading);
/*	if(myPIDHeadingControl.dE>45)
		myPIDHeadingControl.dE=45;
	else	if(myPIDHeadingControl.dE<-45)
		myPIDHeadingControl.dE=-45;*/
	if(myPIDHeadingControl.dE<10&&myPIDHeadingControl.dE>-10)                   //���ַ���
	{
		myPIDHeadingControl.SumdE+=myPIDHeadingControl.dE;
	}
	else
		myPIDHeadingControl.SumdE=0;
		if(myPIDHeadingControl.SumdE>1000)//�����ֱ���
		{
			myPIDHeadingControl.SumdE=1000;
		}
		if(myPIDHeadingControl.SumdE<-1000)//�����ֱ���
		{
			myPIDHeadingControl.SumdE=-1000;
		}	
	
	myPIDHeadingControl.Out=myPIDHeadingControl.Kp*myPIDHeadingControl.dE+myPIDHeadingControl.Ki*(myPIDHeadingControl.SumdE)+myPIDHeadingControl.Kd*Yaw180(myPIDHeadingControl.M_1-heading);
	if(myPIDHeadingControl.Out>2)
		myPIDHeadingControl.Out=2;
	else if(myPIDHeadingControl.Out<-2)
		myPIDHeadingControl.Out=-2;
	return myPIDHeadingControl.Out;
}
double TrackPID_Control(double target_track,double track)
{
//  double left=0,right=0;
	myPIDTrackControl.M_1=myPIDTrackControl.M;
	myPIDTrackControl.M=track;
	myPIDTrackControl.dE_2=myPIDTrackControl.dE_1;
	myPIDTrackControl.dE_1=myPIDTrackControl.dE;
	myPIDTrackControl.dE=track-target_track;
	if(myPIDTrackControl.dE>5)
		myPIDTrackControl.dE=5;
	else	if(myPIDTrackControl.dE<-5)
		myPIDTrackControl.dE=-5;
	if(myPIDTrackControl.dE<1&&myPIDTrackControl.dE>-1)                   //���ַ���
	{
		myPIDTrackControl.SumdE+=myPIDTrackControl.dE;
	}
	else
            myPIDTrackControl.SumdE=0;
        if(myPIDTrackControl.SumdE>10)//�����ֱ���
        {
            myPIDTrackControl.SumdE=10;
        }
        if(myPIDTrackControl.SumdE<-10)//�����ֱ���
        {
            myPIDTrackControl.SumdE=-10;		}	
	
	myPIDTrackControl.Out=myPIDTrackControl.Kp*myPIDTrackControl.dE+myPIDTrackControl.Ki*(myPIDTrackControl.SumdE)+myPIDTrackControl.Kd*(myPIDTrackControl.M_1-track);
	if(myPIDTrackControl.Out>180)
		myPIDTrackControl.Out=180;
	else if(myPIDTrackControl.Out<-180)
		myPIDTrackControl.Out=-180;
	return myPIDTrackControl.Out;
}
double SpeedPID_Control(double target_speed,double speed)
{
//  double left=0,right=0;
	myPIDSpeedControl.M_1=myPIDSpeedControl.M;
	myPIDSpeedControl.M=speed;
	myPIDSpeedControl.dE_2=myPIDSpeedControl.dE_1;
	myPIDSpeedControl.dE_1=myPIDSpeedControl.dE;
	myPIDSpeedControl.dE=target_speed-speed;
	if(myPIDSpeedControl.dE>0.5)
		myPIDSpeedControl.dE=0.5;
	else	if(myPIDSpeedControl.dE<-0.5)
		myPIDSpeedControl.dE=-0.5;
	if(myPIDSpeedControl.dE<0.8&&myPIDSpeedControl.dE>-0.8)                   //���ַ���
	{
		myPIDSpeedControl.SumdE+=myPIDSpeedControl.dE;
	}

		if(myPIDSpeedControl.SumdE>10)//�����ֱ���
		{
			myPIDSpeedControl.SumdE=10;
		}
		if(myPIDSpeedControl.SumdE<-10)//�����ֱ���
		{
			myPIDSpeedControl.SumdE=-10;
		}	
	
	myPIDSpeedControl.Out=myPIDSpeedControl.Kp*myPIDSpeedControl.dE+myPIDSpeedControl.Ki*(myPIDSpeedControl.SumdE)+myPIDSpeedControl.Kd*(myPIDSpeedControl.M_1-speed);
	if(myPIDSpeedControl.Out>1)
		myPIDSpeedControl.Out=1;
	else if(myPIDSpeedControl.Out<-1)
		myPIDSpeedControl.Out=-1;
	return myPIDSpeedControl.Out;
}
double PosPID_Control(double target_pos,double pos)
{
//  double left=0,right=0;
	myPIDPosControl.M_1=myPIDPosControl.M;
	myPIDPosControl.M=pos;
	myPIDPosControl.dE_2=myPIDPosControl.dE_1;
	myPIDPosControl.dE_1=myPIDPosControl.dE;
	myPIDPosControl.dE=target_pos-pos;
	if(myPIDPosControl.dE>10)
		myPIDPosControl.dE=10;
	else	if(myPIDPosControl.dE<-10)
		myPIDPosControl.dE=-10;
	if(myPIDPosControl.dE<5&&myPIDPosControl.dE>-5)                   //���ַ���
	{
		myPIDPosControl.SumdE+=myPIDPosControl.dE;
	}
	else
		myPIDPosControl.SumdE=0;
		if(myPIDPosControl.SumdE>100)//�����ֱ���
		{
			myPIDPosControl.SumdE=100;
		}
		if(myPIDPosControl.SumdE<-100)//�����ֱ���
		{
			myPIDPosControl.SumdE=-100;
		}	
	
	myPIDPosControl.Out=myPIDPosControl.Kp*myPIDPosControl.dE+myPIDPosControl.Ki*(myPIDPosControl.SumdE)+myPIDPosControl.Kd*(myPIDPosControl.dE-myPIDPosControl.dE_1);
	if(myPIDPosControl.Out>1)
		myPIDPosControl.Out=1;
	else if(myPIDPosControl.Out<-1)
		myPIDPosControl.Out=-1;
	return myPIDPosControl.Out;
}
double Get_Speed()
{
/*  static double dblPtLast;
	static double dblTime = 0,dlbTimeLast = 0;
	static bool bStartFlag = 1;
	static double speed=0;
	dblTime = gl_dblTimerms;
	if(bStartFlag == 1)
	{
		dlbTimeLast = dblTime;
		dblPtLast = pt;
		bStartFlag = 0;
	}
	if(fabs(dblTime-dlbTimeLast) > 200)
		speed=(pt-dblPtLast)/(dblTime-dlbTimeLast);
	dlbTimeLast = dblTime;
	dblPtLast = pt;
	*/
	static double dblDiffHorSpd=0;
	dblDiffHorSpd=0.2*dblDiffHorSpd+0.8*gl_dblDiffHorSpd;
	
	return KalmanFilter_Speed(dblDiffHorSpd*0.514,0.2,20);
}

float KalmanFilter_Speed(const float ResrcData,float ProcessNiose_Q,float MeasureNoise_R)
{
        float R = MeasureNoise_R;
        float Q = ProcessNiose_Q;

        static float x_last = 0;

        float x_mid = x_last;
        float x_now;

        static float p_last = 0;

        float p_mid ;
        float p_now;
        float kg;

        x_mid = x_last;
        p_mid = p_last + Q;
        kg		= p_mid	/	(p_mid+R);
        x_now	= x_mid + kg*(ResrcData-x_mid);

        p_now=(1-kg)*p_mid;

        p_last = p_now;
        x_last = x_now;

        return x_now;
}
float KalmanFilter_PosX(const float ResrcData,float ProcessNiose_Q,float MeasureNoise_R)
{
       	float R = MeasureNoise_R;
        float Q = ProcessNiose_Q;

        static float posx_last = 0;

        float x_mid = posx_last;
        float x_now;

        static float px_last = 0;

        float p_mid ;
        float p_now;
        float kg;

        x_mid = posx_last;
        p_mid = px_last + Q;
        kg		= p_mid	/	(p_mid+R);
        x_now	= x_mid + kg*(ResrcData-x_mid);

        p_now=(1-kg)*p_mid;

        px_last = p_now;
        posx_last = x_now;

        return x_now;
}

float KalmanFilter_PosY(const float ResrcData,float ProcessNiose_Q,float MeasureNoise_R)
{
		float R = MeasureNoise_R;
        float Q = ProcessNiose_Q;

        static float posy_last = 0;

        float x_mid = posy_last;
        float x_now;

        static float py_last = 0;

        float p_mid ;
        float p_now;
        float kg;

        x_mid = posy_last;
        p_mid = py_last + Q;
        kg		= p_mid	/	(p_mid+R);
        x_now	= x_mid + kg*(ResrcData-x_mid);

        p_now=(1-kg)*p_mid;

        py_last = p_now;
        posy_last = x_now;

        return x_now;
}
double Speed_Control(double target_speed,double now_speed)
{
	float fCtlOut = 0,fCtlIn=0;
	static float KP=0.05,KI=0,KD=0,T=0.2;
	static float  fCtlInSum=0,fCtlInLast=0;
	KP=gl_dblLockYaw_a;	
	KI=gl_dblLockYaw_T;
	fCtlIn=target_speed-now_speed;
	fCtlInSum += fCtlIn;
	fCtlOut = KP*fCtlIn   + KI* fCtlInSum * T + (fCtlIn - fCtlInLast) * KD / T;
	fCtlInLast = fCtlIn;
	if(fCtlOut>1)
		fCtlOut=1;
	else if(fCtlOut<-1)
		fCtlOut=-1;
	return fCtlOut;

}
double Get_TargetSpeed(double target_pt,double now_pt,double all_pt)
{
	//gl_dblTimerms ֮�����
	//gl_dblYaw0,gl_fYawĿ�꺽����
	double TargetSpeed=0;             
	double left_pt=now_pt-target_pt;
	double L1=2.5,Speed1=gl_dblSetSpeed/2;
	double L2=3.5;//,Speed2=Speed3~Speed1;
	double L3=4.5,Speed3=gl_dblSetSpeed;
	if(all_pt<2*L2)
		TargetSpeed=Speed1;
	else
	{
		if((all_pt-now_pt)<L2||(now_pt)<L2)//�Ѿ���ʻ�ľ��� δ��ʻ�ľ���
			TargetSpeed=Speed1;
		else
		{
			if(all_pt-now_pt<now_pt)//ǰ���	
			{
                            if(all_pt-now_pt<L2) //���ٶ�
                                TargetSpeed=Speed1+(Speed3-Speed1)*(all_pt-now_pt-L1)/(L2-L1);
                            else  
                                TargetSpeed=Speed3;//���ٶ�
			
			}
			else //����
			{
                            if(now_pt<L3) //���ٶ�
                                TargetSpeed=Speed1+(Speed3-Speed1)*(now_pt-L2)/(L3-L2);
                            else  
                                TargetSpeed=Speed3;//���ٶ�			
			
			
			}				
		}
	}
	
	if(TargetSpeed<Speed1)TargetSpeed=Speed1;
	if(TargetSpeed>Speed3)TargetSpeed=Speed3;
	return TargetSpeed;
}
void SetHeadingPID(double Kp,double Ki,double Kd)
{
	myPIDHeadingControl.Kp=Kp;
	myPIDHeadingControl.Ki=Ki;
	myPIDHeadingControl.Kd=Kd;
	myPIDHeadingControl.dE=0;
	myPIDHeadingControl.dE_1=0;
	myPIDHeadingControl.dE_2=0;
	myPIDHeadingControl.SumdE=0;
	myPIDHeadingControl.M=0;	
	myPIDHeadingControl.M_1=0;
	myPIDHeadingControl.Out=0;
}
void SetTrackPID(double Kp,double Ki,double Kd)
{
	myPIDTrackControl.Kp=Kp;
	myPIDTrackControl.Ki=Ki;
	myPIDTrackControl.Kd=Kd;
	myPIDTrackControl.dE=0;
	myPIDTrackControl.dE_1=0;
	myPIDTrackControl.dE_2=0;
	myPIDTrackControl.SumdE=0;
	myPIDTrackControl.M=0;	
	myPIDTrackControl.M_1=0;
	myPIDTrackControl.Out=0;
}
void SetSpeedPID(double Kp,double Ki,double Kd)
{
	myPIDSpeedControl.Kp=Kp;
	myPIDSpeedControl.Ki=Ki;
	myPIDSpeedControl.Kd=Kd;
	myPIDSpeedControl.dE=0;
	myPIDSpeedControl.dE_1=0;
	myPIDSpeedControl.dE_2=0;
	myPIDSpeedControl.SumdE=0;
	myPIDSpeedControl.M=0;	
	myPIDSpeedControl.M_1=0;
	myPIDSpeedControl.Out=0;
}
void SetPosPID(double Kp,double Ki,double Kd)
{
	myPIDPosControl.Kp=Kp;
	myPIDPosControl.Ki=Ki;
	myPIDPosControl.Kd=Kd;
	myPIDPosControl.dE=0;
	myPIDPosControl.dE_1=0;
	myPIDPosControl.dE_2=0;
	myPIDPosControl.SumdE=0;
	myPIDPosControl.M=0;	
	myPIDPosControl.M_1=0;
	myPIDPosControl.Out=0;
}
void Parameter_Init(void)
{
	SetHeadingPID(gl_dblLockYaw_Kp,gl_dblLockYaw_Ki,gl_dblLockYaw_Kd);
	SetTrackPID(gl_dblYawDis_Kp,gl_dblYawDis_Ki,gl_dblYawDis_Kd);
	SetSpeedPID(gl_dblSpeed_Kp,gl_dblSpeed_Ki,gl_dblSpeed_Kd);
	//SetPosPID(gl_dblPos_Kp,gl_dblPos_Ki,gl_dblPos_Kd);
	gl_unCtrlState=0;

}
void MainCtrol(void)
{
    gl_fYaw= gl_dblDiffHeading;
    gl_fSpeed = Get_Speed();	//���ſ��ƣ����ٿ��ƣ�	
    TargetShipCtrl();	
    //���PWM	
}
