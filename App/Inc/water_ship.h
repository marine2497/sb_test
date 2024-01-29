
#ifndef __FILE_WATER_SHIP_H__
#define __FILE_WATER_SHIP_H__

#include <stdint.h>
#include "gnss.h"
#include "ship.h"
void ship_motor_control(int Motor_v_r,int Motor_v_l);
void self_tracking(void);
int32_t ship_output_vscbs(char status);
int32_t ship_output_vsapid(void);//to A5 

void vessel_mode_init(void);
void vessel_go_code(void);;
void self_ctrl_gps(unsigned char *pucDataBuf,unsigned int unDataLen);

int32_t smc_is(const char *line);
int32_t bsvsc_is(const char *line);
int32_t bsvsc_parser(const char *line);
int32_t smc_parser(SHIP_NODE *ship_node,const char *line);


typedef struct {
    uint32_t utc_ms;//毫秒
    float heading;
    float pitch;
    float roll;   
    uint8_t valid;//
    uint8_t num; 
    uint8_t diff_age;//
    uint8_t ID;
    char checksum;
}psic_tra_t;
int32_t psic_tra_is(const char *line);
int32_t psic_tra_parser(psic_tra_t *psic_tra,const char *line);




typedef struct {
    double p;
    double i;
    double d;
}PID_t;        
typedef struct {
    PID_t headingPID;
    PID_t trackPID;
    PID_t speedPID;
    PID_t posPID;
    char checksum;
}vspid_t;
int32_t vspid_is(const char *line);
int32_t vspid_parser(vspid_t *vspid,const char *line);
#endif // __FILE_GNSS_MODE_DIVIDED_H__
