
#ifndef __FILE_STRING_SPLIT_H__
#define __FILE_STRING_SPLIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    char * buffer;               
    int32_t buffer_length;      
    char ** section;             
    int32_t section_length;     
    int32_t section_length_valid;
    const char *seperators;      
} string_split_t;

void string_split_init(string_split_t *ss,char * buffer,int32_t buffer_length,char ** section,int32_t section_length,const char *seperators);
void string_split_input(string_split_t *ss,const char *line);


#define STRING_SPLIT_DEFINE(deco,name,buffer_size,section_size) \
    deco string_split_t name; \
    deco char   name##_buffer__[buffer_size]; \
    deco char * name##_section__[section_size]

#define STRING_SPLIT_INIT(name,sp) \
    string_split_init(&name, \
        name##_buffer__, \
        sizeof(name##_buffer__), \
        name##_section__, \
        sizeof(name##_section__), \
        sp)


#ifdef __cplusplus
}
#endif

#endif // __FILE_STRING_SPLIT_H__
