/**
 *  \file se3_cmd.c
 *  \author Nicola Ferri
 *  \brief L0 command dispatch and execute
 */

#include "se3_core.h"
#include "se3_communication_core.h"
#include "se3_dispatcher_core.h"

#include "crc16.h"
#include "se3_rand.h"


#define SE3_FLASH_SIGNATURE_ADDR  ((uint32_t)0x08020000)
#define SE3_FLASH_SIGNATURE_SIZE  ((size_t)0x40)







uint8_t se3_sessions_buf[SE3_SESSIONS_BUF];
uint8_t* se3_sessions_index[SE3_SESSIONS_MAX];
//SE3_L1_GLOBALS se3c1;

void device_init()
{
	se3_communication_core_init();
	se3_time_init();
	se3_flash_init();
    se3_dispatcher_init();
}

void device_loop()
{
	for (;;) {
		if (comm.req_ready) {
			comm.resp_ready = false;
            se3_cmd_execute();
			comm.req_ready = false;
			comm.resp_ready = true;
		}
	}
}


static uint16_t invalid_cmd_handler(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    return SE3_ERR_CMD;
}

static uint16_t se3_exec(se3_cmd_func handler)
{
    uint16_t resp_size = 0, tmp;
    uint16_t status = SE3_OK;
    uint16_t nblocks = 0;
    uint16_t data_len;
#if SE3_CONF_CRC
	uint16_t crc;
	uint16_t u16tmp;
#endif

    data_len = se3_req_len_data(req_hdr.len);

#if SE3_CONF_CRC
	// compute CRC
	crc = se3_crc16_update(SE3_REQ_OFFSET_CRC, comm.req_hdr, 0);
	if (data_len > 0) {
		crc = se3_crc16_update(data_len, comm.req_data, crc);
	}
	if (req_hdr.crc != crc) {
		status = SE3_ERR_COMM;
		resp_size = 0;
	}
#endif

	if(status == SE3_OK) {
		status = handler(data_len, comm.req_data, &resp_size, comm.resp_data);
	}

    if (hwerror) {
        status = SE3_ERR_HW;
        resp_size = 0;
        hwerror = false;
    }
    else if (resp_size > SE3_RESP_MAX_DATA) {
        status = SE3_ERR_HW;
        resp_size = 0;
    }

    resp_hdr.status = status;

    if (resp_size <= SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER) {
        nblocks = 1;
        // zero unused memory
        memset(
            comm.resp_data + resp_size, 0,
            SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER - resp_size);
    }
    else {
        tmp = resp_size - (SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER);
        nblocks = 1 + tmp / (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER);
        if (tmp % (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER) != 0) {
            nblocks++;
            // zero unused memory
            memset(
                comm.resp_data + resp_size, 0,
                (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER) - (tmp % (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER)));
        }
    }

	resp_hdr.len = se3_resp_len_data_and_headers(resp_size);

#if SE3_CONF_CRC
	u16tmp = 1;
	SE3_SET16(comm.resp_hdr, SE3_RESP_OFFSET_READY, u16tmp);
	SE3_SET16(comm.resp_hdr, SE3_RESP_OFFSET_STATUS, status);
	SE3_SET16(comm.resp_hdr, SE3_RESP_OFFSET_LEN, resp_hdr.len);
	SE3_SET32(comm.resp_hdr, SE3_RESP_OFFSET_CMDTOKEN, req_hdr.cmdtok[0]);
	crc = se3_crc16_update(SE3_REQ_OFFSET_CRC, comm.resp_hdr, 0);
	if (resp_size > 0) {
		crc = se3_crc16_update(resp_size, comm.resp_data, crc);
	}
	resp_hdr.crc = crc;
#endif

    return nblocks;
}

void se3_cmd_execute()
{
    uint16_t req_blocks = 1, resp_blocks = 1;
    size_t i;
    se3_cmd_func handler = NULL;
	uint32_t cmdtok0;

    req_blocks = req_hdr.len / SE3_COMM_BLOCK;
    if (req_hdr.len % SE3_COMM_BLOCK != 0) {
        req_blocks++;
    }
    if (req_blocks > SE3_COMM_N - 1) {
        // should not happen anyway
        resp_blocks = 0;
        goto update_comm;
    }
    for (i = 1; i < req_blocks; i++) {
        if (req_hdr.cmdtok[i] != req_hdr.cmdtok[i - 1] + 1) {
            resp_blocks = 0;
            goto update_comm;
        }
    }

	if (handler == NULL) {
		switch (req_hdr.cmd) {
		case SE3_CMD0_MIX:
			handler = L0d_cmd1;
			break;
		case SE3_CMD0_ECHO:
			handler = L0d_echo;
			break;
		case SE3_CMD0_FACTORY_INIT:
			handler = L0d_factory_init;
			break;
		case SE3_CMD0_BOOT_MODE_RESET:
			handler = L0d_bootmode_reset;
			break;
		default:
			handler = invalid_cmd_handler;
		}
	}

    resp_blocks = se3_exec(handler);

    // set cmdtok
	cmdtok0 = req_hdr.cmdtok[0];
    for (i = 0; i < resp_blocks; i++) {
        resp_hdr.cmdtok[i] = cmdtok0;
		cmdtok0++;
    }


update_comm:
    // update comm response bit map
    comm.resp_bmap = SE3_BMAP_MAKE(resp_blocks);
}

/**
 *  \file se3_cmd0.c
 *  \author Nicola Ferri
 *  \brief L0 command handlers
 */




uint16_t L0d_echo(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    memcpy(resp, req, req_size);
    *resp_size = req_size;
    return SE3_OK;
}

uint16_t L0d_factory_init(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    enum {
        OFF_SERIAL = 0
    };
    const uint8_t* serial_tmp = req + OFF_SERIAL;
    se3_flash_it it;

	if (serial.written) {
		return SE3_ERR_STATE;
	}

    se3_flash_it_init(&it);
    if (!se3_flash_it_new(&it, SE3_FLASH_TYPE_SERIAL, SE3_SERIAL_SIZE)) {
        return SE3_ERR_HW;
    }
    if (!se3_flash_it_write(&it, 0, serial_tmp, SE3_SERIAL_SIZE)) {
        return SE3_ERR_HW;
    }

    memcpy(serial.data, serial_tmp, SE3_SERIAL_SIZE);
    serial.written = true;
    return SE3_OK;
}


uint16_t L0d_bootmode_reset(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{

	if(!(se3_flash_bootmode_reset(SE3_FLASH_SIGNATURE_ADDR, SE3_FLASH_SIGNATURE_SIZE)))
		return SE3_ERR_HW;

	return SE3_OK;
}
/** \brief L0 command which executes an L1 command
 *
 *  This handler also manages encryption and login token check
 */

uint16_t L0d_cmd1(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    se3_cmd_func handler = NULL;
    uint16_t resp1_size, req1_size;
    uint16_t resp1_size_padded;
    const uint8_t* req1;
    uint8_t* resp1;
    uint16_t status;
    struct {
        const uint8_t* auth;
        const uint8_t* iv;
        const uint8_t* token;
        uint16_t len;
        uint16_t cmd;
        const uint8_t* data;
    } req_params;
    struct {
        uint8_t* auth;
        uint8_t* iv;
        uint8_t* token;
        uint16_t len;
        uint16_t status;
        uint8_t* data;
    } resp_params;

    req_params.auth = req + SE3_REQ1_OFFSET_AUTH;
    req_params.iv = req + SE3_REQ1_OFFSET_IV;
    req_params.token = req + SE3_REQ1_OFFSET_TOKEN;
    req_params.data = req + SE3_REQ1_OFFSET_DATA;

    if (req_size < SE3_REQ1_OFFSET_DATA) {
        SE3_TRACE(("[L0d_cmd1] insufficient req size\n"));
        return SE3_ERR_COMM;
    }

    // prepare request
    if (!login.cryptoctx_initialized) {
        se3_payload_cryptoinit(&(login.cryptoctx), login.key);
        login.cryptoctx_initialized = true;
    }
    if (!se3_payload_decrypt(
        &(login.cryptoctx), req_params.auth, req_params.iv,
        /* !! modifying request */ (uint8_t*)(req  + SE3_L1_AUTH_SIZE + SE3_L1_IV_SIZE),
        (req_size - SE3_L1_AUTH_SIZE - SE3_L1_IV_SIZE) / SE3_L1_CRYPTOBLOCK_SIZE, req_hdr.cmd_flags))
    {
        SE3_TRACE(("[L0d_cmd1] AUTH failed\n"));
        return SE3_ERR_COMM;
    }

    if (login.y) {
        if (memcmp(login.token, req_params.token, SE3_L1_TOKEN_SIZE)) {
            SE3_TRACE(("[L0d_cmd1] login token mismatch\n"));
            return SE3_ERR_ACCESS;
        }
    }


    SE3_GET16(req, SE3_REQ1_OFFSET_LEN, req_params.len);
    SE3_GET16(req, SE3_REQ1_OFFSET_CMD, req_params.cmd);
    if (req_params.cmd < SE3_CMD1_MAX) {
    	if (req_params.cmd > 6 && req_params.cmd < 11 && !login.y) {   //
    		SE3_TRACE(("[L1d_crypto_init] not logged in\n"));		   //  TODO: ADDED BY US
    		return SE3_ERR_ACCESS;                                     //
    	}															   //
        handler = L1d_handlers[req_params.cmd];
    }
    if (handler == NULL) {
        handler = L1d_error;
    }

    req1 = req_params.data;
    req1_size = req_params.len;
    resp1 = resp + SE3_RESP1_OFFSET_DATA;
    resp1_size = 0;

    status = handler(req1_size, req1, &resp1_size, resp1);

    resp_params.len = resp1_size;
    resp_params.auth = resp + SE3_RESP1_OFFSET_AUTH;
    resp_params.iv = resp + SE3_RESP1_OFFSET_IV;
    resp_params.token = resp + SE3_RESP1_OFFSET_TOKEN;
    resp_params.status = status;
    resp_params.data = resp1;

    resp1_size_padded = resp1_size;
    if (resp1_size_padded % SE3_L1_CRYPTOBLOCK_SIZE != 0) {
        memset(resp1 + resp1_size_padded, 0, (SE3_L1_CRYPTOBLOCK_SIZE - (resp1_size_padded % SE3_L1_CRYPTOBLOCK_SIZE)));
        resp1_size_padded += (SE3_L1_CRYPTOBLOCK_SIZE - (resp1_size_padded % SE3_L1_CRYPTOBLOCK_SIZE));
    }

    *resp_size = SE3_RESP1_OFFSET_DATA + resp1_size_padded;

    // prepare response
    SE3_SET16(resp, SE3_RESP1_OFFSET_LEN, resp_params.len);
    SE3_SET16(resp, SE3_RESP1_OFFSET_STATUS, resp_params.status);
    if (login.y) {
        memcpy(resp + SE3_RESP1_OFFSET_TOKEN, login.token, SE3_L1_TOKEN_SIZE);
    }
    else {
        memset(resp + SE3_RESP1_OFFSET_TOKEN, 0, SE3_L1_TOKEN_SIZE);
    }
	if (req_hdr.cmd_flags & SE3_CMDFLAG_ENCRYPT) {
		se3_rand(SE3_L1_IV_SIZE, resp_params.iv);
	}
	else {
		memset(resp_params.iv, 0, SE3_L1_IV_SIZE);
	}
    se3_payload_encrypt(
        &(login.cryptoctx), resp_params.auth, resp_params.iv,
        resp + SE3_L1_AUTH_SIZE + SE3_L1_IV_SIZE, (*resp_size - SE3_L1_AUTH_SIZE - SE3_L1_IV_SIZE) / SE3_L1_CRYPTOBLOCK_SIZE, req_hdr.cmd_flags);



    return SE3_OK;
}



