// com/structure.h


#ifndef __FILE_COM_STRUCTURE_H__
#define __FILE_COM_STRUCTURE_H__

#include "stm32f10x.h"
// ���½ṹ�嶼��1�ֽڶ��롣
#pragma pack(push)
#pragma pack(1)

// ��������д���BootInfo��
typedef struct
{
    char boot_flag[32];//boot��ǣ��̶�ΪSouthGnss��ͨ�����ַ����ж�boot�ļ��Ƿ���ȷ
    char boot_ver[32];//boot�汾��Ϣ������boot�ļ�ʱ���� 		
    char sn[32];//����ţ���PC��������д��       		             
    char pn[32];//Ӳ���汾�ţ���PC��������д�� 		
    char hid[32];//Ӳ����ʶ����PC��������д��  		
    char brand[32];//Ʒ�ƣ���PC��������д��  	
    char model[32];//�ͺţ���PC��������д�� 	
    char product_date[32];//�������ڣ���PC��������д�� 		  		
    char build_date[32];//�������ʱ��
    char board1sn[32];
    char board2sn[32];
    char des[16];
} boot_info_t;

// ���ذ汾��Ϣ�����Body��
typedef struct
{
    char boot_ver[32];      //boot�汾��Ϣ������boot�ļ�ʱ����
    char app_ver[32];       //����汾����SOFT_VERSION����ơ�
    char sn[32];            //����ţ���PC��������д��
    char pn[32];            //Ӳ���汾�ţ���PC��������д��
    char hid[32];           //Ӳ����ʶ����PC��������д��
    char brand[32];         //Ʒ�ƣ���PC��������д��
    char model[32];         //�ͺţ���PC��������д��
    char product_date[32];  //�������ڣ���PC��������д��
    char board1sn[32];
    char board2sn[32];
} body_version_t;

// ��ѯע��״̬�����Body��
typedef struct
{
    char     sn[32];
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint8_t  regist_code[20];
} body_register_query_t;

// ���û�վ�����Body��
typedef struct
{
    double latitude;
    double longitude;
    double altitude;
} body_set_base_t;

// ��������Body��
typedef struct
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
    //
    //rover ǰ���߾�γ��
    double rover_latitude;
    double rover_longitude;
    double rover_altitude;
    //
    //master �����߾�γ��
    double master_latitude;
    double master_longitude;
    double master_altitude; 
    //
    float speed;//�ٶ� speed;
    float pitch;//���� pitch : x�ᣬ[-90,90)
    float roll;//���  roll  : y�ᣬ[-180,180)
    uint8_t rover_nosv;//ǰ���߿���������
    uint8_t master_nosv;//�����߿���������
    //
    uint8_t rover_valid;//ǰ���߽�״̬
    uint8_t master_valid;//�����߽�״̬
} body_output_0x0100_t;


// ��������Body,add by yys 2014.6.24
typedef struct
{

    uint8_t rover_nosv;//ǰ���߿���������
    uint8_t master_nosv;//�����߿���������
    //
    uint8_t rover_valid;//ǰ���߽�״̬
    uint8_t master_valid;//�����߽�״̬
    
    uint8_t rover_age;//ǰ���߲����ʱ
    uint8_t master_age;//�����߲����ʱ
    
    float rover_hdop;//ǰ����hdop
    float master_hdop;//������hdop
    
} body_output_0x0d03_t;

// ��������Body��
typedef struct
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;             // UTC Сʱ
    uint8_t  minute;           // UTC ����
    uint8_t  second;           // UTC ��
    uint16_t millisecond;      // UTC ����
    double   master_latitude;  // ������γ��
    double   master_longitude; // �����߾���
    double   master_altitude;  // �����߸߳�
    float    base_line_length; // ���߳����ס�
    float    speed;            // �ٶ�
    float    heading;          // �����
    float    heading_rmc;      // ��ʻ��
    float    pitch_gnss;       // ������ĸ�����
    float    pitch_ins;        // ��������ĸ�����
    float    roll_ins;         // ��������ĺ����
    uint8_t  master_nosv;      // �����߿���������
    uint8_t  master_hdop_10;   // ������HDOP*10
    uint8_t  master_diff_age;  // �����߲���ӳ�
    uint8_t  master_gga_valid; // �����߶�λ״̬
    uint8_t  master_rmc_valid; // �������ٶ�״̬
    uint8_t  heading_valid;    // ����״̬
    uint8_t  rover_nosv;       // ǰ���߿���������
    uint8_t  rover_valid;      // ǰ���߶�λ״̬
} body_output_0x0200_t;

// ��������Body��
typedef struct
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
    uint16_t ms;
    double   latitude;  // ������γ��
    double   longitude; // �����߾���
    double   altitude;  // �����߸߳�
    float    speed;            // �ٶ�
    float    heading;          // �����
    float    pitch;            // ������
    float    roll;             // �����
    uint8_t  valid;     // �����߶�λ״̬
} body_output_0x0400_t;

// ������������0x0402��0x0403�����Body��
typedef struct
{
    uint32_t id;
    float gyr_offset[3];
    float gyr_gain[3];
    float acc_offset[3];
    float acc_gain[3];
} body_sensor_param_t;

// ������ԭʼ���ݡ�
typedef struct
{
    uint32_t uid;
    int32_t  omg_raw[3];
    int32_t  spf_raw[3];
} body_sensor_raw_t;

// ��Ӧͬ��ʱ�䡣
typedef struct
{
    uint16_t ms;
    uint8_t  second;
    uint8_t  minute;
    uint8_t  hour;
    uint8_t  day;
    uint8_t  month;
    uint16_t year;
} body_sync_time_t;

typedef struct
{
    double latitude;
    double longitude;
    double altitude;
    uint8_t valid;
} body_refstation_0x0d04_t;

// ���Ͻṹ����Ҫ������
#pragma pack(pop)

// ����Ľṹ�岻��Ҫ������

#endif // __FILE_COM_STRUCTURE_H__
