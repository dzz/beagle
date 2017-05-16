#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif


#ifdef __linux__
#define _POSIX_TIMERS
#define _REENTRANT
#define _POSIX_C_SOURCE 200809L
#include <time.h>
#endif

#include "../system/log.h"
#include "hf_timer.h"
#include "../system/rt_module_codes.h"

#ifdef __linux__
typedef unsigned long LARGE_INTEGER;
#endif

LARGE_INTEGER Frequency;
double pcf;


#ifdef _WIN32
unsigned int initTimer()
{
    QueryPerformanceFrequency(&Frequency);
    pcf = (double)Frequency.QuadPart;
    log_message(CTT2_RT_MODULE_TIMER, LOG_LEVEL_INFO, "freq:\t%x", Frequency.QuadPart);
    log_message(CTT2_RT_MODULE_TIMER, LOG_LEVEL_INFO, "time:\t%x", timer_get_ms());
    return MODULE_LOADED;
}

double timer_get_ms()
{
    LARGE_INTEGER CurrentTime;
    QueryPerformanceCounter(&CurrentTime);

    CurrentTime.QuadPart *= 1000;

    return (double)CurrentTime.QuadPart / pcf;
}

void dropTimer() {} 
#endif

#ifdef __linux

unsigned int initTimer()
{
    return MODULE_LOADED;
}

double timer_get_ms()
{
    struct timespec sample;
    clock_gettime(CLOCK_MONOTONIC_RAW, &sample);
    {
        double ms_seconds = (double)(sample.tv_sec) * 1000;
        double ms_nsecs = (double)(sample.tv_nsec)/1000000;
        double ms = ms_seconds + ms_nsecs;

        return ms;
    }
}

void dropTimer() {} 

#endif
