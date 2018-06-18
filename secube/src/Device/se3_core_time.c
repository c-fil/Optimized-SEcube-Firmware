/**
 *  \file se3c0.c
 *  \author Nicola Ferri
 *  \brief L0 structures and functions
 */

#include "se3_core_time.h"

uint64_t now;  ///< current UNIX time in seconds
bool now_initialized;  ///< time was initialized


void se3_time_init(){
	now_initialized = false;
	now = 0;
}



uint64_t se3c0_time_get()
{
#ifdef CUBESIM
    now = (uint64_t)time(0);
#endif
    return now;
}

void se3c0_time_set(uint64_t t)
{
    now = t;
	now_initialized = true;
}

void se3c0_time_inc()
{
    static unsigned int ms = 0;
    if (++ms == 1000) {
        (now)++;
        ms = 0;
    }
}

bool get_now_initialized(){
	return now_initialized;
}

