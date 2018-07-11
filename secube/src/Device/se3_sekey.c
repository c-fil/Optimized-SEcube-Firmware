#include "se3_sekey.h"

/*	sekey_get_implementation: This function would be the core of the sekey behavior,
 * 	it has to be implemented. The provided code is just a stub	*/

bool sekey_get_implementation(uint8_t* algo_implementation, uint8_t algo_index){
	/*	a function that, given an index, chooses among the possible implementations that the key allows you
	 * 	to perform is needed. The key must be searched by using se3_keys.c
	 */

	*algo_implementation = SE3_SECURITY_CORE;

	return true;
}

