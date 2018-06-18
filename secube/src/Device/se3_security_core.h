/**
 *  \file se3c1.h
 *  \author Nicola Ferri
 *  \brief L1 structures and functions
 */

#pragma once

#include "se3_core_time.h"
#include "se3c1def.h"
#include "se3_memory.h"
#include "se3_keys.h"
#include "aes256.h"
#include "sha256.h"
#include "pbkdf2.h"

enum {
	SE3_SESSIONS_BUF = (32*1024),  ///< session buffer size
	SE3_SESSIONS_MAX = 100  ///< maximum number of sessions
};

// ---- records ----

enum {
	SE3_FLASH_TYPE_RECORD = 0xF0  ///< flash node type: record
};

/** \brief Record information */
typedef struct SE3_RECORD_INFO_ {
    uint16_t read_access;  ///< required access level for read
    uint16_t write_access;  ///< required access level for write
} SE3_RECORD_INFO;


typedef struct SE3_L1_GLOBALS_ {
    SE3_RECORD_INFO records[SE3_RECORD_MAX];
    se3_mem sessions;
    uint16_t sessions_algo[SE3_SESSIONS_MAX];
} SE3_L1_GLOBALS;


/** \brief L1_crypto_init function type */
typedef uint16_t(*se3_crypto_init_handler)(
	se3_flash_key* key, uint16_t mode, uint8_t* ctx);

/** \brief L1_crypto_update function type */
typedef uint16_t(*se3_crypto_update_handler)(
	uint8_t* ctx, uint16_t flags,
	uint16_t datain1_len, const uint8_t* datain1,
	uint16_t datain2_len, const uint8_t* datain2,
	uint16_t* dataout_len, uint8_t* dataout);

/** \brief algorithm descriptor type */
typedef struct se3_algo_descriptor_ {
	se3_crypto_init_handler init;  ///< L1_crypto_init function
	se3_crypto_update_handler update;  ///< L1_crypto_update function
	uint16_t size;  ///< context size size
	char display_name[16];  ///< name for the algorithm list API
	uint16_t display_type;  ///< type for the algorithm list API
	uint16_t display_block_size;  ///< block size for the algorithm list API
	uint16_t display_key_size;  ///< key size for the algorithm list API
} se3_algo_descriptor;


enum {
	SE3_RECORD_SIZE_TYPE = 2,  ///< record.type field size
	SE3_RECORD_OFFSET_TYPE = 0, ///< record.type field offset
	SE3_RECORD_OFFSET_DATA = 2, ///< record.data field offset
};

/** \brief Write record
 *
 *  Set data of a record
 *  \remark if a flash operation fails, the hwerror flag (se3c0.hwerror) is set.
 *  \param type type of record
 *  \param data new data to be written to record
 *  \return true on success; false if the record does not exist
 */
bool se3c1_record_set(uint16_t type, const uint8_t* data);

/** \brief Read record
*
*  Get data of a record.
*  \param type type of record
*  \param data output buffer
*  \return true on success; false if the record does not exist or has never been written
*/
bool se3c1_record_get(uint16_t type, uint8_t* data);


// ---- crypto ----

/** \brief L1 CRYPTO_INIT handler
 *
 *  Initialize a cryptographic context
 */
uint16_t L1d_crypto_init(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief L1 CRYPTO_UPDATE handler
 *
 *  Use a cryptographic context
 */
uint16_t L1d_crypto_update(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief L1 CRYPTO_SET_TIME handler
 *
 *  Set device time for key validity
 */
uint16_t L1d_crypto_set_time(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief L1 CRYPTO_SET_TIME handler
 *
 *  Get list of available algorithms
 */
uint16_t L1d_crypto_list(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

void se3_security_core_init();

/** \brief L1 globals */
 SE3_L1_GLOBALS se3c1;


/** session buffer */
extern uint8_t se3_sessions_buf[SE3_SESSIONS_BUF];

/** session index */
extern uint8_t* se3_sessions_index[SE3_SESSIONS_MAX];

/** algorithm description table */
extern se3_algo_descriptor L1d_algo_table[SE3_ALGO_MAX];
