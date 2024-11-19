#include "apt_time.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

char* get_current_time() {
    char *datetime = malloc(32);
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = gmtime(&rawtime);
    if (strftime(datetime, 32, "%a, %d %b %Y %T", timeinfo) == 0) 
    {
        fprintf(stderr, "Failed to format datetime\n");
        free(datetime);
        return NULL;
    }

    return datetime;
}
