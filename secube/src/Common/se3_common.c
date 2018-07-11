#include "se3_common.h"
#include "se3_sdio.h"

const uint8_t se3_magic[SE3_MAGIC_SIZE] = {
    0x3c, 0xab, 0x78, 0xb6, 0x2, 0x64, 0x47, 0xe9, 0x30, 0x26, 0xd4, 0x1f, 0xad, 0x68, 0x22, 0x27,
    0x41, 0xa4, 0x32, 0xba, 0xbe, 0x54, 0x83, 0xee, 0xab, 0x6b, 0x62, 0xce, 0xf0, 0x5c, 0x7, 0x91
};

//########################DEBUG##############################

#define SE3_DEBUG_SD3

#ifdef SE3_DEBUG_SD3

//int debug_count = 0;

int buff_len(uint8_t* buff){
	int n = 0;
	while(buff[n++]!='\0');
	return n;
}

bool MYPRINTF( uint8_t* buf, uint32_t blk_addr){
	bool ret;

	int i,n=0;

	while(buf[n++]!='\0');

	for(i=n;i<512;i++)
		buf[i]=0;

	ret = secube_sdio_write(512, buf, blk_addr, 1);

	return ret;
}

bool sd_flush(){
	debug_count = 0;
	int i;
	int *buf;
	buf = calloc(512,sizeof(int));

	for(i = BASE_DEBUG_ADDRESS; i < BASE_DEBUG_ADDRESS + 1963; i++)
		if((secube_sdio_write(512, buf, i, 1)) == false)
			return false;
	return true;
}

#endif

//############################################################################

uint16_t se3_req_len_data(uint16_t len_data_and_headers)
{
    uint16_t nblocks;
    if (len_data_and_headers < SE3_REQ_SIZE_HEADER) {
        return 0;
    }
    nblocks = len_data_and_headers/SE3_COMM_BLOCK;
    if (len_data_and_headers % SE3_COMM_BLOCK != 0) {
        nblocks++;
    }
    if (nblocks == 0)return 0;
    return len_data_and_headers - SE3_REQ_SIZE_HEADER - (nblocks - 1)*SE3_REQDATA_SIZE_HEADER;
}

uint16_t se3_req_len_data_and_headers(uint16_t len_data)
{
    uint16_t ndatablocks;
    if (len_data <= SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER) {
        return len_data + SE3_REQ_SIZE_HEADER;
    }
    len_data -= (SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER);
    ndatablocks = len_data / (SE3_COMM_BLOCK - SE3_REQDATA_SIZE_HEADER);
    if (len_data % (SE3_COMM_BLOCK - SE3_REQDATA_SIZE_HEADER) != 0) {
        ndatablocks++;
    }
    return SE3_COMM_BLOCK + len_data + ndatablocks*SE3_REQDATA_SIZE_HEADER;
}

uint16_t se3_resp_len_data(uint16_t len_data_and_headers)
{
    uint16_t nblocks;
    if (len_data_and_headers < SE3_RESP_SIZE_HEADER) {
        return 0;
    }
    nblocks = len_data_and_headers / SE3_COMM_BLOCK;
    if (len_data_and_headers % SE3_COMM_BLOCK != 0) {
        nblocks++;
    }
    if (nblocks == 0)return 0;
    return len_data_and_headers - SE3_RESP_SIZE_HEADER - (nblocks - 1)*SE3_RESPDATA_SIZE_HEADER;
}

uint16_t se3_resp_len_data_and_headers(uint16_t len_data)
{
    uint16_t ndatablocks;
    if (len_data <= SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER) {
        return len_data + SE3_RESP_SIZE_HEADER;
    }
    len_data -= (SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER);
    ndatablocks = len_data / (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER);
    if (len_data % (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER) != 0) {
        ndatablocks++;
    }
    return SE3_COMM_BLOCK + len_data + ndatablocks*SE3_RESPDATA_SIZE_HEADER;
}

uint16_t se3_nblocks(uint16_t len)
{
    uint16_t nblocks = len / SE3_COMM_BLOCK;
    if (len%SE3_COMM_BLOCK != 0) {
        nblocks++;
    }
    return nblocks;
}

