#include "prompt.h"
#include <stdio.h>
#include <string.h>
#include "com.h"
#include "Type.h"
#include "string-split.h"

static string_split_t g_ss; // 用于把一行命令分割成一个个参数。
static char   g_ss_buffer[256]; //g_ss的缓冲区。
static char * g_ss_section[16]; //g_ss的缓冲区。
typedef void (*fn_on_prompt_t)(int32_t argc, char **argv);

static void prompt_on_sys      (int32_t argc, char **argv);
static void prompt_on_memory   (int32_t argc, char **argv);
static void prompt_on_param    (int32_t argc, char **argv);
static void prompt_on_output   (int32_t argc, char **argv);
static void prompt_on_help     (int32_t argc, char **argv);
extern uint32_t MACHINE_ID;

//#define SENSOR_DATA_SET                 "$instar,dateset"
//#define SENSOR_LLA_SET                  "$instar,llaset"//"$instar,llaset,纬度,经度,高度";//设置经纬度高程
//#define SENSOR_POLE_SET                 "$instar,htset,2.0"//设置杆高
//#define SENSOR_SAVE_CMD                 "$instar,save,nav"//保存数据
//#define SENSOR_MAG_CMD                  "$instar,mcali"//磁场校准
//#define SENSOR_ACC_CMD                  "$instar,acali"//加速度校准

//#define SENSOR_OUTPUT_SAVE_DATA_CMD     "$instar,output_save_data"//输出保存数据
//#define SENSOR_MAG_DEC                  "$instar,magnatic_declination," //magnatic declination
//#define SENSOR_CHIP_TEMP                "$instar,read_temp"
//#define SENSOR_CHIP_MODEL               "$instar,read_model"
//#define SENSOR_SOFTRESET_BOOT           "$instar,softresetboot"
//#define SENSOR_SOFTRESET_APP            "$instar,softresetapp"

// 命令列表，新增命令时，只需要在这里加上即可。
static struct
{
    const char *    category;
    fn_on_prompt_t  callback;
} g_prompt_table[] = 
{
  { "$instar"   , prompt_on_sys      },
  { "#memory"   , prompt_on_memory   },
  { "#param"    , prompt_on_param    },
  { "#output"   , prompt_on_output   },
  { "#help" ,     prompt_on_help     },
};

static void f_Date_Get_Macro(char *time,int *month,int *day,int *year)
{
  char s_month[5];
  int d,y;
  static const char month_names[]="JanFebMarAprMayJunJulAugSepOctNovDec";
  sscanf(time,"%s %d %d",s_month,&d,&y);
  *month=(strstr(month_names,s_month)-month_names)/3+1;
  *day =d;
  *year =y;
}
void f_Str_Model_Output(void)
{  
//  int day=0,year=0,month=0;
//  f_Date_Get_Macro(__DATE__,&month,&day,&year);
//  if( MACHINE_ID == 1)
//    printf("instar,model,VS01,%d%02d%02d,ok",year,month,day);
//  else
//    if( MACHINE_ID == 2)
//      printf("instar,model,VS02,%d%02d%02d,ok",year,month,day); 
//  printf("\r\n");
}
void prompt_init(void)
{
      string_split_init(&g_ss,g_ss_buffer,sizeof(g_ss_buffer),g_ss_section,sizeof(g_ss_section)/sizeof(g_ss_section[0]),", \t\r\n");
}
// 处理一行命令。
void prompt_handle_line(const char * line)
{
	int32_t table_length;
	int32_t i;
    string_split_input(&g_ss,line);
    //for(int32_t i=0;i<g_ss.section_length_valid;i++)
    //    printf("pt[%d] = \"%s\"\r\n",i,g_ss.section[i]);
    //
    if(g_ss.section_length_valid < 1)
        return;
    //
    // 在命令列表中查找，找到就执行对应的回调函数。
     table_length = sizeof(g_prompt_table) / sizeof(g_prompt_table[0]);
    for( i=0;i<table_length;i++)
    {
        if(strcmp(g_ss.section[0],g_prompt_table[i].category) == 0)
        {
            g_prompt_table[i].callback(g_ss.section_length_valid,g_ss.section);
            break;
        }
    }
}

// #mcu reset => 复位芯片。
void prompt_on_sys(int32_t argc, char **argv)
{
    if(argc == 2 && strcmp(argv[1],"softresetboot") == 0)
    {
//        NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
//        NVIC_SystemReset();
    }
    if(argc == 2 && strcmp(argv[1],"softresetapp") == 0)
    {
//        NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xd000);
//        NVIC_SystemReset();
    }
    //
    if(argc == 2 && strcmp(argv[1],"voltage") == 0)
    {
     //   printf("power voltage is %fV.\r\n", power_voltage());
//        return;
    }
    if(argc == 2 && strcmp(argv[1],"read_model") == 0)
    {
      f_Str_Model_Output();
//      return;
    }
    if(argc == 3 && strcmp(argv[1],"directlink") == 0)
    {
      
     //   printf("power voltage is %fV.\r\n", power_voltage());
//        return;
    }
}

// #memory dump 0x0E0000 64 => 打印一段内存的值。
void prompt_on_memory(int32_t argc, char **argv)
{
    if(argc < 4)
        return;
    //
    if(strcmp(argv[1],"dump") == 0)
    {
        int32_t address = 0;
        int32_t size = 0;
        sscanf(argv[2],"0x%x",&address);
        sscanf(argv[3],"%x",&size);
        //
        printf("[0x%X] =\r\n",address);
        for(int32_t i=0;i<size;i++)
        {
            if(i % 4 == 3)
                printf("%02X ",((uint8_t *)address)[i]);
            else
                printf("%02X",((uint8_t *)address)[i]);
        }
        printf("\r\n");
    }
}

// #param clear => 重置Flash里的参数。
// #param save  => 保存参数到Flash。
// #param print => 打印参数。
void prompt_on_param(int32_t argc, char **argv)
{ 

}

void prompt_on_output(int32_t argc, char **argv)
{
    if(argc >= 2 && strcmp(argv[1],"silent") == 0)
    {
        if(argc == 2 || (argc == 3 && strcmp(argv[2],"true")==0))
        {
            //output_silent(1);
            printf("^ok output silent.\r\n");
            return;
        }
        if((argc == 3 && strcmp(argv[2],"false")==0))
        {
           // output_silent(0);
            printf("^ok output normal.\r\n");
            return;
        }
    }
    if(argc == 3 && strcmp(argv[1],"diffbaud") == 0)//设置差分波特率
    {
    	const char *bdr[] = {"","4800","9600","19200","38400","57600","115200"};
        uint32_t i = 0;
    	for(i = 0;i < sizeof(bdr)/sizeof(bdr[0]);i++)
    	{
    		if(strcmp(argv[2],bdr[i]) == 0) break;
    	}
    	if( i == sizeof(bdr)/sizeof(bdr[0]) || i == 0)
    		return;
        //
		//param_get()->gnss.baud = i;
		//if(param_auto_save() != 0)
		{
			printf("write flash failed!\r\n");
			return;
		}
		printf("%s %s %s\r\n",argv[0],argv[1],argv[2]);
        return;
    }
    if(argc == 3 && strcmp(argv[1],"outfreq") == 0)//设置报文输出频率
    {
    	const char *freq[] ={"","10","5","1","0.2","0.1"};
        uint32_t j = 0;
    	for(j = 0;j < sizeof(freq)/sizeof(freq[0]);j++)
    	{
    		if(strcmp(argv[2],freq[j]) == 0) break;
    	}
    	if( j == sizeof(freq)/sizeof(freq[0]) || j == 0)
    		return;
        //
		//param_get()->gnss.freq = j;
		//if(param_auto_save() != 0)
		{
			printf("write flash failed!\r\n");
			return;
		}
		printf("%s %s %s\r\n",argv[0],argv[1],argv[2]);
        return;
    }    
    //
    printf("^failed operation not support.\r\n");
}


//help user 输出常规帮助信息
//help root 输出root权限可看到帮助信息
void prompt_on_help(int32_t argc, char **argv)
{
    //help information add by yys
    static const char* root =
    "\r\n\r\n"
    "#help \r\n"
    "#sys,resetboot\r\n"
    "#sys,resetapp\r\n"
    "#sys,voltage\r\n"
    "#memory,dump,{add},{size}\r\n"
    "#output,diffbaud,{ 4800 | 9600 | 19200 | 38400 | 57600 | 115200 }\r\n"
    "#output,outfreq,{ 0.1 | 0.2 | 1 | 5 | 10}\r\n"
    "#output,silent,{[true] | false} }\r\n"
    "#param,{ print | save | clear | {ignore-auto-save,[true|false]}}\r\n"
    "\r\n\r\n";
   
    printf("\r\n\r\n%s",root);

}
