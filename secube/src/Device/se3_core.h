 #pragma once


#include <se3c0def.h>


#if defined(_MSC_VER)
#define SE3_ALIGN_16 __declspec(align(0x10))
#elif defined(__GNUC__)
#define SE3_ALIGN_16 __attribute__((aligned(0x10)))
#else
#define SE3_ALIGN_16
#endif


/** L0 command handler */


void device_init();

void device_loop();


/** \brief Execute received command
 *
 *  Process the last received request and produce a response
 */
void se3_cmd_execute();


/** \brief L0 ECHO command handler
 *
 *  Send back received data
 */
uint16_t echo(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief L0 FACTORY_INIT command handler
 *
 *  Initialize device's serial number
 */
uint16_t factory_init(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);


/** \brief L0 FACTORY_INIT command handler
 *
 *  Reset USEcube to boot mode
 */
uint16_t bootmode_reset(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);




