#include "nfcm1833tiny.h"
#include "string.h"

#define maxtimeout 1000U
#define uart_send(str) HAL_UART_Transmit(&uart_to_nfcm1833tiny,str,sizeof(str),maxtimeout)
#define uart_revc(str) HAL_UART_Receive(&uart_to_nfcm1833tiny,str,sizeof(str),maxtimeout)
#define uart_cancel()  HAL_UART_AbortReceive(&uart_to_nfcm1833tiny)

#define GETID_CMD       {0x04,0x03,0x26,0x01,0x00}
#define GETID_CMD_LEN   5
#define INVENTORY_RESPONSE_SIZE 10

extern UART_HandleTypeDef uart_to_nfcm1833tiny;
extern CRC_HandleTypeDef hcrc;
static protocol_status_t select_tag_type_uart_tag_type_5(void);
static uint16_t crcx25(const uint8_t *data, uint8_t len);

ping_status ping_module_uart(void)
{
	uint8_t ping_cmd_str[] = {ECHO};
	uint8_t ping_cmd_response[] = {0x00};
	
	if(uart_send(ping_cmd_str)== HAL_OK) {
		uart_revc(ping_cmd_response);
	} else
		return PING_ERR;
		
	if (ping_cmd_response[0] == ping_cmd_str[0])
		return PING_OK;
	else
		return PING_ERR;
}

protocol_status_t select_tag_type_uart(tag_type_t tag_type)
{
	switch (tag_type) {
	case TYPE_5:
		return select_tag_type_uart_tag_type_5();
		break;
		
	default:
		return PROTOCOL_ERR;
		break;
	}
}

protocol_status_t select_tag_type_uart_tag_type_5(void)
{
	uint8_t selec_tag_type5_cmd_str[] = {PROTOCOLSELECT,0x02,TYPE_5,0x0D};
	uint8_t selec_tag_type5_cmd_response[] = {0xFF,0xFF};
	
	if(uart_send(selec_tag_type5_cmd_str)!= HAL_OK) {
		return PROTOCOL_ERR;
	} else {
		if(uart_revc(selec_tag_type5_cmd_response)!= HAL_OK) {
			return PROTOCOL_ERR;
		}
	}
	
	return (protocol_status_t) (selec_tag_type5_cmd_response[0]|selec_tag_type5_cmd_response[1]);
}

send_recv_status_t sendrecv_uart(uint8_t * command, uint8_t command_len, cr95hf_recv_format_t * data_result)
{
	uint8_t temp[255]= {0};
	
	if(HAL_UART_Transmit(&uart_to_nfcm1833tiny,command,command_len,maxtimeout)!= HAL_OK) {
		uart_cancel();
	} else {
		HAL_UART_Receive(&uart_to_nfcm1833tiny,temp,2,maxtimeout);
		data_result->result_code=(send_recv_status_t)temp[0];
		
		if (data_result->result_code == 0x80) {
			data_result->data_len = temp[1] - 3;
                        HAL_UART_Receive(&uart_to_nfcm1833tiny,temp+2,temp[1]+3,maxtimeout);
			memcpy(data_result->data_recv,temp+2,data_result->data_len);
			data_result->crc[0] = temp[data_result->data_len+2];
			data_result->crc[1] = temp[data_result->data_len+3];
			data_result->rfu    = temp[data_result->data_len+4];
                        int16_t crc = data_result->crc[1] * 0x0100+ data_result->crc[0];
                        int16_t crc_cal = crcx25(data_result->data_recv,data_result->data_len);
                        if (crc != crc_cal) 
                        {
                          data_result->result_code = RECV_ERR;
                        }
		}
	}
	
	return data_result->result_code;
}

send_recv_status_t getDeviceID_uart(uint8_t * deviceID)
{
	uint8_t get_ID_cmd[] = GETID_CMD;
	cr95hf_recv_format_t data_recv;
	send_recv_status_t status = sendrecv_uart(get_ID_cmd,GETID_CMD_LEN,&data_recv);
	if (status == SEND_RECV_OK) memcpy(deviceID,data_recv.data_recv+data_recv.data_len - ID_TAG_SIZE,ID_TAG_SIZE);
	return status;
}

uint16_t crcx25(const uint8_t *data, uint8_t len)
{
	return HAL_CRC_Calculate(&hcrc, (uint32_t *)data, len) ^ 0xFFFF;
}