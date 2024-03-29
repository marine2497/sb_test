#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char * s_sections[64] = {0};
static char s_sections_buffer[1024] = {0};

char g_parser_print_buffer[PARSER_PRINT_BUFFER_SIZE];

static unsigned long CalculateBlockCRC32(unsigned long ulCount,const unsigned char *ucBuffer );

uint8_t parser_get_checksum0183(const char *str)
{
    uint8_t cs = 0;

    if(*str != '$')
        return 0;
    str++;

    for(;*str != '\0';str++)
    {
        cs ^= *str;
    }

    return cs;
}


int32_t parser_check_nmea0183(const char * line)
{
    if(*line != '$')
        return -1;
    //
    uint8_t xor_sum = 0;
    const char * c = line+1;
    for(; *c != '\0' && *c != '*' && (c-line)<512; c++)
        xor_sum ^= *c;
    //
    if(*c != '*')
        return -2;
    //
    uint32_t sum_l = 0;
    char sum_s[3] = { c[1], c[2], '\0' };
    sscanf(sum_s,"%02X",&sum_l);
    //
    if(xor_sum != (uint8_t)sum_l)
        return -3;
    //
    return 0;
}

uint32_t parser_get_crc32(const char *str)
{
    if(str[0] != '#')
        return 0;
    return CalculateBlockCRC32(strlen(str+1),(const uint8_t *)str+1);
}


/* CRC 高位字节值表 */
static  char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41,0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1,0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40,0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41,0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40} ;
/* CRC低位字节值表*/
static char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 
0xCC, 0x0C, 0x0D, 0xCD,0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,0x08, 0xC8, 
0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 
0x14, 0xD4,0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,0x11, 0xD1, 0xD0, 0x10, 
0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
0x28, 0xE8, 0xE9, 0x29,0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,0xEC, 0x2C, 
0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 
0xA0, 0x60,0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,0xA5, 0x65, 0x64, 0xA4, 
0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 
0xB4, 0x74, 0x75, 0xB5,0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,0x70, 0xB0, 
0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 
0x9C, 0x5C,0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,0x99, 0x59, 0x58, 0x98, 
0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,0x43, 0x83, 0x41, 0x81, 0x80, 0x40} ;
unsigned short parser_get_crc16(char *puchMsg, unsigned short usDataLen)
//unsigned char *puchMsg ; /* 要进行CRC校验的消息 */
//unsigned short usDataLen ; /* 消息中字节数 */
{
  char uchCRCHi = 0xFF ; /* 高CRC字节初始化 */
  char uchCRCLo = 0xFF ; /* 低CRC 字节初始化 */
  unsigned int uIndex ; /* CRC循环中的索引 */
  //printf("length is  %d\r\n",usDataLen);
  
  while (usDataLen--) /* 传输消息缓冲区 */
  {
    uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
    uchCRCLo = auchCRCLo[uIndex] ;
  }
    return (uchCRCHi << 8 | uchCRCLo) ;
}
/**********************************************/
//帧格式：
//设备地址（1字节）      功能码（1字节）   数据命令区长度（2字节）   数字命令区（n字节）   crc16校验码（2字节：从设备地址到数据命令区）
//   0x01                    0x03            0x00 0x02                     0x00 0x02         0x65  0xCB
//
/*********************************************/
int parser_modbus_rtu(char *puchMsg, unsigned short usDataLen)
{
  unsigned int len ; 
  unsigned short crc16_rec,crc16_cal;
  if(usDataLen<3)
    return -1;
  
  len = (puchMsg[2]);
  if(usDataLen<(len+5))
    return -1;
  crc16_rec = (puchMsg[len+3]<<8|puchMsg[len+4]);
  crc16_cal = parser_get_crc16(puchMsg,len+3);
  
 // printf("mod_len %d length: %d  crc16_rec %x crc16_cal %x\r\n",len,usDataLen,crc16_rec,crc16_cal);
  
  if(crc16_rec == crc16_cal)
    return 1;
  else
    return -1;

  
}


int32_t parser_check_crc32(const char * line)
{
    if(*line != '#')
        return -1;
    //
    const char * p = line;
    for(;*p!='\0'&&*p!='*';p++)
    {}
    if(*p != '*')
        return -2;
    //
    uint32_t crc_cal = CalculateBlockCRC32(p-line-1,(const uint8_t *)line+1);
    char crc_cal_str[9];
    snprintf(crc_cal_str,sizeof(crc_cal_str),"%08x",crc_cal);
    for(int i=0;i<8;i++)
    {
        if(p[1+i] != crc_cal_str[i])
        return -3;
    }
    //
    return 0;
}


int32_t parser_check_line(const char * line,int32_t length)
{
    if(line[0] == '$')
    {
      if(line[1] == 'i'&&line[2] == 'n'&&line[3] == 's'&&line[4] == 't'&&line[5] == 'a'&&line[6] == 'r')
        return 0;
      else
        return parser_check_nmea0183(line);
    }
    if(line[0] == '#')
        return 0;//parser_check_crc32(line);
    return -1;
}

int32_t parser_cut_section(const char * line, char ***sections_ptr)
{
    *sections_ptr = s_sections;
    //
    uint32_t section_amount = 0; 
    uint32_t buffer_index = 0;
    s_sections[0] = s_sections_buffer;
    for(const char * ch = line;*ch != '\0';ch ++)
    {
        if(*ch == ',' || *ch == ';' || *ch == '*')
        {

            s_sections_buffer[buffer_index++] = '\0';
            s_sections[++ section_amount] = s_sections_buffer+buffer_index;

            if(buffer_index >= sizeof(s_sections_buffer))
                return 0;
            if(section_amount >= sizeof(s_sections)/sizeof(s_sections[0]))
                return 0;
            //
            continue;
        }

        s_sections_buffer[buffer_index++] = *ch;
        if(buffer_index >= sizeof(s_sections_buffer))
            return 0;
    }
    s_sections_buffer[buffer_index++] = '\0';
    s_sections[++section_amount] = 0;
    //
    return section_amount;
}

uint32_t parser_str2utc(const char * utc_str)
{
    const char * t = utc_str;
    if(strlen(t) < 7 || t[6] != '.')
        return 0;
    //
    uint32_t h = (t[0]-'0') * 10 + (t[1]-'0');
    uint32_t m = (t[2]-'0') * 10 + (t[3]-'0');
    uint32_t s = (t[4]-'0') * 10 + (t[5]-'0');
    //
    t = t + 7;
    uint32_t ms = 0;
    if(t[0] != '\0')
    {
        ms += (t[0] - '0') * 100;
        if(t[1] != '\0')
        {
            ms += (t[1] - '0') * 10;
            if(t[2] != '\0')
                ms += t[2] - '0';
        }
    }
    //
    return ((h*60 + m)*60 + s)*1000 + ms;
}


// 璇哄皵娉颁富鏉縚om-20000129.pdf
#define CRC32_POLYNOMIAL   0xEDB88320L
/* --------------------------------------------------------------------------
Calculate a CRC value to be used by CRC calculation functions. 
-------------------------------------------------------------------------- */
unsigned long CRC32Value(int i)
{
    int j;
    unsigned long ulCRC;
    ulCRC = i;
    for ( j = 8 ; j > 0; j-- )
    {
        if ( ulCRC & 1 )
            ulCRC = ( ulCRC >> 1 ) ^ CRC32_POLYNOMIAL;
        else
            ulCRC >>= 1;
    }
    return ulCRC;
}
/* --------------------------------------------------------------------------
Calculates the CRC-32 of a block of data all at once
-------------------------------------------------------------------------- */
unsigned long CalculateBlockCRC32(unsigned long ulCount,const unsigned char *ucBuffer )
{
    unsigned long ulTemp1;
    unsigned long ulTemp2;
    unsigned long ulCRC = 0;
    while ( ulCount-- != 0 )
    {
        ulTemp1 = ( ulCRC >> 8 ) & 0x00FFFFFFL;
        ulTemp2 = CRC32Value( ((int) ulCRC ^ *ucBuffer++ ) & 0xff );
        ulCRC = ulTemp1 ^ ulTemp2;
    }
    return( ulCRC );
}

// #define __TEST_PARSER_C__
#ifdef __TEST_PARSER_C__

#include <stdio.h>

const char * g_lines[] =
{
    "$GPGGA,091827.00,2307.58813,N,11321.93129,E,2,08,1.0,32.6,M,-5.2,M,7.0,0129*6C\r\n", // ok
    "$GPGGA,091827.00,2307.58813,N,11321.93129,E,2,08,1.0,32.6,M,-5.2,M,7.0,0129*6D\r\n", // cs error
    "$GPGGA,,,N,,E,2,,,32.6,M,-5.2,M,7.0,0129*51\r\n", // ok
    "$,,e,u;444,,*33,\r\n",
    "#BESTXYZA,COM1,0,76.5,FINESTEERING,1810,293246.000,00040000,bde7,12104;SOL_COMPUTED,SINGLE,-2327476.1729,5387379.5189,2489613.9895,1.9562,4.0244,1.6251,SOL_COMPUTED,DOPPLER_VELOCITY,-0.0068,-0.0024,0.0015,0.2338,0.4810,0.1942,\"\",0.150,0.000,0.000,14,13,13,13,0,06,00,33*e5f321de\r\n"
};

int main(void)
{
    for(int i=0;i<sizeof(g_lines)/sizeof(g_lines[0]);i++)
    {
        int32_t rs = parser_check_line(g_lines[i]);
        if(rs != 0)
        {
            printf("[%d] not a correct line. %d\n",i,rs);
            continue;
        }
        //
        char ** sections = 0;
        int32_t sa = parser_cut_section(g_lines[i],&sections);
        printf("[%d] \"%s\"{\n",i,g_lines[i]);
        for(int32_t i=0;i<sa;i++)
            printf("  {%2d} =\"%s\"\n",i,sections[i]);
        printf("}\n\n");
    }
    return 0;
}

#endif // __TEST_PARSER_C__
