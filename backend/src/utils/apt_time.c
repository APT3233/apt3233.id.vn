#include "apt_time.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

char* get_current_time() {
    char *datetime = malloc(32);
    if (datetime == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime); 

    if (strftime(datetime, 32, "%m/%d/%Y %I:%M:%S %p", timeinfo) == 0) {
        fprintf(stderr, "Failed to format datetime\n");
        free(datetime);
        return NULL;
    }

    return datetime;
}
