/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NFCM1833TINx_MODULE_H
#define __NFCM1833TINx_MODULE_H
#include "main.h"

#define ID_TAG_SIZE 8

typedef enum _error_code_protocol {
	PROTOCOL_OK               = 0x00,
	PROTOCOL_ERR	          = 0x01,
	INVALID_CMD_LEN           = 0x82,
	INVALID_PROTOCOL          = 0x83,
	NO_FIELD                  = 0x8F
} protocol_status_t;

typedef enum _error_code_sendrecv {
	SEND_RECV_OK  = 0x80,
	COMM_ERR      = 0x86,
	NO_TAG        = 0x87,
	INVALID_SOF   = 0x88,
	BUF_OVF       = 0x89,
	FRAME_ERR     = 0x8A,
	RECV_ERR      = 0x8E,
        CRC_ERR       = 0x01
} send_recv_status_t;
typedef enum _tag_type_4A_cmd{
        get_reqa_tag_type_4a_len = 4,
        reqa_response_size = 2,
        get_anticol_1_tag_type_4a_len = 5,
        anticol_1_response_size = 5,
        get_select_1_tag_type_4a_len = 10,
        select_1_response_size = 3,
        get_anticol_2_tag_type_4a_len = 5,
        anticol_2_response_size = 5,
        get_select_2_tag_type_4a_len = 10,
        select_2_response_size = 3,
} tag_type_4;
typedef enum _ping_code {
	PING_OK,
	PING_ERR
} ping_status;

typedef enum _pollfield_status {
	TAG_DETECTED = 0x01,
	TAG_NOT_FOUND = 0x00
} tag_status_t;

typedef enum _tag_type {
	FIELD_OFF	= 0x00,
	TYPE_5      = 0x01,
	ISO15693	= 0x01,
	TYPE_1      = 0x02,
	ISO14443A 	= 0x02,
	TYPE_4B     = 0x03,
	ISO14443B	= 0x03,
	TYPE_3		= 0x04,
	ISO18092	= 0x04
} tag_type_t;

typedef enum _cr95hf_cmd {
	IDN 			= 	0x01,
	PROTOCOLSELECT 	=	0x02,
	POLLFIELD  		= 	0x03,
	SENDRECV   		= 	0x04,
	LISTEN     		= 	0x05,
	SEND       		= 	0x06,
	IDLE       		= 	0x07,
	RDREG      		= 	0x08,
	WRREG      		= 	0x09,
	SUBFREQRES 		= 	0x0B,
	ACFILTER   		= 	0x0D,
	ECHO       		= 	0x55
} cr95hf_cmd_t;

typedef struct _cr95hf_recv_data {
	send_recv_status_t result_code;
	uint8_t data_len;
	uint8_t data_recv[250];
	uint8_t crc[2];
	uint8_t rfu;
} cr95hf_recv_format_t;



typedef struct _inventory_data_recv {
	uint8_t error_flag;
	uint8_t dsfid;
	uint8_t * UID;
} inventory_response_t;


protocol_status_t select_tag_type(tag_type_t tag_type);

send_recv_status_t sendrecv(uint8_t * command, uint8_t command_len, cr95hf_recv_format_t * data_result);

send_recv_status_t sendrecv_with_iso14443a(uint8_t * command, uint8_t command_len, cr95hf_recv_format_t * data_result);

ping_status ping_module(void);

tag_status_t check_tag_in_field(void);

send_recv_status_t getDeviceID(uint8_t * deviceID);

send_recv_status_t getDeviceID_with_iso14443a(uint8_t * deviceID);

void WakeUp_CR95HF (void);

#endif /*__NFCM1833TINx_MODULE_H*/