// gnss/gnss.h
// 2013-11-29 8:29:07
// yys@system-intergration-department

#ifndef __FILE_GNSS_GNSS_H__
#define __FILE_GNSS_GNSS_H__

#include <stdint.h>
#include "gpgga.h"
#include "bestxyza.h"
#include "headinga.h"
#include "gprmc.h"
#include "gphdt.h"
#include "ppnav.h"
#include "refstation.h"
#include "gptra.h"
#include "ppnfo.h"
#include "structure.h"
#include "oem_o.h"
#include "datetime.h"

#define MODE_UNDEFINE             0x0000
#define MODE_DIRECT_MASTER        0x0001
#define MODE_DIRECT_ROVER         0x0002
#define MODE_DIRECT_INS           0x0003
#define MODE_WORK_DIVIDED         0x0004
#define MODE_WORK_COUPLING        0x0006
#define MODE_WORK_COUPLING_INS    0x0007
#define MODE_WORK_SINGLE          0x0008
#define MODE_WORK_SINGLE_INS      0x0009
#define	MODE_WORK_SINGLE_BASE     0x0010
#define	MODE_WORK_TEST            0x0011

#define	MODE_WORK_SINGLE_2ANTENNA 0x0108
#define	MODE_WORK_SINGLE_2ANTENNA_INS 0x0109

typedef enum
{
    GNSS_OUTPUT_LINE_NONE,
    GNSS_OUTPUT_LINE_ALL,
    GNSS_OUTPUT_LINE_IGNORE,
}gnss_output_line_t;

typedef struct
{
    uint32_t working_mode;
    uint32_t oem_type;
    uint32_t baud;//差分波特�?    
    uint32_t freq;//数据输出频率
    //
    gpgga_t         master_gpgga;
    uint32_t        master_gpgga_update_time;
    bestxyza_t      master_bestxyza;
    uint32_t        master_bestxyza_update_time;
    gprmc_t         master_gprmc;
    uint32_t        master_gprmc_update_time;
    refstation_t    master_refstation;
    uint32_t        master_refstation_update_time;
    headinga_t      master_headinga;
    uint32_t        master_headinga_update_time;
    gptra_t         master_gptra;
    uint32_t        master_gptra_update_time;
    //
    gpgga_t         rover_gpgga;
    uint32_t        rover_gpgga_update_time;
    headinga_t      rover_headinga;
    uint32_t        rover_headinga_update_time;
    //
    ppnav_t         ins_ppnav;
    uint32_t        ins_ppnav_update_time;
    
    ppnfo_t         ins_ppnfo;
    uint32_t        ins_ppnfo_update_time;
    
    //
    double base_x;
    double base_y;
    double base_z;
    double base_longitude;
} gnss_t;
typedef struct
{    
   int node_num;
   double utc_s;
   double roll;
   double pitch;
   double heading;   
   double acc_x;
   double acc_y;
   double acc_z;
} node_def;
#define CAR_STATUS_BITS 30
typedef struct
{ 
  uint8_t     m_Gear;
  float       m_Motor;
  float       m_Speed;
  uint8_t     m_Car_status[CAR_STATUS_BITS];  
  uint8_t     m_Recv;
  uint8_t     m_Flag;  
  char        checksum;
  double      m_Time;
  uint32_t    m_Status;
} CAR_INFO;
void    gnss_init(void);
int32_t gnss_reset(void);
const   gnss_t *gnss_get(void);
void    gnss_check_event(void);
int32_t gnss_set_mode(uint32_t mode);
void    gnss_restore_mode(void);
int32_t gnss_set_base(double base_latitude,double base_longitude,double base_altitude);
int32_t gnss_get_current_datetime(datetime_t *dt);
void    gnss_output_line(gnss_output_line_t mode);
const char * gnss_get_working_mode_string(void);
void    gnss_check_message_age(gnss_t * gnss);

#endif // __FILE_GNSS_GNSS_H__
