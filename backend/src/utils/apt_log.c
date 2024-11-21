#include <stdio.h>
#include "apt_log.h"
#include "apt_time.h"

void log_info(int level, const char* title, const char* msg)
{
    if(level < 0) 
        printf("[\033[1;31mERROR\033[0m][\033[0;35m%s\033[0m] %s %s\n", get_current_time(), title, msg);
    else if(level == 0) 
        printf("[\033[1;33mWARNING\033[0m][\033[0;35m%s\033[0m] %s %s\n", get_current_time(), title, msg);
    else 
        printf("[\033[1;36mINFO\033[0m][\033[0;35m%s\033[0m] %s %s\n", get_current_time(), title, msg);
    
}