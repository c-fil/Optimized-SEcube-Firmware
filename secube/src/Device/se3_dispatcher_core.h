#include "se3_security_core.h"
#include <se3c0def.h>
#include <stdbool.h>
#include "se3_keys.h"
// ---- L1 structures ----

#define SE3_CMD1_MAX (16)

enum {
	SE3_SECURITY_CORE,
	SE3_FPGA,
	SE3_SMARTCARD
}se3_algo_impl_t;

typedef struct se3_payload_cryptoctx_ {
	B5_tAesCtx aesenc;
    B5_tAesCtx aesdec;
	B5_tHmacSha256Ctx hmac;
	uint8_t hmac_key[B5_AES_256];
    uint8_t auth[B5_SHA256_DIGEST_SIZE];
} se3_payload_cryptoctx;

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

SE3_LOGIN_STATUS login;




typedef uint16_t(*se3_cmd_func)(uint16_t, const uint8_t*, uint16_t*, uint8_t*);





uint16_t L1d_config(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);


/** \brief L1 KEY_EDIT
 *
 *  Insert, delete or update a key
 */
uint16_t L1d_key_edit(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief L1 KEY_LIST
 *
 *  Get a list of keys in the device
 */
uint16_t L1d_key_list(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);


/** \brief L1 CHALLENGE command handler
 *
 *  Get a login challenge from the device
 */
uint16_t L1d_challenge(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief L1 LOGIN command handler
 *
 *  Respond to challenge and complete the login
 */
uint16_t L1d_login(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief L1 LOGOUT command handler
 *
 *  Log out and release resources
 */
uint16_t L1d_logout(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

uint16_t L1d_error(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

uint16_t L0d_cmd1(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);


/** \brief Clear login session data
 *
 *  Cleans all data associated with the login session, making SEcube ready for a new login.
 */
void se3c1_login_cleanup();

/** \brief Initialize L1 structures */
void se3_dispatcher_init();





void se3_payload_cryptoinit(se3_payload_cryptoctx* ctx, const uint8_t* key);
void se3_payload_encrypt(se3_payload_cryptoctx* ctx, uint8_t* auth, uint8_t* iv, uint8_t* data, uint16_t nblocks, uint16_t flags);
bool se3_payload_decrypt(se3_payload_cryptoctx* ctx, const uint8_t* auth, const uint8_t* iv, uint8_t* data, uint16_t nblocks, uint16_t flags);


static se3_cmd_func L1d_handlers[SE3_CMD1_MAX] = {
    /* 0  */ NULL,
    /* 1  */ L1d_challenge,
    /* 2  */ L1d_login,
    /* 3  */ L1d_logout,
    /* 4  */ L1d_config,
    /* 5  */ L1d_key_edit,
    /* 6  */ L1d_key_list,
    /* 7  */ L1d_crypto_init,
    /* 8  */ L1d_crypto_update,
    /* 9  */ L1d_crypto_list,
    /* 10 */ L1d_crypto_set_time,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ L1d_error
};



