/**
 *  \file se3_common.h
 *  \brief This file contains defines and functions common for L0 and L1
 * 
 */

#pragma once

//#include "se3c0def.h"
#include "se3c1def.h"

extern const uint8_t se3_magic[SE3_MAGIC_SIZE];

//########################DEBUG##############################
//#define SE3_DEBUG_SD 0
//#define SE3_DEBUG_SD2 0

#ifndef se3_serial_def
#define se3_serial_def
typedef struct SE3_SERIAL_ {
    uint8_t data[SE3_SERIAL_SIZE];
    bool written;  ///< Indicates whether the serial number has been set (by FACTORY_INIT)
} SE3_SERIAL;
#endif

/** \brief decoded request header */
typedef struct se3c0_req_header_ {
    uint16_t cmd;
    uint16_t cmd_flags;
    uint16_t len;
#if SE3_CONF_CRC
    uint16_t crc;
#endif
    uint32_t cmdtok[SE3_COMM_N - 1];
} se3c0_req_header;


SE3_SERIAL serial;

uint16_t hwerror;


#ifdef SE3_DEBUG_SD2
#define BASE_DEBUG_ADDRESS 16704 //41024 //16656

bool sd_flush();
int debug_count;

int buff_len(uint8_t* buff);



bool MYPRINTF( uint8_t* buf, uint32_t blk_addr);

#endif
//##############################################################


/**
 *  \brief Compute length of data in a request in terms of SE3_COMM_BLOCK blocks
 *  
 *  \param [in] len_data_and_headers Data length
 *  \return Number of SE3_COMM_BLOCK blocks
 *  
 */
uint16_t se3_req_len_data(uint16_t len_data_and_headers);

/**
 *  \brief Compute length of data in a request accounting for headers
 *  
 *  \param [in] len_data Data length
 *  \return Number of Bytes
 *  
 */
uint16_t se3_req_len_data_and_headers(uint16_t len_data);

/**
 *  \brief Compute length of data in a request in terms of SE3_COMM_BLOCK blocks
 *  
 *  \param [in] len_data_and_headers Data length
 *  \return Number of SE3_COMM_BLOCK blocks
 *  
 */
uint16_t se3_resp_len_data(uint16_t len_data_and_headers);

/**
 *  \brief Compute length of data in a response accounting for headers
 *  
 *  \param [in] len_data Data Length
 *  \return Number of Bytes
 *  
 */
uint16_t se3_resp_len_data_and_headers(uint16_t len_data);

/**
 *  \brief Compute number of SE3_COMM_BLOCK blocks, given length in Bytes
 *  
 *  \param [in] cap Length
 *  \return Number of Blocks
 *  
 */
uint16_t se3_nblocks(uint16_t len);

/*
enum {
	SE3_PAYL_CRYPTO = 1,
	SE3_PAYL_SIGN = 2,
	SE3_PAYL_CRYPTO_SIGN = 3
};*/









