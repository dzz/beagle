#include "log.h"
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include "rt_module_codes.h"

#define LOG_FORMATTING_BUFFER_SIZE 1024
static FILE* logfile;
static char* formatting_buffer;
static unsigned int initialized = 0;

static unsigned int logging_client;
static unsigned int log_level = LOG_LEVEL_ERROR | LOG_LEVEL_WARNING | LOG_LEVEL_INFO | LOG_LEVEL_DEBUG | LOG_LEVEL_AUDMSG | LOG_LEVEL_GFXMSG;

void log_set_level(unsigned int level) {
    log_level = level;
}

unsigned int initLog() {
    logfile = fopen("log.txt","wb");
    if(!logfile)
        return MODULE_FAILURE;
    formatting_buffer = (char*)malloc(sizeof(char)*LOG_FORMATTING_BUFFER_SIZE);
    if(!formatting_buffer)
        return MODULE_FAILURE;

    initialized = 1;
    logging_client = 0;
    return MODULE_LOADED;
}

FILE *getLogfile(){
    return logfile;
}

void dropLog() {
    initialized = 0;
    free(formatting_buffer);
    fclose(logfile);
}



char* tag_from_level_code(unsigned int level) {
    switch(level) {
        case LOG_LEVEL_INFO:
            return "info";
        case LOG_LEVEL_WARNING:
            return "warning";
        case LOG_LEVEL_ERROR:
            return "error";
        case LOG_LEVEL_DEBUG:
            return "debug";
        case LOG_LEVEL_GFXMSG:
            return "gfxmsg";
        case LOG_LEVEL_AUDMSG:
            return "audmsg";
    }
    return "unknown";
}

void log_client_message( unsigned int level, const char* message ) {
    if( (level & LOG_LEVEL) == 0 ) {
        return;
    }
    if(!initialized) {
        return;
    } else {
        unsigned int system = CTT2_CLIENT_APPLICATION;
        char* system_prefix = ctt2_module_from_code(system);
        char* level_tag = tag_from_level_code(level);
        const char* format = "py:%s\n";

        if(LOG_TARGET & LOG_TARGET_FILE) {
            fprintf(logfile, format, message );
        }
        if(LOG_TARGET & LOG_TARGET_STDOUT) {
            printf(format, message );
        }

    }
}

void log_message(unsigned int system, unsigned int level, const char* message, ...) {

    const char* format = "%-8s[%04x]\t%-8s\t%s\n";

    if(!initialized) {
        va_list args;
        va_start(args, message);
        printf("(detached):");
        vprintf(message, args);
        printf("\n");
        va_end(args);
        return;
    }
    if( (level & LOG_LEVEL) == 0 ) {
        return;
    }
    else {
        char* system_prefix = ctt2_module_from_code(system);
        char* level_tag = tag_from_level_code(level);
        va_list args;
        va_start(args, message);
        vsnprintf(formatting_buffer, LOG_FORMATTING_BUFFER_SIZE, message, args);
        va_end(args);

        if(LOG_TARGET & LOG_TARGET_FILE) {
            fprintf(logfile, format, system_prefix, system, level_tag, formatting_buffer);
        }
        if(LOG_TARGET & LOG_TARGET_STDOUT) {
            printf(format, system_prefix, system, level_tag, formatting_buffer);
        }
    }
}
