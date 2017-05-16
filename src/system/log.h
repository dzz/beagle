#ifndef _LOGFILE__
#define __LOGFILE__

#define LOG_TARGET_STDOUT (1)
#define LOG_TARGET_FILE (2)

#define LOG_TARGET (LOG_TARGET_STDOUT | LOG_TARGET_FILE )

#define LOG_LEVEL_ERROR (1)
#define LOG_LEVEL_WARNING (2)
#define LOG_LEVEL_INFO (4)
#define LOG_LEVEL_DEBUG (8)
#define LOG_LEVEL_GFXMSG (16)
#define LOG_LEVEL_AUDMSG (32)

#define LOG_LEVEL log_level

#include <stdio.h>
#include <stdarg.h>

#include "rt_module_codes.h"

unsigned int initLog();
void dropLog();

void log_message( unsigned int system, unsigned int level, const char* mesage, ... ); 
void log_client_message( unsigned int level, const char* mesage ); 
void log_set_level(unsigned int level);

FILE* getLogfile();

#endif
