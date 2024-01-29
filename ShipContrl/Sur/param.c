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


int32_t parser_check_line(const char * line)
{
    if(line[0] == '$')
        return parser_check_nmea0183(line);
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


// Åµ¶ûÌ©Ö÷°å_om-20000129.pdf
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

