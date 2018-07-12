//SEKEY
#include "se3_keys.h"
#include <stdint.h>
#include <stdbool.h>

#include "se3_security_core.h"
#include "se3_smartcard.h"
#include "se3_FPGA.h"


enum {
	SE3_SECURITY_CORE,
	SE3_FPGA,
	SE3_SMARTCARD
}se3_algo_impl_t;




bool sekey_get_implementation_info(uint8_t* algo_implementation, uint8_t* crypto_algo, uint8_t* key);

bool sekey_get_auth(uint8_t *key);
