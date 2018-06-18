/**
 *  \file se3c0.h
 *  \author Nicola Ferri
 *  \brief L0 structures and functions
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sha256.h"
#include "aes256.h"

#include "se3c0def.h"
#include "se3_common.h"



uint64_t se3c0_time_get();
void se3c0_time_set(uint64_t t);
void se3c0_time_inc();
void se3_time_init();
bool get_now_initialized();
