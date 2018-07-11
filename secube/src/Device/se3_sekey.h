//SEKEY
#include "se3_keys.h"
#include "se3_security_core.h"
#include <stdint.h>
#include <stdbool.h>

enum {
	SE3_SECURITY_CORE,
	SE3_FPGA,
	SE3_SMARTCARD
}se3_algo_impl_t;


bool sekey_get_implementation(uint8_t* algo_implementation, uint8_t algo_index);
