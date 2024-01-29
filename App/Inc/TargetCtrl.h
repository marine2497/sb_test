#ifndef __FILE_TARGETCTRL_H__
#define __FILE_TARGETCTRL_H__

#define PWM_MIN					0//344			//对应 1ms - 1.90ms
#define PWM_MAX					50000//4688
#define PWM_BASE				0
#define PWM_OUT				0//1输出PWM  NUAA程序 0不输出PWM 对接程序
extern float gl_fPosX      	, gl_fPosY		, gl_fPosZ		;
extern unsigned char  gl_unAutoFlag     ;
extern unsigned char  gl_unPointAll     ;
//extern unsigned char  gl_unFunctionCode ;
extern unsigned char	gl_unFlyMode_SET  ;		
extern unsigned char	gl_unCTRLPARA_SET ;
extern unsigned char  gl_unTurningFlag   ;
extern unsigned char	ucAutoHov;
extern unsigned char  gl_unCtrlState     ; //控制状态
extern unsigned char  gl_unFunctionCode ;	//功能码
extern double gl_dblLon0		, gl_dblLat0		;//初始经纬度
extern double gl_dblLonD[100], gl_dblLatD[100];//目标经纬度
extern double gl_dblYawDis_Kp , gl_dblYawDis_Ki , gl_dblYawDis_Kd;
extern double gl_dblRatio ;
extern double gl_dblLockYaw , gl_dblLockYaw_Kp , gl_dblLockYaw_Ki , gl_dblLockYaw_Kd ;//航向锁定PID
extern double gl_dblTurning_Kp, gl_dblTurning_Ki, gl_dblTurning_Kd; //转弯PID
extern double gl_dblLockYaw_a , gl_dblLockYaw_T ;//航向锁定超前矫正
extern float  gl_fPitch 		, gl_fRoll ,		 gl_fYaw 		, gl_fYaw_1   ,gl_fYawLast, gl_fYawRate;//
extern double gl_dblSpeed_Kp, gl_dblSpeed_Ki , gl_dblSpeed_Kd ,
			        gl_dblPos_Kp , gl_dblPos_Ki , gl_dblPos_Kd ;
extern double gl_MFCLeftMotorControl ,gl_MFCRightMotorControl ;//上位机直接控制
extern double gl_LeftMotorControl,gl_RightMotorControl;
extern double gl_dblSpeedDownLine;
extern double gl_dblSetSpeed;
extern double gl_dblTurnRadius;//直角转弯半径
extern double gl_dblSetTurnTime;//给定转弯时间
typedef struct PIDControl
{
double Kp;
double Ki;
double Kd;
double dE;
double dE_1;
double dE_2;
double SumdE;
double M;
double M_1;
double Out;
}MyPIDControl;
extern MyPIDControl myHeadingPIDControl;
extern MyPIDControl myTrackPIDControl;
extern MyPIDControl mySpeedPIDControl;
extern MyPIDControl myPosPIDControl;
double Get_YawRate(double dblYaw,double dblYawlast,double time,double timelast);
double Kalman(const double ResrcData,double ProcessNiose_Q,double MeasureNoise_R,double InitialPrediction);
void CalcDistYawError2NextWP(double dblX, double dblY, double dblX0, double dblY0, double dblX1, double dblY1, double* pdblDistYawError, double* pdblDist2NextWP);
void CalcGrd(double dblLat0, double dblLon0, double dblLat, double dblLon, double *pdblPosX, double *pdblPosY);
void InvCalcGrd(double dblLat0, double dblLon0, double *dblLat, double *dblLon, double r, double heading);
void Motor_Control(double left,double right);
void TrackCtrl(void);
void TargetShipCtrl(void);
double XyToHeading(double x,double y);
double YawPID_Control(double target_heading,double heading);
double TrackPID_Control(double target_track,double track);
double SpeedPID_Control(double target_heading,double heading);
double PosPID_Control(double target_track,double track);
double Get_TargetSpeed(double target_pt,double now_pt,double all_pt);
void SetHeadingPID(double Kp,double Ki,double Kd);
void SetTrackPID(double Kp,double Ki,double Kd);
void SetSpeedPID(double Kp,double Ki,double Kd);
void SetPosPID(double Kp,double Ki,double Kd);
float PhaseLeadCtl_fCalc(float fCtlIn);
double Get_Speed(void);
double Speed_Control(double target_speed,double now_speed);
double Oil_Control(double target_pt,double now_ptf);
float KalmanFilter_Speed(const float ResrcData,float ProcessNiose_Q,float MeasureNoise_R);
float KalmanFilter_PosX(const float ResrcData,float ProcessNiose_Q,float MeasureNoise_R);
float KalmanFilter_PosY(const float ResrcData,float ProcessNiose_Q,float MeasureNoise_R);
void PID_Init(void);

void MainCtrol(void);
void Parameter_Init(void);

#endif
