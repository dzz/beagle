#include <malloc.h>
#include <stdio.h>
#include "files.h"
#include "log.h"

char* read_file(char *file)
{
    FILE *fptr;
    long length;
    char *buf;

    fptr = fopen(file, "rb"); 
    if (!fptr)  {
        char *r = malloc(sizeof(char));
        r[0] = '\0';
        log_message( CTT2_RT_MODULE_CORE, LOG_LEVEL_ERROR, "could not read :%s",file);
        return r;
    }
    fseek(fptr, 0, SEEK_END); 
    length = ftell(fptr); 
    buf = (char*)malloc(length+1); 
    fseek(fptr, 0, SEEK_SET); 
    fread(buf, length, 1, fptr); 
    fclose(fptr); 
    buf[length] = 0; 
    log_message( CTT2_RT_MODULE_CORE, LOG_LEVEL_DEBUG, "read %x bytes from %s",length, file);
    return buf; 
}
