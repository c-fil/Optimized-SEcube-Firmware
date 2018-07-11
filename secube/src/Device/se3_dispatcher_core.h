#include "se3_security_core.h"
//#include <se3c0def.h>
//#include <stdbool.h>
#include "se3_keys.h"
#include "se3_common.h"
#include "se3_rand.h"
#include "se3_sekey.h"

#define SE3_CMD1_MAX 	16
#define SE3_N_HARDWARE 	3

/** \brief L1 login status data */
typedef struct SE3_LOGIN_STATUS_ {
    bool y;  ///< logged in
    uint16_t access;  ///< access level
    uint16_t challenge_access;  ///< access level of the last offered challenge
    union {
        uint8_t token[SE3_L1_TOKEN_SIZE];   ///< login token
        uint8_t challenge[SE3_L1_CHALLENGE_SIZE];  ///< login challenge response expected
    };
    uint8_t key[SE3_L1_KEY_SIZE];  ///< session key for protocol encryption
    se3_payload_cryptoctx cryptoctx;  ///< context for protocol encryption
    bool cryptoctx_initialized;  ///< context initialized flag
} SE3_LOGIN_STATUS;

SE3_LOGIN_STATUS login_struct;


typedef uint16_t(*se3_cmd_func)(uint16_t, const uint8_t*, uint16_t*, uint8_t*);


uint16_t config(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);


/** \brief L1 KEY_EDIT
 *
 *  Insert, delete or update a key
 */
uint16_t key_edit(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief L1 KEY_LIST
 *
 *  Get a list of keys in the device
 */
uint16_t key_list(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);


/** \brief L1 CHALLENGE command handler
 *
 *  Get a login challenge from the device
 */
uint16_t challenge(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief L1 LOGIN command handler
 *
 *  Respond to challenge and complete the login
 */
uint16_t login(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief L1 LOGOUT command handler
 *
 *  Log out and release resources
 */
uint16_t logout(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

uint16_t error(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

uint16_t dispatcher_call(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief Clear login session data
 *
 *  Cleans all data associated with the login session, making SEcube ready for a new login.
 */
void login_cleanup();

/** \brief Initialize L1 structures */
void se3_dispatcher_init();

void set_req_hdr(se3c0_req_header req_hdr_i);

static se3_cmd_func handlers[SE3_N_HARDWARE][SE3_CMD1_MAX] = {{
//Security Core
    /* 0  */ NULL,
    /* 1  */ challenge,
    /* 2  */ login,
    /* 3  */ logout,
    /* 4  */ config,
    /* 5  */ key_edit,
    /* 6  */ key_list,
    /* 7  */ crypto_init,
    /* 8  */ crypto_update,
    /* 9  */ crypto_list,
    /* 10 */ crypto_set_time,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ error
}, {
//FPGA
    /* 0  */ NULL,
    /* 1  */ NULL,
    /* 2  */ NULL,
    /* 3  */ NULL,
    /* 4  */ NULL,
    /* 5  */ NULL,
    /* 6  */ NULL,
    /* 7  */ NULL,
    /* 8  */ NULL,
    /* 9  */ NULL,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL
}, {
//Smartcard
    /* 0  */ NULL,
    /* 1  */ NULL,
    /* 2  */ NULL,
    /* 3  */ NULL,
    /* 4  */ NULL,
    /* 5  */ NULL,
    /* 6  */ NULL,
    /* 7  */ NULL,
    /* 8  */ NULL,
    /* 9  */ NULL,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL
}};




