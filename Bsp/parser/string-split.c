#include "string-split.h"

static int32_t is_seperator(string_split_t *ss,char ch);


void string_split_init(string_split_t *ss,char * buffer,int32_t buffer_length,char ** section,int32_t section_length,const char *seperators)
{
    ss->buffer               = buffer;
    ss->buffer_length        = buffer_length;
    ss->section              = section;
    ss->section_length       = section_length;
    ss->section_length_valid = 0;
    ss->seperators           = seperators;
}

void string_split_input(string_split_t *ss,const char *str_input)
{
    int32_t buffer_index  = 0;
    int32_t section_index = 0;
    //
    int32_t last_char_is_seperator = 1; 
    for(const char *ch=str_input;*ch!='\0';ch++)
    {
        int32_t this_char_is_seperator = is_seperator(ss,*ch);
        //
        if(! this_char_is_seperator)
        {
            if(last_char_is_seperator)
            {
                
                ss->section[section_index] = ss->buffer + buffer_index;
                ss->buffer[buffer_index ++] = *ch;
                if(buffer_index >= ss->buffer_length-1)
                {
                    last_char_is_seperator = 1; 
                    break;
                }
            }
            else
            {
                
                ss->buffer[buffer_index ++] = *ch;
                if(buffer_index >= ss->buffer_length-1)
                {
                    last_char_is_seperator = 1; 
                    break;
                }
            }
        }
        else
        {
            if(last_char_is_seperator)
            {
               
            }
            else
            {
                
                ss->buffer[buffer_index ++] = '\0';
                section_index ++;
                if(buffer_index >= ss->buffer_length-1 || section_index >= ss->section_length)
                {
                    last_char_is_seperator = 1;
                    break;
                }
            }
        }
        //
        last_char_is_seperator = this_char_is_seperator;
    }
    //
    if(!last_char_is_seperator)
    {
        
        ss->buffer[buffer_index ++] = '\0';
        section_index ++;
    }
    //
    ss->section_length_valid = section_index;
}

int32_t is_seperator(string_split_t *ss,char ch)
{
    for(const char *c=ss->seperators;*c!='\0';c++)
    {
        if(*c == ch)
            return 1;
    }
    return 0;
}

//#define __TEST_STRING_SPLIT__
#ifdef  __TEST_STRING_SPLIT__

#include <stdio.h>

string_split_t g_ss;
char g_ss_buffer[128];
char *g_ss_section[8];

int main(void)
{
    printf("\nsplit(\"abc1\n \t2de\nfgh 3456\ti;jk[l]8 mn \top \")\n");
    string_split_init(&g_ss,g_ss_buffer,sizeof(g_ss_buffer),g_ss_section,sizeof(g_ss_section)/sizeof(g_ss_section[0])," \t\n");
    string_split_input(&g_ss,"abc1\n \t2de\nfgh 3456\ti;jk[l]8 mn \top ");
    //
    for(int i=0;i<g_ss.section_length_valid;i++)
    {
        printf("[%d]=>%s<=\n",i,g_ss.section[i]);
    }
    //
    printf("\nsplit(\"ab,c,def,ghi,jkl,mno\") 128 4\n");
    string_split_init(&g_ss,g_ss_buffer,sizeof(g_ss_buffer),g_ss_section,4," ,");
    string_split_input(&g_ss,"ab,c,def,ghi,jkl,mno");
    //
    for(int i=0;i<g_ss.section_length_valid;i++)
    {
        printf("[%d]=>%s<=\n",i,g_ss.section[i]);
    }
    //
    printf("\nsplit(\"ab,cd,efghi,jkl,mno,pqrs,tu,vw\") 10 8\n");
    g_ss_buffer[128];
    string_split_init(&g_ss,g_ss_buffer,12,g_ss_section,8," ,");
    string_split_input(&g_ss,"ab,cd,efghi,jkl,mno,pqrs,tu,vw");
    //
    for(int i=0;i<g_ss.section_length_valid;i++)
    {
        printf("[%d]=>%s<=\n",i,g_ss.section[i]);
    }
    //
    return 0;
}

#endif // __TEST_STRING_SPLIT__

